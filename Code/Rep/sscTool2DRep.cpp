//
// C++ Implementation: vmTool2drep
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#include "sscTool2DRep.h"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>

#include "sscSliceProxy.h"
#include "sscToolManager.h"
#include "sscView.h"
#include "sscVtkHelperClasses.h"

namespace ssc
{

ToolRep2D::ToolRep2D(const std::string& uid, const std::string& name) :
	RepImpl(uid, name),
	mBB_vp(0,1,0,1,0,1)
{
	mUseOffset = true;
	mUseCrosshair = false;
	mUseToolLine = true;
	mUseOffsetText = false;
}

ToolRep2D::~ToolRep2D()
{
}

ToolRep2DPtr ToolRep2D::New(const std::string& uid, const std::string& name)
{
	ToolRep2DPtr retval(new ToolRep2D(uid, name));
	retval->mSelf = retval;
	return retval;
}

std::string ToolRep2D::getType() const
{
	return "vm::ToolRep2D";
}

double ToolRep2D::getOffset()
{
	if (mSlicer->getTool() && showOffset())
		return mSlicer->getTool()->getTooltipOffset();
	return 0.0;
}

/**Set transform from slice space to viewport, and the viewport size.
 * Some old code need this in order to render in viewport coordinates.
 */
void ToolRep2D::setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp)
//void ToolRep2D::set_vpMs(const Transform3D& vpMs)
{
	m_vpMs = vpMs;	
	mBB_vp = vp;

	crossHairResized();
	update();
}

/**Set a slice proxy representing the plane on which to render.
 */
void ToolRep2D::setSliceProxy(ssc::SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
		disconnect(mSlicer.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		disconnect(mSlicer.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));		
	}
	
	mSlicer = slicer;
	
	connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	connect(mSlicer.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
	connect(mSlicer.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
}

/**Set display of the line from tool tip to the cross pos (this is the offset line).
 */
void ToolRep2D::setUseOffset(bool on)
{
	mUseOffset = on;
	setVisibility();
}

/**Set display of a yellow crosshair centered on the cross pos.
 */
void ToolRep2D::setUseCrosshair(bool on)
{
	mUseCrosshair = on;
	setVisibility();
}

/**Set display of the line from back infinity to the tool tip.
 */
void ToolRep2D::setUseToolLine(bool on)
{
	mUseToolLine = on;
	setVisibility();
}

/**Set display of the amount of offset, in the upper right corner.
 */
void ToolRep2D::setUseOffsetText(bool on)
{
	mUseOffsetText = on;
	setVisibility();
}

void ToolRep2D::createUSProbe(vtkRendererPtr renderer)
{
	mUSProbe2D.reset( new USProbe2D(renderer) );
}

void ToolRep2D::addRepActorsToViewRenderer(ssc::View* view)
{
	createToolLine(view->getRenderer(), Vector3D(0,0,0));
	createCrossHair(view->getRenderer() );
	createOffsetText(view->getRenderer(), Vector3D(0,0,0));
	createUSProbe(view->getRenderer());
	setVisibility();
	update();
}

void ToolRep2D::removeRepActorsFromViewRenderer(ssc::View* view)
{
	cursor.reset();
	center2Tool.reset();
	tool2Back.reset();
	centerPoint.reset();
	toolPoint.reset();
	distanceText.reset();
	mUSProbe2D.reset();
}

void ToolRep2D::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}
void ToolRep2D::toolTransformAndTimestampSlot(Transform3D prMt, double timestamp)
{
	update();	
}
void ToolRep2D::toolVisibleSlot(bool visible)
{
	setVisibility();
	update();
}

void ToolRep2D::update()
{
	if (!mSlicer->getTool())
		return;
	//setVisibility();
	//Logger::log("tool.log", "<"+string_cast(__FUNCTION__)+">"+" -- again!!!");
	//Logger::log("vm.log", "ToolRep2D::update()");

	if (showProbe())
	{
		setProbeSector(mSlicer->getTool()->getProbeSector());
	}
	
	Transform3D prMt;
	if (mSlicer->getTool())
	{
		prMt = mSlicer->getTool()->get_prMt();
	}
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D sMr = mSlicer->get_sMr();
	Transform3D vpMt = m_vpMs*sMr*rMpr*prMt;
	
	Vector3D cross = vpMt.coord(getOffset() * Vector3D(0,0,1)); // zero position plus offset along z
	Vector3D tooltip = vpMt.coord(Vector3D(0,0,0)); // the zero position
	Vector3D toolback = vpMt.coord(Vector3D(0,0,-1000)); // a point 1m backwards in z

	if (cursor)
		cursor->update(cross, mBB_vp); ///crosshair, shows in Navigation
	updateOffsetText();	
	updateToolLine(cross, tooltip, toolback);
	updateUSProbe(sMr*rMpr*prMt);
}

void ToolRep2D::setProbeSector(ssc::ProbeSector data)
{
	if (mUSProbe2D)
	{
		mUSProbe2D->setShape(data.mType, data.mDepthStart, data.mDepthEnd, data.mWidth); 
	}	
}

void ToolRep2D::updateUSProbe(const Transform3D& vpMt)
{
	if (mUSProbe2D)
	{
		mUSProbe2D->setPosition(vpMt);
	}
}

bool ToolRep2D::showProbe()
{
	return mSlicer->getTool() && mSlicer->getTool()->getType()==ssc::Tool::TOOL_US_PROBE;
}
bool ToolRep2D::showOffset()
{
	return mSlicer->getTool() && mUseOffset && !showProbe();
}

void ToolRep2D::setVisibility()
{
//	Logger::log("vm.log", "ToolRep2D::setVisibility(), offset=" + string_cast(showOffset()));
	bool hasTool = mSlicer->getTool();
	
	if (cursor)
		cursor->getActor()->SetVisibility(mUseCrosshair && hasTool);
	if (center2Tool)
		center2Tool->getActor()->SetVisibility(showOffset());
	if (tool2Back)
		tool2Back->getActor()->SetVisibility(mUseToolLine && hasTool);
	if (centerPoint)
		centerPoint->getActor()->SetVisibility(mUseToolLine && hasTool);
	if (toolPoint)
		toolPoint->getActor()->SetVisibility(showOffset());
	if (distanceText)
		distanceText->getActor()->SetVisibility(mUseOffsetText && showOffset());	
	if (mUSProbe2D)
		mUSProbe2D->setVisibility(showProbe());
}

/**create a yellow crosshair centered at the tool offset pos. 
 * 
 */
void ToolRep2D::createCrossHair(vtkRendererPtr renderer)
{
	//Logger::log("vm.log", "<"+string_cast(__FUNCTION__)+">"+" new " );
	cursor.reset( new CrossHair2D(renderer) ) ;
	crossHairResized();
}
/**create a yellow crosshair centered at the tool offset pos. 
 * 
 */
void ToolRep2D::crossHairResized()
{
	if (cursor)
	{
		double bordarOffset = 30.0;
		RGBColor color(1.0, 0.8, 0.0);
		Vector3D focalPoint(0.0,0.0,0.0);
		//Logger::log("tool.log", "("+string_cast(__FUNCTION__)+")"+" mCross"+string_cast(cross));
		//cursor->setValue( focalPoint, mBB_vp.range()[0], mBB_vp.range()[1], bordarOffset, color  );
		cursor->setValue( focalPoint, int(mBB_vp.range()[0]), int(mBB_vp.range()[1]), bordarOffset, color  );
	}
}

/**create the line projection of the tool + offset onto the plane.
 */
void ToolRep2D::createToolLine(vtkRendererPtr renderer, const Vector3D& centerPos )
{
	// line from tooltip to offset point
	RGBColor offsetColor(1.0,0.8,0.0);
	center2Tool.reset( new LineSegment(renderer) );
	center2Tool->setPoints( centerPos, Vector3D(0.0, 0.0, 0.0), offsetColor ) ;

	// line from back infinity to tooltip
	RGBColor toolColor(0.25,0.87,0.16);
	tool2Back.reset( new LineSegment(renderer) );
	tool2Back->setPoints( Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), toolColor );

	//adding dots at toolpoint
	Vector3D dotColor(0.96,0.87,0.17);
	centerPoint.reset( new OffsetPoint(renderer) );
	centerPoint->setValue( centerPos,  dotColor );
	centerPoint->setRadius ( 4 );

	// ? 
	Vector3D dotColor2(0.0,0.0,1.0);
	toolPoint.reset( new OffsetPoint(renderer) );
	toolPoint->setValue( Vector3D(0.0,0.0,0.0), dotColor );
	toolPoint->setRadius ( 3 );
	toolPoint->getActor()->VisibilityOff();
}

/**create the offset text
 */
void ToolRep2D::createOffsetText(vtkRendererPtr renderer, const Vector3D& pos )
{
	std::string text;
	Vector3D color(0.7372, 0.815, 0.6039);
	distanceText.reset( new TextDisplay( "---", color, 18) );
	distanceText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	distanceText->textProperty()->SetJustificationToRight();
	distanceText->setPosition(Vector3D(0.95, 0.95, 0.0));
	renderer->AddActor( distanceText->getActor() );
	updateOffsetText();
}

/**
 * must update crosshair, offset, points and distance
 *  when in registration no update to tool
 */
void ToolRep2D::updateToolLine(const Vector3D& crossPos, const Vector3D& toolTipPos, const Vector3D toolTipBackPos)
{
	if (!center2Tool || !tool2Back || !centerPoint || !toolPoint)
		return;
	
	center2Tool->updatePosition( crossPos, toolTipPos);
	tool2Back->updatePosition( toolTipPos, toolTipBackPos );

	//points
	centerPoint->update( crossPos );

	if( getOffset() > 2.0 )
	{
		toolPoint->getActor()->VisibilityOn();
		toolPoint->update( toolTipPos );
		toolPoint->setRadius ( 4 );
	}
	else
	{
		toolPoint->getActor()->VisibilityOff(); // doesn't work
		toolPoint->update( toolTipPos ); // does work...
		toolPoint->setRadius ( 3 );
	}
}

void ToolRep2D::updateOffsetText()
{
	if (!distanceText)
	{
		return;
	}

	if( getOffset() > 2.0 && mUseOffsetText && showOffset())
	{
		char buffer[100];
		//snprintf( buffer, sizeof(buffer), "Offset: %3.0f mm ", getOffset() );
		printf( buffer, sizeof(buffer), "Offset: %3.0f mm ", getOffset() );
		std::string text = buffer;
		distanceText->updateText( text );
		distanceText->getActor()->VisibilityOn();
	}
	else
	{
		distanceText->getActor()->VisibilityOff(); // doesnt work...
		distanceText->updateText( "" ); // does work...
	}

}

} // namespace vm
