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

#include "catch.hpp"

#include <QTimer>

#include "cxtestSender.h"
#include "cxtestSignalListener.h"
#include "cxtestUtilities.h"

#include "cxTestCustusXController.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "sscTypeConversions.h"

namespace cxtest
{

//QString addTag(QString name, QString text)
//{
//	QString escapedXmlTag("<%1>");
////	QString escapedXmlTag("\\&lt;%1\\&gt;");
//	return escapedXmlTag.arg(name) + text + escapedXmlTag.arg("/"+name);
//}

QString defineJenkinsMeasurememt(QString name, QString value)
{
	QString format("<measurement><name>%1</name><value>%2</value></measurement>");
	return format.arg(name).arg(value);
//	QString nameElement = addTag("name", name);
//	QString valueElement = addTag("value", value);
//	QString measurementElement = addTag("measurement", nameElement+valueElement);
//	return measurementElement;
//	QString format("&lt;measurement&gt; &lt;name&gt; %1 &lt;/name&gt; &lt;value&gt; %2 &lt;/value&gt; &lt;/measurement&gt;");
//	QString format("&lt;measurement&gt;
//	               "&lt;name&gt;
//	               "%1 &lt;/name&gt;
//	               "&lt;value&gt;
//	               "%2
//	               "&lt;/value&gt;
//	               "&lt;/measurement&gt;
//	               ");
}


TEST_CASE("Speed: Run CustusX with a minimum render speed", "[speed][gui][integration]")
{
	cout << "CTEST_FULL_OUTPUT" << endl;
	// this stuff will be performed just before all tests in this class
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("renderingInterval", 4);


	CustusXController custusX(NULL);

	custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
	custusX.start();
	//custusX.mPatientFolder = "/Users/christiana/Patients/Speed_Test_Lap_Large.cx3.cx3";

//	QTimer::singleShot(      0,   &custusX, SLOT(initialBeginCheckRenderSlot()) );
//	QTimer::singleShot(      0,   &custusX, SLOT(loadPatientSlot()) );
//	QTimer::singleShot( 5*1000,   &custusX, SLOT(initialEndCheckRenderSlot()) );
  //  QTimer::singleShot( 5*1000+1, &custusX, SLOT(initialBeginCheckRenderSlot()) );
  //  QTimer::singleShot(20*1000,   &custusX, SLOT(secondEndCheckRenderSlot()) );
  //  QTimer::singleShot(21*1000,   &custusX, SLOT(displayResultsSlot()) );


  //  QTimer::singleShot(25*1000,   qApp, SLOT(quit()) );

	qApp->exec();
	custusX.stop();

	// output FPS in a way friendly to the Jenkins measurement plugin:
//	QString escapedXmlTag("&lt;%1&gt;");
//	QString format("&lt;measurement&gt; &lt;name&gt; %1 &lt;/name&gt; &lt;value&gt; %2 &lt;/value&gt; &lt;/measurement&gt;");
//	QString format("&lt;measurement&gt;&lt;name&gt;%1&lt;/name&gt;&lt;value&gt;%2&lt;/value&gt;&lt;/measurement&gt;");
//	QString format("&amp;lt;measurement&amp;gt;&amp;lt;name&amp;gt;%1&amp;lt;/name&amp;gt;&amp;lt;value&amp;gt;%2&amp;lt;/value&amp;gt;&amp;lt;/measurement&amp;gt;");
	QString measurement = defineJenkinsMeasurememt("FPS", QString::number(custusX.mMeasuredFPS));
	std::cout << measurement << std::endl;

	// TODO: enter this value into config file
	double minimumFPS = 5;
	REQUIRE(custusX.mMeasuredFPS > minimumFPS);

//	TestSenderPtr sender(new TestSender());
//	bool sendImageOnce = true;
//	bool sendTwoStreams = false;
//	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender, sendTwoStreams, sendImageOnce);

//	REQUIRE(waitForSignal(sender.get(), SIGNAL(newPackage())));
//	checkSenderGotImageFromStreamer(sender);

//	REQUIRE_FALSE(waitForSignal(sender.get(), SIGNAL(newPackage())));

//	imagestreamer->stopStreaming();
}

}//namespace cx