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

#ifndef CXFILTERIMPL_H
#define CXFILTERIMPL_H

#include <vector>
#include <QObject>
#include "sscData.h"
#include "sscDataAdapter.h"
#include "cxDataInterface.h"
#include "cxFilter.h"
#include <QDomElement>

namespace cx
{

/** Helper implementation for Filter.
 *
 * Inherit from this class to get access to useful functionality common
 * to Filter descendants.
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 21, 2012
 * \author christiana
 */
class FilterImpl : public Filter
{
    Q_OBJECT

public:
    virtual ~FilterImpl() {}

    virtual QString getUid() const;
    virtual void setUid(QString uid);
    virtual std::vector<DataAdapterPtr> getOptions(QDomElement root);
    virtual std::vector<SelectDataStringDataAdapterBasePtr> getInputTypes();
    virtual std::vector<SelectDataStringDataAdapterBasePtr> getOutputTypes();
    virtual void setActive(bool on);

    virtual bool preProcess();

protected:
    explicit FilterImpl();

    /** Helper: Return the index'th input type as an image.
      * Return zero if not available (as image). */
    ssc::ImagePtr getCopiedInputImage(int index = 0);

    virtual void createOptions(QDomElement root) = 0;
    virtual void createInputTypes() = 0;
    virtual void createOutputTypes() = 0;

    std::vector<SelectDataStringDataAdapterBasePtr> mInputTypes;
    std::vector<SelectDataStringDataAdapterBasePtr> mOutputTypes;
    std::vector<DataAdapterPtr> mOptionsAdapters;
    QDomElement mOptions;

    // data used by execute - copied for thread safety purposes
    std::vector<ssc::DataPtr> mCopiedInput;
    QDomElement mCopiedOptions;
    bool mActive;
    QString mUid;

};

} // namespace cx


#endif // CXFILTERIMPL_H