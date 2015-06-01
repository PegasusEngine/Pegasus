/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridManagerProxy.cpp
//! \author	Kevin Boulanger
//! \date	25th May 2015
//! \brief	Proxy object, used by the editor to interact with the property grid manager

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Proxy/PropertyGridManagerProxy.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"

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


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
