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

#include "cxMeshHelpers.h"

#include <QColor>

#include <vtkPoints.h>
#include <vtkCellArray.h>

#include "cxPatientModelService.h"
#include "cxMesh.h"
#include "cxTypeConversions.h"
#include "cxTime.h"

namespace cx
{
vtkPolyDataPtr polydataFromTransforms(TimedTransformMap transformMap_prMt, Transform3D rMpr)
{
  vtkPolyDataPtr retval = vtkPolyDataPtr::New();

  vtkPointsPtr points = vtkPointsPtr::New();
  vtkCellArrayPtr lines = vtkCellArrayPtr::New();

  points->Allocate(transformMap_prMt.size());

  TimedTransformMap::iterator mapIter = transformMap_prMt.begin();
  while(mapIter != transformMap_prMt.end())
  {
    Vector3D point_t = Vector3D(0,0,0);

    Transform3D prMt = mapIter->second;
    Transform3D rMt = rMpr * prMt;
    Vector3D p = rMt.coord(point_t);
    points->InsertNextPoint(p.begin());

		++mapIter;
  }

  lines->Initialize();
  std::vector<vtkIdType> ids(points->GetNumberOfPoints());
  for (unsigned i=0; i<ids.size(); ++i)
    ids[i] = i;
  lines->InsertNextCell(ids.size(), &(*ids.begin()));

  retval->SetPoints(points);
  retval->SetLines(lines);
  return retval;
}

void loadMeshFromToolTransforms(PatientModelServicePtr dataManager, TimedTransformMap transforms_prMt)
{
  //create polydata from positions
	Transform3D rMpr = dataManager->get_rMpr();
  vtkPolyDataPtr centerlinePolydata = polydataFromTransforms(transforms_prMt, rMpr);
  QString uid = "tool_positions_mesh_%1";
  QString name = "Tool positions mesh %1";
  MeshPtr mesh = dataManager->createSpecificData<Mesh>(uid, name);
  mesh->setVtkPolyData(centerlinePolydata);
  mesh->setColor(QColor("red"));
  dataManager->insertData(mesh);
}

std::map<std::string, std::string> getDisplayFriendlyInfo(MeshPtr mesh)
{
	std::map<std::string, std::string> retval;
	if(!mesh)
		return retval;

	//mesh
	retval["Filename"] = mesh->getFilename().toStdString();
	retval["Coordinate system"] = mesh->getCoordinateSystem().toString().toStdString();
	retval["Name"] = mesh->getName().toStdString();
	retval["Parent space"] = mesh->getParentSpace().toStdString();
	retval["Space"] = mesh->getSpace().toStdString();
	retval["Type"] = mesh->getType().toStdString();
	retval["Uid"] = mesh->getUid().toStdString();
	retval["rMd"] = matrixAsSingleLineString(mesh->get_rMd());
	retval["Backface culling"] = string_cast(mesh->getBackfaceCulling());
	retval["Color"] = mesh->getColor().name().toStdString();
	retval["Frontface culling"] = string_cast(mesh->getFrontfaceCulling());
//	retval["Is wireframe"] = string_cast(mesh->getIsWireframe());
	retval["Acquisition time"] = string_cast(mesh->getAcquisitionTime().toString(timestampSecondsFormatNice()));
	retval["Fiber bundle"] = string_cast(mesh->isFiberBundle());

	//vtkPolyData
	float actualMemorySizeKB = (float)mesh->getVtkPolyData()->GetActualMemorySize();
	retval["Actual memory size"] = string_cast(actualMemorySizeKB/(1024*1024))+" GB, "+string_cast(actualMemorySizeKB/1024)+" MB, "+string_cast(actualMemorySizeKB)+" kB";
	retval["Points"] = string_cast(mesh->getVtkPolyData()->GetNumberOfPoints());
	retval["Lines"] = string_cast(mesh->getVtkPolyData()->GetNumberOfLines());
	retval["Pieces"] = string_cast(mesh->getVtkPolyData()->GetNumberOfPieces());
	retval["Polys"] = string_cast(mesh->getVtkPolyData()->GetNumberOfPolys());
	retval["Strips"] = string_cast(mesh->getVtkPolyData()->GetNumberOfStrips());
	retval["Verts"] = string_cast(mesh->getVtkPolyData()->GetNumberOfVerts());

	return retval;
}

}//namespace cx

