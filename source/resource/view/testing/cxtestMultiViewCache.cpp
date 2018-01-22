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


#include "catch.hpp"
#include "cxMultiViewCache.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"
#include <vtkRenderWindow.h>

namespace cxtest {

TEST_CASE("MultiViewCache init", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::MultiViewCachePtr viewCache = cx::MultiViewCache::create(factory);
	REQUIRE(viewCache);
}

TEST_CASE("MultiViewCache retrieveView 3D", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::MultiViewCachePtr viewCache = cx::MultiViewCache::create(factory);
	REQUIRE(viewCache);

	viewCache->clearCache();

	boost::shared_ptr<cx::ViewCollectionWidget> mainWidget;
	mainWidget.reset(cx::ViewCollectionWidget::createViewWidgetLayout(factory).data());

	cx::ViewWidget* view = viewCache->retrieveView(mainWidget.get(), cx::View::VIEW_3D, false);
	REQUIRE(view);
}

TEST_CASE("MultiViewCache retrieveView 2D", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::MultiViewCachePtr viewCache = cx::MultiViewCache::create(factory);
	REQUIRE(viewCache);

	viewCache->clearCache();

	boost::shared_ptr<cx::ViewCollectionWidget> mainWidget;
	mainWidget.reset(cx::ViewCollectionWidget::createViewWidgetLayout(factory).data());

	cx::ViewWidget* view = viewCache->retrieveView(mainWidget.get(), cx::View::VIEW_2D, false);
	REQUIRE(view);
}

TEST_CASE("MultiViewCache multiple 2D views", "[opengl][resource][visualization][experimental]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());

	boost::shared_ptr<cx::ViewCollectionWidget> mainWidget;
	mainWidget.reset(cx::ViewCollectionWidget::createViewWidgetLayout(factory).data());

	for(int i = 0; i < 5000; ++i)
	{
		mainWidget->setOffScreenRenderingAndClear(false);
		for (int x = 0; x < 10; ++x)
		{
			for(int y = 0; y < 10; ++y)
			{
				cx::LayoutRegion region = cx::LayoutRegion(x,y);
				cx::ViewPtr view = mainWidget->addView(cx::View::VIEW_2D, region);
				REQUIRE(view);
			}
		}
		vtkRenderWindowPtr renderWindow = factory->getRenderWindow("test", false);
		renderWindow->Render();
	}
}

} //cxtest
