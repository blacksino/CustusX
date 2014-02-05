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

#include "cxSecondaryViewLayoutWindow.h"

#include "cxViewManager.h"
#include "sscLogger.h"

namespace cx
{

SecondaryViewLayoutWindow::SecondaryViewLayoutWindow(QWidget* parent) :
	QMainWindow(parent)
{
	this->setWindowTitle("View Layout");
}

SecondaryViewLayoutWindow::~SecondaryViewLayoutWindow()
{
}

void SecondaryViewLayoutWindow::showEvent(QShowEvent* event)
{
	viewManager()->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
	this->setCentralWidget(viewManager()->getLayoutWidget(1));
}

void SecondaryViewLayoutWindow::hideEvent(QCloseEvent* event)
{
	viewManager()->setActiveLayout("", 1);
}

void SecondaryViewLayoutWindow::closeEvent(QCloseEvent *event)
{
	viewManager()->setActiveLayout("", 1);
}

} // namespace cx

