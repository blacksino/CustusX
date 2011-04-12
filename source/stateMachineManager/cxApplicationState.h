/*
 * cxApplicationState.h
 *
 *  Created on: Aug 17, 2010
 *      Author: christiana
 */

#ifndef CXAPPLICATIONSTATE_H_
#define CXAPPLICATIONSTATE_H_


#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "sscEnumConverter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxRequestEnterStateTransition.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{

/**
 */
class ApplicationState : public QState
{
  Q_OBJECT
public:
  ApplicationState(QState* parent, QString uid, QString name);
  virtual ~ApplicationState();
  virtual void onEntry(QEvent * event );
  virtual void onExit(QEvent * event );
  virtual QString getUid() const;
  virtual QString getName() const;
  std::vector<ApplicationState*> getChildStates();
  QAction* createAction(QActionGroup* group);
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const = 0;
protected slots:
  void setActionSlot();
protected:
  QString mUid;
  QString mName;
  QAction* mAction;
  bool mActive;
};

class ParentApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  ParentApplicationState(QState* parent) : ApplicationState(parent, "ParentUid", "Parent") {}
  virtual void onEntry(QEvent * event ) {}
  virtual void onExit(QEvent * event ) {}
  virtual ~ParentApplicationState(){};
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdCOUNT; }
};

class LaboratoryApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  LaboratoryApplicationState(QState* parent) : ApplicationState(parent, enum2string(ssc::mdLABORATORY), enum2string(ssc::mdLABORATORY)) {}
  virtual ~LaboratoryApplicationState(){};
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdLABORATORY; }
};

class BronchoscopyApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  BronchoscopyApplicationState(QState* parent) : ApplicationState(parent, enum2string(ssc::mdBRONCHOSCOPY), enum2string(ssc::mdBRONCHOSCOPY)) {}
  virtual ~BronchoscopyApplicationState(){};
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdBRONCHOSCOPY; }
};

class NeurologyApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  NeurologyApplicationState(QState* parent) : ApplicationState(parent, enum2string(ssc::mdNEUROLOGY), enum2string(ssc::mdNEUROLOGY)) {}
  virtual ~NeurologyApplicationState() {}
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdNEUROLOGY; }
};

class LaparoscopyApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  LaparoscopyApplicationState(QState* parent) : ApplicationState(parent, enum2string(ssc::mdLAPAROSCOPY), enum2string(ssc::mdLAPAROSCOPY)) {}
  virtual ~LaparoscopyApplicationState() {}
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdLAPAROSCOPY; }
};

class EndovascularApplicationState : public ApplicationState
{
  Q_OBJECT
public:
  EndovascularApplicationState(QState* parent) : ApplicationState(parent, enum2string(ssc::mdENDOVASCULAR), enum2string(ssc::mdENDOVASCULAR)) {}
  virtual ~EndovascularApplicationState() {}
  virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdENDOVASCULAR; }
};

} // namespace cx


#endif /* CXAPPLICATIONSTATE_H_ */
