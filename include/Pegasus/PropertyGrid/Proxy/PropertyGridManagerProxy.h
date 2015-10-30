/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridManagerProxy.h
//! \author	Karolyn Boulanger
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
    virtual unsigned int GetNumRegisteredClasses() const override;

    //! Get the proxy of one of the registered classes
    //! \param index Index of the class to get info from (< GetNumRegisteredClasses())
    //! \return Information about the registered class
    virtual const IPropertyGridClassInfoProxy * GetClassInfo(unsigned int index) const override;

    //! Gets the enumeration records
    //! \return Enumeration type records
    virtual unsigned int GetNumRegisteredEnumInfos() const override;

    //! Gets a pointer to the enumeration info
    //! \param index Index of the enumeration type
    //! \return Name of the enumeration type info
    virtual const IEnumTypeInfoProxy* GetEnumInfo(unsigned int index) const override;

    //! Gets a pointer to the enumeration info
    //! \param enumName Name of the enumeration type
    //! \return Name of the enumeration type info
    virtual const IEnumTypeInfoProxy* GetEnumInfo(const char* enumName) const override;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDMANAGERPROXY_H
