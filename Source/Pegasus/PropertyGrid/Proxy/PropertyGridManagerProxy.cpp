/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridManagerProxy.cpp
//! \author	Karolyn Boulanger
//! \date	25th May 2015
//! \brief	Proxy object, used by the editor to interact with the property grid manager

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Proxy/PropertyGridManagerProxy.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/PropertyGrid/Proxy/PropertyGridEnumTypeInfo.h"
#include "Pegasus/PropertyGrid/PropertyGridEnumType.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGridManagerProxy::PropertyGridManagerProxy()
{
}

//----------------------------------------------------------------------------------------

PropertyGridManagerProxy::~PropertyGridManagerProxy()
{
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridManagerProxy::GetNumRegisteredClasses() const
{
    return PropertyGridManager::GetInstance().GetNumRegisteredClasses();
}

//----------------------------------------------------------------------------------------

const IPropertyGridClassInfoProxy * PropertyGridManagerProxy::GetClassInfo(unsigned int index) const
{
    return PropertyGridManager::GetInstance().GetClassInfo(index).GetProxy();
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridManagerProxy::GetNumRegisteredEnumInfos() const
{
    return PropertyGridManager::GetInstance().GetNumRegisteredEnumInfos();
}

//----------------------------------------------------------------------------------------

const IEnumTypeInfoProxy* PropertyGridManagerProxy::GetEnumInfo(unsigned int index) const
{
    const EnumTypeInfo* info = PropertyGridManager::GetInstance().GetEnumInfo(index);
    if (info != nullptr)
    {
        return info->GetProxy();
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------

const IEnumTypeInfoProxy* PropertyGridManagerProxy::GetEnumInfo(const char* enumName) const
{
    const EnumTypeInfo* info = PropertyGridManager::GetInstance().GetEnumInfo(enumName);
    if (info != nullptr)
    {
        return info->GetProxy();
    }

    return nullptr;
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
