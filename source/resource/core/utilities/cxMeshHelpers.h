#include <vtkPolyData.h>

#include "cxTool.h"

/**
 * MeshHelpers
 *
 * \date Dec 16, 2010
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */

namespace cx
{
class DataManager;

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

vtkPolyDataPtr polydataFromTransforms(TimedTransformMap transformMap_prMt, Transform3D rMpr);
void loadMeshFromToolTransforms(DataServicePtr dataManager, TimedTransformMap transforms_prMt);

/**
 * Get information about a ssc mesh.
 */
std::map<std::string, std::string> getDisplayFriendlyInfo(MeshPtr mesh);

/**
 * \}
 */

}//namespace cx
