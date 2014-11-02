/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxLapFrameToolCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxToolManager.h"
#include "cxVector3D.h"
#include "cxDefinitionStrings.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDataLocations.h"
#include "cxTool.h"
#include "cxToolManager.h"
#include "cxPatientData.h"
#include <cxActiveToolWidget.h>
#include "cxDoubleWidgets.h"

//TODO: remove
#include "cxLegacySingletons.h"

namespace cx
{

//------------------------------------------------------------------------------
LapFrameToolCalibrationWidget::LapFrameToolCalibrationWidget(QWidget* parent) :
    BaseWidget(parent, "LapFrameToolCalibrationWidget", "LapFrame Calibrate"),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mTestButton(new QPushButton("Test calibration")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mCameraAngleAdapter = DoubleDataAdapterXml::initialize("Camera Angle", "",
      "Additional tilt of calibration around tool y-axis,\nfor use with cameras tilted relative to tool direction",
      0.0, DoubleRange(-M_PI/2, M_PI/2, M_PI/180), 0);
  mCameraAngleAdapter->setInternal2Display(180.0/M_PI);

  mCalibRefTool = SelectToolStringDataAdapter::New();
  mCalibRefTool->setValueName("Calibration Frame");
  mCalibRefTool->setHelp("Select Calibration Reference Frame");

  mCalibratingTool = SelectToolStringDataAdapter::New();
  mCalibratingTool->setValueName("Tool");
  mCalibratingTool->setHelp("Select which Tool to calibrate");

  this->setToolTip(this->defaultWhatsThis());

//  toplayout->addWidget(new QLabel("<b>Select a tool with a known reference point:</b>"));
  toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibRefTool));
  toplayout->addWidget(mReferencePointLabel);
  toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibratingTool));
//  toplayout->addWidget(new ActiveToolWidget(this));
  toplayout->addWidget(new SpinBoxAndSliderGroupWidget(this, mCameraAngleAdapter));
  toplayout->addWidget(mCalibrateButton);
  toplayout->addWidget(mCalibrationLabel);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mTestButton);
  toplayout->addWidget(mDeltaLabel);
  toplayout->addStretch();

  mReferencePointLabel->setText("<i> Use only with the special Laparascopic <br>"
	                                "calibration frame as reference. </i>");

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testCalibrationSlot()));

  connect(mCalibRefTool.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

  //setting default state
  this->toolSelectedSlot();

  connect(cx::toolManager(), &cx::ToolManager::stateChanged, this, &LapFrameToolCalibrationWidget::trackingStartedSlot);
//  connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(trackingStartedSlot()));
}

LapFrameToolCalibrationWidget::~LapFrameToolCalibrationWidget()
{}

QString LapFrameToolCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Laparascopic tool calibration</h3>"
      "<p><i>Calibrates a tool by sampling it when it is inserted into the slot of the custom-made calibration frame.</i></br>"
      "<p><i>The tool to be calibrated will have its calibration set in such a way that the calibrating and calibrated tools"
      "are in the same position.</i></br>"
      "<p>By using the test button you can test your calibration by pointing at a known reference point.</br></p>"
      "</html>";
}

void LapFrameToolCalibrationWidget::calibrateSlot()
{
  ToolPtr refTool = mCalibRefTool->getTool();
  ToolPtr tool = mCalibratingTool->getTool();
  double cameraAngle = mCameraAngleAdapter->getValue();
  if(!refTool || !tool)
  {
    reportError(QString("Calibration prerequisited not met: calref:%1, tool:%2").arg(refTool!=0).arg(tool!=0) );
    return;
  }
  if(!refTool->getVisible() || !tool->getVisible() || !refTool->hasReferencePointWithId(1))
  {
    reportError(QString("Calibration prerequisited not met: calref vis:%1, tool vis :%2, refpoint:%3").arg(refTool->getVisible()).arg(tool->getVisible()).arg(refTool->hasReferencePointWithId(1)) );
    return;
  }

  LapFrameToolCalibrationCalculator calc(tool, refTool, cameraAngle);
  Transform3D calibration = calc.get_calibration_sMt();

  QMessageBox msgBox;
  msgBox.setText("Do you want to overwrite "+tool->getName()+"'s calibration file?");
  msgBox.setInformativeText("This cannot be undone.");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Ok)
  {
    tool->setCalibration_sMt(calibration);
//    std::stringstream ss;
//    ss << calibration.matrix().format(Eigen::IOFormat()) << std::endl;
    mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
  }
}

void LapFrameToolCalibrationWidget::testCalibrationSlot()
{
  ToolPtr refTool = mCalibRefTool->getTool();
  ToolPtr tool = mCalibratingTool->getTool();
  double cameraAngle = mCameraAngleAdapter->getValue();

  if(!refTool || !tool || !refTool->hasReferencePointWithId(1))
    return;

  LapFrameToolCalibrationCalculator calc(tool, refTool, cameraAngle);
  Vector3D delta_selectedTool = calc.get_delta_ref();

  QString delta = QString("%1 mm").arg(delta_selectedTool.length(), 6, 'g', 1);
  mDeltaLabel->setText("<b>Delta "+tool->getName()+":</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Accuracy:</b>  " + delta);

  report("Delta "+tool->getName()+": "+qstring_cast(delta_selectedTool)+" Length:   "+ delta);


}

void LapFrameToolCalibrationWidget::toolSelectedSlot()
{
	//  QString text("Ref. point: <UNDEFINED POINT>");
	mCalibrateButton->setEnabled(false);
	mTestButton->setEnabled(false);

	if (mCalibRefTool->getTool())
	{
//		mCalibrationLabel->setText("Calibration:\n" + qstring_cast(mCalibratingTool->getTool()->getCalibration_sMt()));
//    Transform3D calibration = mCalibratingTool->getTool()->getCalibration_sMt();
//    mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
		mCalibrateButton->setEnabled(true);
		mTestButton->setEnabled(true);
	}

	//  mReferencePointLabel->setText(text);
}
//------------------------------------------------------------------------------

void LapFrameToolCalibrationWidget::trackingStartedSlot()
{
	ToolPtr ref = toolManager()->getTool("calibration_tool");
	if (ref)
		mCalibRefTool->setValue(ref->getUid());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




LapFrameToolCalibrationCalculator::LapFrameToolCalibrationCalculator(ToolPtr tool, ToolPtr calRef, double cameraAngle) :
    mTool(tool), mCalibrationRef(calRef), mCameraAngle(cameraAngle)
{
	m_sMpr = mTool->getCalibration_sMt() * mTool->get_prMt().inv();

//	m_qMcr = Transform3D::fromString(" 0.0,  0.0, -1.0, -71.5,"
//		                                  " 0.0, -1.0,  0.0,  -8.0,"
//	                                      "-1.0,  0.0,  0.0,  -8.8,"
//	                                      " 0.0,  0.0,  0.0,   1.0");

	m_qMcr = Transform3D::Identity();
	m_qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();
}

Vector3D LapFrameToolCalibrationCalculator::get_delta_ref()
{
	Vector3D p(0,0,0);
	Transform3D qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();

	Vector3D calibPoint_pr =  qMpr.inv().coord(p);
	Vector3D toolPoint_pr =  mTool->get_prMt().coord(p);
	return calibPoint_pr - toolPoint_pr;
}

Transform3D LapFrameToolCalibrationCalculator::get_calibration_sMt()
{
	return m_sMpr * m_qMpr.inv() * createTransformRotateY(mCameraAngle);
}


}
