#ifndef CXPOLYDATAMESHREADER_H
#define CXPOLYDATAMESHREADER_H

#include "cxPortService.h"
#include "org_custusx_core_port_Export.h"

class ctkPluginContext;

namespace cx {

/**\brief Reader for .vtk files.
 *
 */
class org_custusx_core_port_EXPORT PolyDataMeshReader: public FileReaderWriterService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	PolyDataMeshReader();
	virtual ~PolyDataMeshReader(){}
	virtual bool canLoad(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);

	virtual void save(DataPtr data, const QString& filename);

	bool isNull(){ return false; }
};

}

#endif // CXPOLYDATAMESHREADER_H