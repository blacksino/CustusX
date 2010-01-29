#ifndef CXCONTEXTDOCKWIDGET_H_
#define CXCONTEXTDOCKWIDGET_H_

#include <QDockWidget>

#include "sscImage.h"

class QTabWidget;
class QVBoxLayout;
class QComboBox;

namespace cx
{
class RegistrationManager;
class ToolManager;
class MessageManager;
class ViewManager;
class RepManager;
class DataManager;


/**
 * \class ContextDockWidget
 *
 * \brief Dock widget for context specific information. Lays out the widgets
 * using tabs.
 *
 * \date Mar 16, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class ContextDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  ContextDockWidget(QWidget* parent); ///< sets up layout and connects signals and slots
  ~ContextDockWidget(); ///< empty

  int addTab(QWidget * page, const QString & label); ///< add a context specific widget
  void removeTab(int tabIndex); ///< remove a context specific widget

signals:
  void currentImageChanged(ssc::ImagePtr currentImage); ///< sends out a signal when the user chooses a different image to work on
  void changeTabIndex(int index); ///< Send a signal when the tab index must be changed
  void deleteImage(ssc::ImagePtr image);
  
public slots:
  void deleteCurrentImageSlot(); ///< Deletes the current image and emits delete signal to managers
  
protected slots:
  void visibilityOfDockWidgetChangedSlot(bool visible); ///< connects/disconnects to signals from the datamanager
  void populateTheImageComboBoxSlot(); ///< populates the combobox with the images loaded into the datamanager
  void currentImageDeletedSlot(); ///< sends out the currentImageChanged signal with an empty image
  void imageSelectedSlot(const QString& comboBoxText); ///< sends out a signal and adds reps of the image to the views

protected:
  //gui
  QWidget* mGuiWidget; ///< widget for adding the gui items (because QDockWidget already has a layout...)
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QComboBox* mImagesComboBox; ///< combobox for displaying available images
  QTabWidget* mTabWidget; ///< tab widget for adding the incoming context specific widgets

  //managers
  DataManager* mDataManager; ///< has all the data loaded into the system
  RegistrationManager* mRegistrationManager; ///< handles image and patient registration
  ToolManager* mToolManager; ///< interface to the navigation system
  MessageManager* mMessageManager; ///< takes messages intended for the user
  ViewManager* mViewManager; ///< controls layout of views and has a pool of views
  RepManager* mRepManager; ///< has a pool of reps

  //data
  ssc::ImagePtr mCurrentImage; ///< the currently selected image

private:
  ContextDockWidget(); ///< not implemented
};
}//namespace cx
#endif /* CXCONTEXTDOCKWIDGET_H_ */
