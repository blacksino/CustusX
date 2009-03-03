#ifndef CXMAINWINDOW_H_
#define CXMAINWINDOW_H_

#include <QMainWindow>
//#include <boost/shared_ptr.hpp>

/**
 * cxMainWindow.h
 *
 * \brief
 *
 * \date Jan 20, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

class QAction;
class QMenu;
class QActionGroup;

namespace ssc
{
class DataManager;
}

namespace cx
{
typedef ssc::DataManager DataManager;

class RepManager;
class ViewManager;
class ToolManager;
class MessageManager;
class CustomStatusBar;
class ImageRegistrationDockWidget;
class PatientRegistrationDockWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

protected slots:
  //application menu
  void aboutSlot();
  void preferencesSlot();
  void quitSlot();

  //workflow menu
  void patientDataWorkflowSlot();
  void imageRegistrationWorkflowSlot();
  void patientRegistrationWorkflowSlot();
  void navigationWorkflowSlot();
  void usAcquisitionWorkflowSlot();

  //data menu
  void loadDataSlot();

  //tool menu
  void configureSlot();

  //TODO: REMOVE
  //degugging
  void printSlot(std::string message);


protected:
  enum WorkflowState
  {
    PATIENT_DATA,
    IMAGE_REGISTRATION,
    PATIENT_REGISTRATION,
    NAVIGATION,
    US_ACQUISITION
  };

  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar(); //TODO

  //Takes care of removing and adding widgets depending on which workflow state the system is in
  void changeState(WorkflowState fromState, WorkflowState toState);
  void activatePatientDataState();
  void deactivatePatientDataState();
  void activateImageRegistationState();
  void deactivateImageRegistationState();
  void activatePatientRegistrationState();
  void deactivatePatientRegistrationState();
  void activateNavigationState();
  void deactivateNavigationState();
  void activateUSAcquisitionState();
  void deactivateUSAcquisitionState();

  WorkflowState mCurrentWorkflowState;

  ViewManager* mViewManager;
  DataManager* mDataManager;
  ToolManager* mToolManager;
  RepManager* mRepManager;
  MessageManager* mMessageManager;

  QWidget* mCentralWidget;

  QMenu* mWorkflowMenu;
  QMenu* mDataMenu;
  QMenu* mToolMenu;
  QMenu* mLayoutMenu;

  QAction* mPatientDataWorkflowAction;
  QAction* mImageRegistrationWorkflowAction;
  QAction* mPatientRegistrationWorkflowAction;
  QAction* mNavigationWorkflowAction;
  QAction* mUSAcquisitionWorkflowAction;
  QActionGroup* mWorkflowActionGroup;

  QAction* mLoadDataAction;

  QAction* mConfigureToolsAction;
  QAction* mInitializeToolsAction;
  QAction* mStartTrackingToolsAction;
  QAction* mStopTrackingToolsAction;
  QActionGroup* mToolsActionGroup;

  QAction* m3D_1x1_LayoutAction;
  QAction* m3DACS_2x2_LayoutAction;
  QAction* m3DACS_1x3_LayoutAction;
  QAction* mACSACS_2x3_LayoutAction;
  QActionGroup* mLayoutActionGroup;

  QToolBar* mDataToolBar; //load
  QToolBar* mToolToolBar; //start and stop

  ImageRegistrationDockWidget* mImageRegistrationDockWidget;
  PatientRegistrationDockWidget* mPatientRegistrationDockWidget;
  //cxVolumetricTFsDockWidget* mVolumetricTFsDockWidget; //TODO
  //CustomStatusBar* mCustomStatusBar; //TODO, not working yet

  //Prefrences
  QString mCurrentPatientDataFolder;
  QString mCurrentToolConfigFile;

};
}//namespace cx

#endif /* CXMAINWINDOW_H_ */
