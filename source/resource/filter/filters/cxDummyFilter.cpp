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

#include "cxDummyFilter.h"
#include "cxTypeConversions.h"
#include "cxSelectDataStringProperty.h"
#include "cxData.h"
#include "cxDoubleProperty.h"
#include "cxStringProperty.h"
#include "cxBoolProperty.h"
#include "cxVisServices.h"

namespace cx
{

QString DummyFilter::getName() const
{
	return "Dummy Filter";
}

QString DummyFilter::getType() const
{
	return "DummyFilter";
}

QString DummyFilter::getHelp() const
{
	return "A dummy implementation of the cx::Filter class,\n"
	        "intended for concept testing.\n"
	        "Can also be used as a template for new filters.";
}

StringPropertyBasePtr DummyFilter::getStringOption(QDomElement root)
{
	QStringList list;
	list << "String0" << "String1";
	return StringProperty::initialize("String", "", "dummy string option",
												 list[0], list, root);
}

DoublePropertyBasePtr DummyFilter::getDoubleOption(QDomElement root)
{
	return DoubleProperty::initialize("Value", "",
	                                             "dummy double value.", 1, DoubleRange(0.1, 10, 0.01), 2,
												 root);
}

BoolPropertyBasePtr DummyFilter::getBoolOption(QDomElement root)
{
	return BoolProperty::initialize("Bool0", "",
	                                           "Dummy bool value.", false, root);
}

void DummyFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getStringOption(mOptions));
	mOptionsAdapters.push_back(this->getDoubleOption(mOptions));
	mOptionsAdapters.push_back(this->getBoolOption(mOptions));
}

void DummyFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->getPatientService());
	temp->setValueName("Input");
	temp->setHelp("Dummy data input for a dummy algorithm");
	mInputTypes.push_back(temp);

	temp = StringPropertySelectImage::New(mServices->getPatientService());
	temp->setValueName("Image");
	temp->setHelp("Dummy image input for a dummy algorithm");
	mInputTypes.push_back(temp);

	temp = StringPropertySelectMesh::New(mServices->getPatientService());
	temp->setValueName("Mesh");
	temp->setHelp("Dummy mesh input for a dummy algorithm");
	mInputTypes.push_back(temp);
}

void DummyFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->getPatientService());
	temp->setValueName("Output");
	temp->setHelp("Dummy output from the dummy algorithm");
	mOutputTypes.push_back(temp);
}

bool DummyFilter::execute()
{
	StringPropertyBasePtr stringOption = this->getStringOption(mCopiedOptions);
	DoublePropertyBasePtr doubleOption = this->getDoubleOption(mCopiedOptions);
	BoolPropertyBasePtr boolOption = this->getBoolOption(mCopiedOptions);

	std::cout << "Running dummy algorithm..." << std::endl;
	std::cout << QString("  String option [%1]: %2").arg(stringOption->getDisplayName()).arg(stringOption->getValue()) << std::endl;
	std::cout << QString("  Double option [%1]: %2").arg(doubleOption->getDisplayName()).arg(doubleOption->getValue()) << std::endl;
	std::cout << QString("  Bool   option [%1]: %2").arg(boolOption->getDisplayName()).arg(boolOption->getValue()) << std::endl;

	for (unsigned i=0; i< mCopiedInput.size(); ++i)
		std::cout << QString("  Input %1: %2").arg(mCopiedInput[i] ? mCopiedInput[i]->getName() : "NULL") << std::endl;

	std::cout << "Returning input as output." << std::endl;

	return true;
}

bool DummyFilter::postProcess()
{
	//TODO: add stuff such as saving to dataManager here.
	std::cout << "DummyFilter::postProcess " << mCopiedInput.size() << std::endl;

	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	//    return mInput;
	return true;
}



} // namespace cx