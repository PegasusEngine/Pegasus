/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IPropertyGridClassInfoProxy.h
//! \author	Karolyn Boulanger
//! \date	25th May 2015
//! \brief	Proxy interface, used by the editor to interact with a property grid class info

#ifndef PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDCLASSINFOPROXY_H
#define PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDCLASSINFOPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"

namespace Pegasus {
namespace PropertyGrid {


//! Proxy interface, used by the editor to interact with a property grid class info
class IPropertyGridClassInfoProxy
{
public:

    //! Destructor
    virtual ~IPropertyGridClassInfoProxy() { }


    //! Get the name of the class associated with the class info
    //! \return Name of the class associated with the class info (non-empty)
    virtual const char * GetClassName() const = 0;

    //! Get the proxy of the parent class if defined
    //! \return Proxy of the parent class if defined, nullptr if the class is a base class
    virtual const IPropertyGridClassInfoProxy * GetParentClassInfo() const = 0;


    //! Get the number of registered properties, for the current derived class only
    //! \return Number of registered properties, for the current derived class only
    virtual unsigned int GetNumDerivedClassProperties() const = 0;

    //! Get the property record for one of the registered properties, for the current derived class only
    //! \param index Index of the property (0 <= index < GetNumDerivedClassProperties())
    //! \return Record for the property
    virtual const PropertyRecord & GetDerivedClassPropertyRecord(unsigned int index) const = 0;

    //! Get the number of registered class properties, including parent classes (but not classes deriving from the current class)
    //! \return Number of registered class properties, including parent classes (but not classes deriving from the current class)
    virtual unsigned int GetNumClassProperties() const = 0;

    //! Get the property record for one of the registered class properties, including parent classes (but not classes deriving from the current class)
    //! \param index Index of the class property (0 <= index < GetNumClassProperties())
    //! \return Record for the class property
    virtual const PropertyRecord & GetClassPropertyRecord(unsigned int index) const = 0;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDCLASSINFOPROXY_H
