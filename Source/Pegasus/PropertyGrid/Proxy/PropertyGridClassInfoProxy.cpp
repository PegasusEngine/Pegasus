/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassInfoProxy.cpp
//! \author	Karolyn Boulanger
//! \date	25th May 2015
//! \brief	Proxy object, used by the editor to interact with a property grid class info

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Proxy/PropertyGridClassInfoProxy.h"
#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGridClassInfoProxy::PropertyGridClassInfoProxy(const PropertyGridClassInfo * classInfo)
:   mClassInfo(classInfo)
{
    PG_ASSERT(classInfo != nullptr);
}

//----------------------------------------------------------------------------------------

PropertyGridClassInfoProxy::~PropertyGridClassInfoProxy()
{
}

//----------------------------------------------------------------------------------------

const char * PropertyGridClassInfoProxy::GetClassName() const
{
    return mClassInfo->GetClassName();
}

//----------------------------------------------------------------------------------------

const IPropertyGridClassInfoProxy * PropertyGridClassInfoProxy::GetParentClassInfo() const
{
    const PropertyGridClassInfo * const parentClassInfo = mClassInfo->GetParentClassInfo();
    if (parentClassInfo != nullptr)
    {
        return parentClassInfo->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridClassInfoProxy::GetNumDerivedClassProperties() const
{
    return mClassInfo->GetNumDerivedClassProperties();
}

//----------------------------------------------------------------------------------------

const PropertyRecord & PropertyGridClassInfoProxy::GetDerivedClassPropertyRecord(unsigned int index) const
{
    return mClassInfo->GetDerivedClassPropertyRecord(index);
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridClassInfoProxy::GetNumClassProperties() const
{
    return mClassInfo->GetNumClassProperties();
}

//----------------------------------------------------------------------------------------

const PropertyRecord & PropertyGridClassInfoProxy::GetClassPropertyRecord(unsigned int index) const
{
    return mClassInfo->GetClassPropertyRecord(index);
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
