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

#include "cxOpenIGTLinkTrackingSystemService.h"

#include "ctkPluginContext.h"
#include "cxLogger.h"
#include "cxOpenIGTLinkClient.h"
#include "cxOpenIGTLinkSessionManager.h"

namespace cx
{
OpenIGTLinkTrackingSystemService::OpenIGTLinkTrackingSystemService()
{
    // CLIENT START
    /*
    OpenIGTLinkClient *client = new OpenIGTLinkClient;
    client->moveToThread(&mOpenIGTLinkThread);
    connect(&mOpenIGTLinkThread, &QThread::finished, client, &QObject::deleteLater);
    connect(this, &OpenIGTLinkTrackingSystemService::connectToServer, client, &OpenIGTLinkClient::establishConnectionToServer);
    connect(this, &OpenIGTLinkTrackingSystemService::listenToServer, client, &OpenIGTLinkClient::listen);
    connect(client, &OpenIGTLinkClient::packageArrived, this, &OpenIGTLinkTrackingSystemService::getPackage);
    */
    //CLIENT END

    //SESSIONMANAGER START

    OpenIGTLinkSessionManager *session = new OpenIGTLinkSessionManager;

    session->moveToThread(&mOpenIGTLinkThread);
    connect(&mOpenIGTLinkThread, &QThread::finished, session, &QObject::deleteLater);
    connect(this, &OpenIGTLinkTrackingSystemService::listenToServer, session, &OpenIGTLinkSessionManager::connectAndListen);

    //SESSIONMANAGER END

    mOpenIGTLinkThread.start();

    emit connectToServer();
    emit listenToServer();
}

OpenIGTLinkTrackingSystemService::~OpenIGTLinkTrackingSystemService()
{
    mOpenIGTLinkThread.quit();
    mOpenIGTLinkThread.wait();
}

QString OpenIGTLinkTrackingSystemService::getUid() const
{
    return "org.custusx.core.tracking.system.openigtlink";
}

Tool::State OpenIGTLinkTrackingSystemService::getState() const
{
    return Tool::tsNONE;
}

void OpenIGTLinkTrackingSystemService::setState(const Tool::State val)
{}

std::vector<ToolPtr> OpenIGTLinkTrackingSystemService::getTools()
{
    std::vector<ToolPtr> retval;
    return retval;
}

TrackerConfigurationPtr OpenIGTLinkTrackingSystemService::getConfiguration()
{
    TrackerConfigurationPtr retval;
    return retval;
}

ToolPtr OpenIGTLinkTrackingSystemService::getReference()
{
    ToolPtr retval;
    return retval;
}

void OpenIGTLinkTrackingSystemService::setLoggingFolder(QString loggingFolder)
{}

void OpenIGTLinkTrackingSystemService::getPackage()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Package arrived in CustusX.";
}

} /* namespace cx */
