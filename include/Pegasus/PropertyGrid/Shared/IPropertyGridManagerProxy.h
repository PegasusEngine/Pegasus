/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IPropertyGridManagerProxy.h
//! \author	Karolyn Boulanger
//! \date	25th May 2015
//! \brief	Proxy interface, used by the editor to interact with the property grid manager

#ifndef PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDMANAGERPROXY_H
#define PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
    namespace PropertyGrid {
        class IPropertyGridClassInfoProxy;
    }
}

namespace Pegasus {
namespace PropertyGrid {


//! Proxy interface, used by the editor to interact with the property grid manager
class IPropertyGridManagerProxy
{
public:

    //! Destructor
    virtual ~IPropertyGridManagerProxy() {};

    
    //! Get the number of registered classes that contain a property grid
    //! \return Number of registered classes
    virtual unsigned int GetNumRegisteredClasses() const = 0;

    //! Get the proxy of one of the registered classes
    //! \param index Index of the class to get info from (< GetNumRegisteredClasses())
    //! \return Information about the registered class
    virtual const IPropertyGridClassInfoProxy * GetClassInfo(unsigned int index) const = 0;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDMANAGERPROXY_H
