#include "sscSlicedImageProxy.h"

#include <boost/lexical_cast.hpp>
#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

#include "sscView.h"
#include "sscDataManager.h"
#include "sscSliceProxy.h"

namespace ssc
{


SlicedImageProxy::SlicedImageProxy()
{
	mMatrixAxes = vtkMatrix4x4Ptr::New();
	//mWindowLevel = vtkImageMapToWindowLevelColorsPtr::New();

	mReslicer = vtkImageReslicePtr::New();
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality( 2);
	mReslicer->SetResliceAxes(mMatrixAxes);
	mReslicer->SetAutoCropOutput(false);

	mWindowLevel = vtkImageMapToColorsPtr::New();
	mWindowLevel->SetInputConnection( mReslicer->GetOutputPort() );
	mWindowLevel->SetOutputFormatToRGBA();
}

SlicedImageProxy::~SlicedImageProxy()
{
}

void SlicedImageProxy::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}	
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}
}

void SlicedImageProxy::setImage(ImagePtr image)
{
	mImage = image;
	if (mImage)
	{
#ifdef USE_TRANSFORM_RESCLICER
		mReslicer->SetInput(mImage->getRefVtkImageData());
#else
		mReslicer->SetInput(mImage->getBaseVtkImageData());
#endif
		//mWindowLevel->SetInputConnection( mReslicer->GetOutputPort() );
		//mWindowLevel->SetOutputFormatToRGBA();
		mWindowLevel->SetLookupTable(image->getLookupTable2D().getLookupTable());
		mWindowLevel->Update();
	}

//	if (mImage)
//	{
//		disconnect( mImage.get(), SIGNAL(alphaChange()), this, SIGNAL(updateAlpha()));
//	}

//	connect( mImage.get(), SIGNAL(alphaChange()), this, SIGNAL(updateAlpha()));

}

ImagePtr SlicedImageProxy::getImage()const
{
	return mImage;
}

//deliver the sliced image..
vtkImageDataPtr SlicedImageProxy::getOutput()
{
	return mWindowLevel->GetOutput();
}

void SlicedImageProxy::update()
{
	if (!mSlicer || !mImage)
		return;

	Transform3D rMs = mSlicer->get_sMr().inv();
	Transform3D iMr = mImage->get_rMd().inv();
	Transform3D M = iMr*rMs;

#ifdef USE_TRANSFORM_RESCLICER
	mMatrixAxes->DeepCopy(rMs.matrix());
#else
	mMatrixAxes->DeepCopy(M.matrix());
#endif

	//	Transform3D rMs = mSlicer->get_sMr().inv();
	//	mMatrixAxes->DeepCopy(rMs.matrix());
}

void SlicedImageProxy::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}

void SlicedImageProxy::printSelf(std::ostream & os, Indent indent)
{
	//os << indent << "PlaneType: " << mType << std::endl;
	os << indent << "mImage: " << (mImage ? mImage->getUid() : "NULL") << std::endl;
	os << indent << "mSlicer: " << (mSlicer ? mSlicer.get() : 0) << std::endl;
	if (mSlicer)
	{
		mSlicer->printSelf(os, indent.stepDown());
	}
	os << indent << "mReslicer->GetOutput(): " << mReslicer->GetOutput() << std::endl;
	os << indent << "mReslicer->GetInput() : " << mReslicer->GetInput() << std::endl;
	Transform3D test(mReslicer->GetResliceAxes());
	os << indent << "resliceaxes: " << std::endl;
	test.put(os, indent.getIndent()+3);
	os << std::endl;
	//os << indent << "rMs_debug: " << std::endl;
	//rMs_debug.put(os, indent.getIndent()+3);

}


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------




