#ifndef SSCTOOL3DREP_H_
#define SSCTOOL3DREP_H_

#include "sscRepImpl.h"
#include "sscTransform3D.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVtkHelperClasses.h"
#include "sscViewportListener.h"

namespace ssc
{

typedef boost::shared_ptr<class VideoGraphics> VideoGraphicsPtr;
typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;

/**Display a 3D representation of a ssc::Tool.
 * The representation to use is extracted from the tool itself.		
 *
 * Used by Sonowand.
 * Used by CustusX.
 */
class ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	static ToolRep3DPtr New(const QString& uid, const QString& name="");
	virtual ~ToolRep3D();
	virtual QString getType() const;

	virtual void setTool(ToolPtr tool);
	virtual bool hasTool(ToolPtr tool) const;
	virtual ToolPtr getTool();
	ToolTracerPtr getTracer();
	void setSphereRadius(double radius);

	void setTooltipPointColor(Vector3D c) { mTooltipPointColor = c; }
	void setOffsetPointColor(Vector3D c) { mOffsetPointColor = c; }
	void setOffsetLineColor(Vector3D c) { mOffsetLineColor = c; }
	void setOffsetStipplePattern(int pattern) { mStipplePattern = pattern; }

	void setStayHiddenAfterVisible(bool val);
	void setStayVisibleAfterHide(bool val); ///< if true, tool is still rendered as visible after visibility status is hidden.
	void setOffsetPointVisibleAtZeroOffset(bool val); ///< if true the sphere is visible even when the offset is zero
  void setSphereRadiusInNormalizedViewport(bool on);

protected:
	ToolRep3D(const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
	bool showProbe();

private slots:
	/**
	 * Receive transform and timestamp from a tool
	 *\param matrix		The tools transform
	 *\param timestamp	The time at which the transform was recorded
	 */
	void receiveTransforms(Transform3D matrix, double timestamp);
	void probeSectorChanged();
	/**
	 * Receive whether or not the tool is visible to the tracking system
	 *\param visible Whether or not the tool is visible to the tracking system.
	 */
	void receiveVisible(bool visible);
	void tooltipOffsetSlot(double val); ///< receive the virtual tool tip extension.

private:
	void update();
	void updateOffsetGraphics();
	void scaleSpheres();

  double mSphereRadius;
	ToolPtr mTool;
	vtkActorPtr mToolActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkSTLReaderPtr mSTLReader;
	GraphicalPoint3DPtr mTooltipPoint;
	GraphicalPoint3DPtr mOffsetPoint;
	GraphicalLine3DPtr mOffsetLine;
	bool mStayHiddenAfterVisible;
	bool mStayVisibleAfterHide;
	bool mOffsetPointVisibleAtZeroOffset;
	bool mSphereRadiusInNormalizedViewport;
	ToolTracerPtr mTracer;
  ssc::ViewportListenerPtr mViewportListener;

	//US Probe sector
	ProbeSectorPtr mProbeSector;
	vtkPolyDataMapperPtr mProbeSectorPolyDataMapper;
	vtkActorPtr mProbeSectorActor;
	VideoGraphicsPtr mRTStream;
	Vector3D mTooltipPointColor;
	Vector3D mOffsetPointColor;
	Vector3D mOffsetLineColor;
	int mStipplePattern;
};
} // namespace ssc

#endif /*SSCTOOL3DREP_H_*/
