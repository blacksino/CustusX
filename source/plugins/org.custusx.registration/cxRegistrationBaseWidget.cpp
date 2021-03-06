/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxRegistrationBaseWidget.h"

namespace cx
{

RegistrationBaseWidget::RegistrationBaseWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle) :
  BaseWidget(parent, objectName, windowTitle),
	mServices(services)
{
}

} //cx
