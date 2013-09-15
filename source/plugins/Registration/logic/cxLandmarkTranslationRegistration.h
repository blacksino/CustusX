/*
 * cxLandmarkTranslationRegistration.h
 *
 *  \date Sep 27, 2010
 *      \author christiana
 */

#ifndef CXLANDMARKTRANSLATIONREGISTRATION_H_
#define CXLANDMARKTRANSLATIONREGISTRATION_H_

#include <vector>
#include "sscTransform3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/** Use ITK to perform a landmark registration in 3D with translation only.
 *
 * Code is based on  itk/Examples/Registration/IterativeClosestPoint1.cxx
 *
 */
class LandmarkTranslationRegistration
{
public:
  Transform3D registerPoints(std::vector<Vector3D> ref, std::vector<Vector3D> target, bool* ok);
};

/**
 * @}
 */
}

#endif /* CXLANDMARKTRANSLATIONREGISTRATION_H_ */
