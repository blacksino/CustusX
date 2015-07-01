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


#ifndef CXGEOMETRICREP_H_
#define CXGEOMETRICREP_H_

#include "cxResourceVisualizationExport.h"

#include "vtkForwardDeclarations.h"
#include "cxRepImpl.h"
#include "cxVector3D.h"
#include <vtkGlyph3DMapper.h>


namespace cx
{
typedef boost::shared_ptr<class Mesh> MeshPtr;

typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;
typedef vtkSmartPointer<class vtkGlyph3DMapper> vtkGlyph3DMapperPtr;


class cxResourceVisualization_EXPORT GraphicalMeshBase
{
public:
    GraphicalMeshBase();
    ~GraphicalMeshBase();

    void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
    void setBackfaceCulling(bool val);
    void setFrontfaceCulling(bool val);
    void setRepresentation();
    void setColor(double red, double green, double blue);
    void setOpacity(double val);
    void setUserMatrix(vtkMatrix4x4 *matrix);

    vtkActorPtr getActor();
    vtkPolyDataPtr getPolyData();

protected:
    vtkPropertyPtr mProperty;
    vtkActorPtr mActor;
    vtkPolyDataPtr mData;
    vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<GraphicalMeshBase> GraphicalMeshBasePtr;




class cxResourceVisualization_EXPORT GraphicalPolyData : public GraphicalMeshBase
{
public:
    GraphicalPolyData();

    void setIsWireFrame(bool val);
    void setRepresentation();
    void setData(vtkPolyDataPtr data);
    void scalarVisibilityOff();

private:
    vtkPolyDataMapperPtr mMapper;
    bool mIsWireFrame;

};
typedef boost::shared_ptr<GraphicalPolyData> GraphicalPolyDataPtr;


class cxResourceVisualization_EXPORT GraphicalGlyph3DData : public GraphicalMeshBase
{
public:
    GraphicalGlyph3DData();

     void updateGlyph(vtkPolyDataPtr data, const char * orientationArray, const char * colorArray);

private:
    vtkGlyph3DMapperPtr mMapper;
};
typedef boost::shared_ptr<GraphicalGlyph3DData> GraphicalGlyph3DDataPtr;



/** \brief Display one Mesh in 3D.
 *
 * Use this to render geometric polydata (vtkPolyData) in a 3D scene.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT GeometricRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~GeometricRep();
	static GeometricRepPtr New(const QString& uid="");

	virtual QString getType() const { return "GeometricRep"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh

protected:
	GeometricRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

    GraphicalPolyDataPtr mGraphicalPolyDataPtr;
    GraphicalGlyph3DDataPtr mGraphicalGlyph3DDataPtr;

	MeshPtr mMesh;

private slots:
	void meshChangedSlot();
	void transformChangedSlot();
};






} // namespace cx

#endif /*CXGEOMETRICREP_H_*/
