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

#include "cxDonutMetricRep.h"

#include "sscView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "sscTypeConversions.h"
#include "vtkTextActor.h"
#include "sscGraphicalPrimitives.h"
#include "cxShapedMetric.h"
#include "sscGraphicalPrimitives.h"
#include "vtkMatrix4x4.h"
#include "cxGraphicalTorus3D.h"
#include "cxGraphicalDisk.h"

namespace cx
{

DonutMetricRepPtr DonutMetricRep::New(const QString& uid, const QString& name)
{
	DonutMetricRepPtr retval(new DonutMetricRep(uid, name));
	return retval;
}

DonutMetricRep::DonutMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name)
{
}

void DonutMetricRep::clear()
{
	DataMetricRep::clear();
	mTorus.reset();
	mDisk.reset();
}

DonutMetricPtr DonutMetricRep::getDonutMetric()
{
	return boost::dynamic_pointer_cast<DonutMetric>(mMetric);
}

void DonutMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

	this->updateTorus();
	this->updateDisc();

	this->drawText();
}

void DonutMetricRep::updateTorus()
{
	if (!mMetric)
		return;

	DonutMetricPtr donut = this->getDonutMetric();

	if (donut->getFlat())
	{
		mTorus.reset();
		return;
	}

	if (!mTorus && mView && mMetric)
		mTorus.reset(new GraphicalTorus3D(mView->getRenderer()));

	if (!mTorus)
		return;

	mTorus->setPosition(donut->getPosition());
	mTorus->setDirection(donut->getDirection());
	mTorus->setRadius(donut->getRadius());
	mTorus->setThickness(donut->getThickness());
	mTorus->setColor(donut->getColor());
}

void DonutMetricRep::updateDisc()
{
	if (!mMetric)
		return;

	DonutMetricPtr donut = this->getDonutMetric();

	if (!donut->getFlat())
	{
		mDisk.reset();
		return;
	}

	if (!mDisk && mView && mMetric)
	{
		mDisk.reset(new GraphicalDisk());
		mDisk->setRenderer(this->getRenderer());
	}

	if (!mDisk)
		return;

	mDisk->setPosition(donut->getPosition());
	mDisk->setDirection(donut->getDirection());
	mDisk->setRadius(donut->getRadius());
	mDisk->setHeight(donut->getHeight());
	mDisk->setColor(donut->getColor());
	mDisk->setOutlineColor(donut->getColor());
	mDisk->setOutlineWidth(donut->getThickness());
	mDisk->setFillVisible(false);
	mDisk->setLighting(true);

	mDisk->update();
}

}
