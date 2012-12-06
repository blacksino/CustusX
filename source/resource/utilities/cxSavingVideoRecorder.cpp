// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxSavingVideoRecorder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageLuminance.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "sscXmlOptionItem.h"

namespace cx
{

VideoRecorderSaveThread::VideoRecorderSaveThread(QObject* parent, QString saveFolder, QString prefix, bool compressed, bool writeColor) :
	QThread(parent),
	mSaveFolder(saveFolder),
	mPrefix(prefix),
	mImageIndex(0),
	mMutex(QMutex::Recursive),
	mStop(false),
	mCancel(false),
	mTimestampsFile(saveFolder+"/"+prefix+".fts"),
	mCompressed(compressed),
	mWriteColor(writeColor)
{
//	std::cout << "**VideoRecorderSaveThread::VideoRecorderSaveThread()" << std::endl;
}

VideoRecorderSaveThread::~VideoRecorderSaveThread()
{
//	std::cout << "**VideoRecorderSaveThread::~VideoRecorderSaveThread()" << std::endl;
}

QString VideoRecorderSaveThread::addData(double timestamp, vtkImageDataPtr image)
{
	if (!image)
		return "";

	DataType data;
	data.mTimestamp = timestamp;
	data.mImage = vtkImageDataPtr::New();
	data.mImage->DeepCopy(image);
	data.mImageFilename = QString("%1/%2_%3.mhd").arg(mSaveFolder).arg(mPrefix).arg(mImageIndex++);

	{
		QMutexLocker sentry(&mMutex);
		mPendingData.push_back(data);
	}

	return data.mImageFilename;
}

void VideoRecorderSaveThread::stop()
{
	mStop = true;
}

void VideoRecorderSaveThread::cancel()
{
	mCancel = true;
	mStop = true;
}

bool VideoRecorderSaveThread::openTimestampsFile()
{
	if(!mTimestampsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
	  ssc::messageManager()->sendError("Cannot open "+mTimestampsFile.fileName());
	  return false;
	}
	return true;
}


bool VideoRecorderSaveThread::closeTimestampsFile()
{
	mTimestampsFile.close();

	QFileInfo info(mTimestampsFile);
	if (!mCancel)
	{
		ssc::messageManager()->sendInfo(QString("Saved %1 timestamps to file %2")
										.arg(mImageIndex)
										.arg(info.fileName()));
	}
	return true;
}

void VideoRecorderSaveThread::write(VideoRecorderSaveThread::DataType data)
{
	// write timestamp
	QTextStream stream(&mTimestampsFile);
	stream << qstring_cast(data.mTimestamp);
	stream << endl;

	// convert to 8 bit data if applicable.
	if (!mWriteColor && data.mImage->GetNumberOfScalarComponents()>2)
	{
		  vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		  luminance->SetInput(data.mImage);
		  data.mImage = luminance->GetOutput();
		  data.mImage->Update();
	}

	// write image
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInput(data.mImage);
	writer->SetFileName(cstring_cast(data.mImageFilename));
	writer->SetCompression(mCompressed);
	writer->Write();
//	std:cout << "** VideoRecorderSaveThread::write() " << data.mImageFilename << std::endl;

}

/** Write all pending images to file.
  *
  */
void VideoRecorderSaveThread::writeQueue()
{
	while(!mPendingData.empty())
	{
		if (mCancel)
			return;

		DataType current;

		{
			QMutexLocker sentry(&mMutex);
			current = mPendingData.front();
			mPendingData.pop_front();
		}

		this->write(current);
	}
}

void VideoRecorderSaveThread::run()
{
	this->openTimestampsFile();
	while (!mStop)
	{
		this->writeQueue();
		this->msleep(20);
	}

	this->writeQueue();
	this->closeTimestampsFile();
}


///** Merge all us frames into one vtkImageData
// *
// */
//vtkImageDataPtr USFrameData::mergeFrames(std::vector<vtkImageDataPtr> input) const
//{
//  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
//  filter->SetAppendAxis(2); // append along z-axis

//  for (unsigned i=0; i<input.size(); ++i)
//    filter->SetInput(i, input[i]);

//  filter->Update();
//  return filter->GetOutput();
//}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


SavingVideoRecorder::SavingVideoRecorder(ssc::VideoSourcePtr source, QString saveFolder, QString prefix, bool compressed, bool writeColor) :
	mSource(source)
{
//	std::cout << "**SavingVideoRecorder::SavingVideoRecorder()" << std::endl;
	mSaveFolder = saveFolder;
	mSaveThread.reset(new VideoRecorderSaveThread(NULL, saveFolder, prefix, compressed, writeColor));
	mSaveThread->start();
}

SavingVideoRecorder::~SavingVideoRecorder()
{
//	std::cout << "**SavingVideoRecorder::~SavingVideoRecorder()" << std::endl;
	mSaveThread->stop();
}

void SavingVideoRecorder::startRecord()
{
	connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void SavingVideoRecorder::stopRecord()
{
	disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void SavingVideoRecorder::newFrameSlot()
{
	if (!mSource->validData())
		return;

	vtkImageDataPtr image = mSource->getVtkImageData();
	double timestamp = mSource->getTimestamp();
	QString filename = mSaveThread->addData(timestamp, image);

	vtkImageDataPtr frame = vtkImageDataPtr::New();
	frame->DeepCopy(image);

	// numbers in Kb
	int currentMem = frame->GetActualMemorySize() * mImages.size();
	int maxMem = 2*1000*1000; // store max 2Gb of data before clearing cache
//	std::cout << "memused (" << mImages.size() << ") :"<< currentMem/1000 << "." << currentMem << std::endl;

	bool discardImage = (currentMem > maxMem);

	if (discardImage)
		frame = vtkImageDataPtr();

	CachedImageDataPtr cache(new CachedImageData(filename, frame));
	mImages.push_back(cache);
	mTimestamps.push_back(timestamp);
}

std::vector<CachedImageDataPtr> SavingVideoRecorder::getImageData()
{
	return mImages;
}

std::vector<double> SavingVideoRecorder::getTimestamps()
{
	return mTimestamps;
}

void SavingVideoRecorder::cancel()
{
	mSaveThread->cancel();
	mSaveThread->wait(); // wait indefinitely for thread to finish

	//TODO: delete contents
	this->deleteFolder(mSaveFolder);
}

/** Delete folder and all contents that have been written by savers.
  */
void SavingVideoRecorder::deleteFolder(QString folder)
{
	QStringList filters;
	filters << "*.fts" << "*.mhd" << "*.raw" << "*.zraw";

	QDir dir(folder);
	QStringList files = dir.entryList(filters);

	for (int i=0; i<files.size(); ++i)
		dir.remove(files[i]);
	dir.rmdir(folder);
}

//SavingVideoRecorder::DataType SavingVideoRecorder::getRecording()
//{
//	return mData;
//}

void SavingVideoRecorder::completeSave()
{
	mSaveThread->stop();
	mSaveThread->wait(); // wait indefinitely for thread to finish
}

} // namespace cx

