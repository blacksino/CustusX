/*
 * cxRegistrationDataAdapters.cpp
 *
 *  Created on: Jun 16, 2011
 *      Author: christiana
 */

#include "cxRegistrationDataAdapters.h"
#include "cxRegistrationManager.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"

namespace cx
{


RegistrationFixedImageStringDataAdapter::RegistrationFixedImageStringDataAdapter(RegistrationManagerPtr regManager) :
		mManager(regManager)
{
  connect(mManager.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(changed()));
}
QString RegistrationFixedImageStringDataAdapter::getValueName() const
{
  return "Fixed Volume";
}

bool RegistrationFixedImageStringDataAdapter::setValue(const QString& value)
{
  std::cout << "RegistrationFixedImageStringDataAdapter::setImageSlot " << value << std::endl;

  ssc::DataPtr newImage = ssc::dataManager()->getData(value);
  if (newImage==mManager->getFixedData())
    return false;
  mManager->setFixedData(newImage);
  return true;
}
QString RegistrationFixedImageStringDataAdapter::getValue() const
{
  ssc::DataPtr image = mManager->getFixedData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}
QString RegistrationFixedImageStringDataAdapter::getHelp() const
{
  return "Select the fixed registration data";
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationMovingImageStringDataAdapter::RegistrationMovingImageStringDataAdapter(RegistrationManagerPtr regManager) :
		mManager(regManager)
{
  connect(mManager.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(changed()));
}
QString RegistrationMovingImageStringDataAdapter::getValueName() const
{
  return "Moving Volume";
}
bool RegistrationMovingImageStringDataAdapter::setValue(const QString& value)
{
  ssc::DataPtr newImage = ssc::dataManager()->getData(value);
  if (newImage==mManager->getMovingData())
    return false;
  mManager->setMovingData(newImage);
  return true;
}
QString RegistrationMovingImageStringDataAdapter::getValue() const
{
  ssc::DataPtr image = mManager->getMovingData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}
QString RegistrationMovingImageStringDataAdapter::getHelp() const
{
  return "Select the moving registration data";
}



}
