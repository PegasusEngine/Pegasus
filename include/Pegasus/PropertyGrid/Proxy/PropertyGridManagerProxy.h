/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridManagerProxy.h
//! \author	Kevin Boulanger
//! \date	25th May 2015
//! \brief	Proxy object, used by the editor to interact with the property grid manager

#ifndef PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDMANAGERPROXY_H
#define PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Shared/IPropertyGridManagerProxy.h"

namespace Pegasus {
namespace PropertyGrid {


//! Proxy object, used by the editor to interact with the property grid manager
class PropertyGridManagerProxy : public IPropertyGridManagerProxy
{
public:

    // Constructor
    PropertyGridManagerProxy();

    //! Destructor
    virtual ~PropertyGridManagerProxy();

    
    //! Get the number of registered classes that contain a property grid
    //! \return Number of registered classes
    virtual unsigned int GetNumRegisteredClasses() const;

    //! Get the proxy of one of the registered classes
    //! \param index Index of the class to get info from (< GetNumRegisteredClasses())
    //! \return Information about the registered class
    virtual const IPropertyGridClassInfoProxy * GetClassInfo(unsigned int index) const;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDMANAGERPROXY_H
