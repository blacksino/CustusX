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


//#include "boost/shared_ptr.hpp"
#include <vtkRenderer.h>

#include "cxStreamRep3D.h"
#include "cxTrackedStream.h"
#include "cxVideoGraphics.h"
#include "cxView.h"
#include "cxTool.h"


namespace cx
{

StreamRep3DPtr StreamRep3D::New(QString uid)
{
	return wrap_new(new StreamRep3D(), uid);
}

StreamRep3D::StreamRep3D() :
	RepImpl(), mTrackedStream(TrackedStreamPtr())
{
	mPipeline.reset(new VideoGraphics());
}

void StreamRep3D::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mPipeline->getActor());
}

void StreamRep3D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mPipeline->getActor());
}

void StreamRep3D::setTrackedStream(TrackedStreamPtr trackedStream)
{
	mTrackedStream = trackedStream;

	ToolPtr tool = mTrackedStream->getProbeTool();

	connect(tool.get(), &Tool::toolTransformAndTimestamp, this, &StreamRep3D::newTransform);
}

TrackedStreamPtr StreamRep3D::getTrackedStream()
{
	return mTrackedStream;
}

QString StreamRep3D::getType() const
{
	return "StreamRep3D";
}

void StreamRep3D::newTransform(Transform3D prMt, double timestamp)
{
	//TODO: Calculate correct transform. See VideoSourceGraphics::receiveTransforms
	mPipeline->setActorUserMatrix(prMt.getVtkMatrix());
}

} //cx