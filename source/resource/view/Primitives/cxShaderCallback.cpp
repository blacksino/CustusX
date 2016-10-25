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

#include "cxShaderCallback.h"

#include "cxGPUImageBuffer.h"
#include "cxTypeConversions.h"
#include "cxGLHelpers.h"
#include "cxLogger.h"

#include <vtkShaderProgram.h>
#include <vtkOpenGLHelper.h>


#ifdef __APPLE__
#include <OpenGL/glu.h>
//#include <OpenGL/gl.h>
#include "X11/Xlib.h"
//#include "/usr/include/X11/Xlib.h"
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif


namespace cx
{


ShaderCallback::ShaderCallback(int index)
{
	this->init(index);
}

ShaderCallback::ShaderCallback()
{
	this->init(-1);
}

void ShaderCallback::init(int index)
{
	mWindow = (0.0);
	mLevel = (0.0);
	mLLR = (0.0);
	mAlpha = (1.0);

	mIndex = index;
//	this->Renderer = 0;
}

ShaderCallback::~ShaderCallback()
{

}

void ShaderCallback::SetBuffer(GPUImageDataBufferPtr buffer)
{
	mVolumeBuffer = buffer;
}

void ShaderCallback::SetBuffer(GPUImageLutBufferPtr buffer)
{
	mLutBuffer = buffer;
}

void ShaderCallback::SetColorAttribute(float window, float level, float llr,float alpha)
{
	mWindow = window;
	mLevel = level;
	mLLR = llr;
	mAlpha = alpha;
}

void ShaderCallback::initializeRendering()
{
	if (mVolumeBuffer)
		mVolumeBuffer->allocate(mIndex);
	if (mLutBuffer)
		mLutBuffer->allocate();
}

void ShaderCallback::setUniformiArray(vtkOpenGLHelper *cellBO, QString name, int val)
{
	QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
//	uniforms->SetUniformi(cstring_cast(fullName), 1, &val);
	cellBO->Program->SetUniform1iv(cstring_cast(fullName), 1, &val);
}

void ShaderCallback::setUniformfArray(vtkOpenGLHelper *cellBO, QString name, float val)
{
	QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
//	uniforms->SetUniformf(cstring_cast(fullName), 1, &val);
	cellBO->Program->SetUniform1fv(cstring_cast(fullName), 1, &val);
}

//void SingleVolumePainterHelper::eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader)
void ShaderCallback::eachRenderInternal(vtkOpenGLHelper *cellBO)
{
	if (!mVolumeBuffer)
		return;

	mVolumeBuffer->bind(mIndex);

	int texture = 2*mIndex; //texture unit 1
	int lut = 2*mIndex+1; //texture unit 1

	int lutsize = 0;
	if (mLutBuffer)
	{
//		mLutBuffer->bind(mIndex);
		lutsize = mLutBuffer->getLutSize();
	}

	this->setUniformiArray(cellBO, "test", 1);

	this->setUniformiArray(cellBO, "cx_texture", texture);
	this->setUniformiArray(cellBO, "cxTextureSamplers", texture);
//	this->setUniformiArray(cellBO, "lut", lut);
//	this->setUniformiArray(cellBO, "lutsize", lutsize);
//	this->setUniformfArray(cellBO, "llr", mLLR);
//	this->setUniformfArray(cellBO, "level", mLevel);
//	this->setUniformfArray(cellBO, "window", mWindow);
//	this->setUniformfArray(cellBO, "alpha", mAlpha);


//	report_gl_error();
//	float values[9] = {0, 0, 0,
//					   0, 0, 1,
//					   0, 1, 0};
//	cellBO->Program->SetUniformMatrix3x3("cx_tcoordMat", values);

//	cellBO->Program->SetAttributeArray("cx_tcoordMC", values, 3, vtkShaderProgram::Normalize);

	report_gl_error();
}

//void ShaderCallback::uploadTextureCoordinate(vtkOpenGLHelper *cellBO)
//{
//	return;//test
//	if(!cellBO)
//		return;
//	if (!mVolumeBuffer)
//		return;

//	mVolumeBuffer->bind(mIndex);

////	float val[3] = {0, 0, 0};//TODO: set texture coordinate. Look in Texture3DSlicerProxyImpl::updateCoordinates

////	cellBO->Program->SetUniform3fv("cx_tcoordMC", 1, &val);

////	cellBO->Program->SetUniform1fv("cx_tcoordMC[]", 1, &val);
////	float values[9] = {0, 0, 0,
////					   0, 0, 1,
////					   0, 1, 0};
////	cellBO->Program->SetUniformMatrix3x3("cx_tcoordMC", values);
//	report_gl_error();

//	//Can't get this to work
////	//Upload texture coordinate index
////	//See http://stackoverflow.com/questions/10630823/how-to-get-texture-coordinate-to-glsl-in-version-150
////	GLint texcoord_index = glGetAttribLocation(cellBO->Program->GetHandle(), "cx_tcoordMC");
////	report_gl_error();
////	CX_LOG_DEBUG() << "cellBO->Program->GetHandle(): " << cellBO->Program->GetHandle();
////	CX_LOG_DEBUG() << "texcoord_index: " << texcoord_index;

//////	glBegin(GL_QUADS);
////	report_gl_error();
////	glVertexAttrib3f(texcoord_index, 0, 0, 0);
////	report_gl_error();
////	glVertexAttrib3f(texcoord_index, 1, 0, 0);
////	report_gl_error();
////	glVertexAttrib3f(texcoord_index, 0, 1, 0);
////	glVertexAttrib3f(texcoord_index, 0, 0, 1);
////	glVertexAttrib3f(texcoord_index, 0, 1, 1);
////	glVertexAttrib3f(texcoord_index, 1, 1, 0);
////	glVertexAttrib3f(texcoord_index, 1, 0, 1);
////	glVertexAttrib3f(texcoord_index, 1, 1, 1);
////	report_gl_error();
//////	glEnd();


//	//Alternative
//	//	glEnableVertexAttribArray(texcoord_index);
////	glVertexAttribPointer(texcoord_index, attribs.Size, attribs.Type,
////						  attribs.Normalize, attribs.Stride,
////						  BUFFER_OFFSET(attribs.Offset));

//	// An array of 3 vectors which represents 3 vertices
//	static const GLfloat g_vertex_buffer_data[] = {
////	   -1.0f, -1.0f, 0.0f,
////	   1.0f, -1.0f, 0.0f,
////	   0.0f,  1.0f, 0.0f,
////		18.0906, 11.2849, 0.05,
////		-17.9906, 11.2849, 0.05,
////		18.0906, -11.1849, 0.05,
////		-17.9906, -11.1849, 0.05,
//		0.5f, 0.5f, 0.5f,
//		1.0f, -1.0f, 0.0f,
//		0.0f,  1.0f, 0.0f,
//		1.0f,  1.0f, 1.0f,
//	};

//	// This will identify our vertex buffer
//	GLuint vertexbuffer;
//	// Generate 1 buffer, put the resulting identifier in vertexbuffer
//	glGenBuffers(1, &vertexbuffer);
//	// The following commands will talk about our 'vertexbuffer' buffer
//	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//	// Give our vertices to OpenGL.
//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

//	GLint texcoord_index = glGetAttribLocation(cellBO->Program->GetHandle(), "cx_tcoordMC");
//	CX_LOG_DEBUG() << "texcoord_index: " << texcoord_index;
//	glEnableVertexAttribArray(texcoord_index);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//	glVertexAttribPointer(
//	   texcoord_index,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//	   3,                  // size
//	   GL_FLOAT,           // type
//	   GL_FALSE,           // normalized?
//	   0,                  // stride
//	   (void*)0            // array buffer offset
//	);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//	glDisableVertexAttribArray(texcoord_index);
//	report_gl_error();
//}

void ShaderCallback::Execute(vtkObject *, unsigned long eventId, void *cbo)
{
	vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);

//    float diffuseColor[3];


//    diffuseColor[0] = 0.4;
//    diffuseColor[1] = 0.7;
//    diffuseColor[2] = 0.6;
//    cellBO->Program->SetUniform3f("diffuseColorUniform", diffuseColor);

	if(eventId == vtkCommand::UpdateShaderEvent)
	{
		if(mVolumeBuffer)
		{
			mVolumeBuffer->allocate(mIndex);
		}
		this->eachRenderInternal(cellBO);
//		this->uploadTextureCoordinate(cellBO);
	}
	else if(eventId == vtkCommand::EndEvent) //pre render
	{
//		GLint oldTextureUnit;
//		glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);
//		CX_LOG_DEBUG() << "oldTextureUnit: " << oldTextureUnit;

		if(mVolumeBuffer)
		{
			mVolumeBuffer->allocate(mIndex);
//			this->uploadTextureCoordinate(cellBO);
		}
//		if (mLutBuffer)
//		{
//			mLutBuffer->allocate();
//		}

//		glActiveTexture(oldTextureUnit);
	}
}

}//cx
