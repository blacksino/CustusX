#include "cxViewGroupInria.h"

#include "vtkImageData.h"
#include "sscView.h"
#include "cxRepManager.h"
#include "cxInriaRep2D.h"
#include "cxMessageManager.h"



namespace cx
{
ViewGroupInria::ViewGroupInria(int startIndex, ssc::View* view1,
    ssc::View* view2, ssc::View* view3) :
    mStartIndex(startIndex)
{
  mViews.push_back(view1);
  mViews.push_back(view2);
  mViews.push_back(view3);

  this->connectContextMenu();
}

ViewGroupInria::~ViewGroupInria()
{}

void ViewGroupInria::setImage(ssc::ImagePtr image)
{
  mImage = image;
  RepManager* repManager = RepManager::getInstance();

  int baseIndex = 3*mStartIndex -2;
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+0));
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+1));
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+2));

  inriaRep2D_1->setImage(mImage);
  inriaRep2D_2->setImage(mImage);
  inriaRep2D_3->setImage(mImage);

  if (!mImage)
  {
    return;
  }
  if (!mImage->getRefVtkImageData().GetPointer())
  {
    messageManager()->sendWarning("ViewManager::currentImageChangedSlot vtk image missing from current image!");
    return;
  }

  mViews[0]->setRep(inriaRep2D_1);
  mViews[1]->setRep(inriaRep2D_2);
  mViews[2]->setRep(inriaRep2D_3);

  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(mImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();
}

void ViewGroupInria::removeImage(ssc::ImagePtr image)
{
  if(mImage!=image)
    return;

  messageManager()->sendInfo("removeImage from inria view group: "+image->getName());
  RepManager* repManager = RepManager::getInstance();

  int baseIndex = 3*mStartIndex -2;
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+0));
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+1));
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+2));

  if(inriaRep2D_1->hasImage(mImage))
  {
    mViews[0]->removeRep(inriaRep2D_1);
    mViews[1]->removeRep(inriaRep2D_2);
    mViews[2]->removeRep(inriaRep2D_3);
    inriaRep2D_1->getVtkViewImage2D()->SyncRemoveDataSet(image->getRefVtkImageData());

    // Test: create small dummy data set with one voxel
    vtkImageDataPtr dummyImageData = vtkImageData::New();
    dummyImageData->SetExtent(0,0,0,0,0,0);
    dummyImageData->SetSpacing(1,1,1);
    //dummyImageData->SetScalarTypeToUnsignedShort();
    dummyImageData->SetScalarTypeToUnsignedChar();
    dummyImageData->SetNumberOfScalarComponents(1);
    dummyImageData->AllocateScalars();
    unsigned char* dataPtr = static_cast<unsigned char*>(dummyImageData->GetScalarPointer());
    dataPtr = 0;//Set voxel to black
    inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(dummyImageData);
    inriaRep2D_1->getVtkViewImage2D()->SyncReset();

    //emit imageDeletedFromViews(mImage);
    messageManager()->sendInfo("Removed current image from inria view group");
  }
  mImage.reset();
}

void ViewGroupInria::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  RepManager* repManager = RepManager::getInstance();
  int baseIndex = 3*mStartIndex -2;
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+0));
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+1));
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_"+toString(baseIndex+2));

  ssc::ProbeRepPtr probeRep = repManager->getProbeRep("ProbeRep_"+toString(mStartIndex));

  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    connect(probeRep.get(),     SIGNAL(pointPicked(double,double,double)),inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
    connect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    connect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    connect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  }
  if (mode!=ssc::rsIMAGE_REGISTRATED)
  {
    disconnect(probeRep.get(),     SIGNAL(pointPicked(double,double,double)),inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
    disconnect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    disconnect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    disconnect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  }
}
}//namespace cx
