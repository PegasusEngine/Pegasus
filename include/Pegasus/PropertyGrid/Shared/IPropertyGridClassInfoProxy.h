/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IPropertyGridManagerProxy.h
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
    virtual ~IPropertyGridClassInfoProxy() {};


    //! Get the name of the class associated with the class info
    //! \return Name of the class associated with the class info (non-empty)
    virtual const char * GetClassName() const = 0;

    //! Get the proxy of the parent class if defined
    //! \return Proxy of the parent class if defined, nullptr if the class is a base class
    virtual const IPropertyGridClassInfoProxy * GetParentClassInfo() const = 0;


    //! Structure holding the description of a property
    struct PropertyRecord
    {
        PropertyType type;          //!< Type of the property, PROPERTYTYPE_xxx constant
        int size;                   //!< Size in bytes of the property (> 0)
        const char * name;          //!< Name of the property, starting with an uppercase letter (non-empty)
        const char * typeName;      //!< Name of the property type.
        void * defaultValuePtr;     //!< Pointer to the default value of the property
    };

    //! Get the number of registered properties, for the current class only
    //! \return Number of registered properties, for the current class only
    virtual unsigned int GetNumClassProperties() const = 0;

    //! Get the property record for one of the registered properties, for the current class only
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \return Record for the property
    virtual PropertyRecord GetClassProperty(unsigned int index) const = 0;

    //! Get the number of registered properties, including parent classes (but not derived classes)
    //! \return Number of registered properties, including parent classes (but not derived classes)
    virtual unsigned int GetNumProperties() const = 0;

    //! Get the property record for one of the registered properties, including parent classes (but not derived classes)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Record for the property
    virtual PropertyRecord GetProperty(unsigned int index) const = 0;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDCLASSINFOPROXY_H
