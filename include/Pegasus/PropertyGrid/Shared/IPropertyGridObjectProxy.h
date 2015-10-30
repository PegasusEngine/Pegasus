/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IPropertyGridObjectProxy.h
//! \author	Karolyn Boulanger
//! \date	25th May 2015
//! \brief	Proxy interface, used by the editor to interact with a property grid object

#ifndef PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDOBJECTPROXY_H
#define PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDOBJECTPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"

namespace Pegasus {
namespace PropertyGrid {


//! Proxy interface, used by the editor to interact with a property grid object
class IPropertyGridObjectProxy
{
public:

    //! Destructor
    virtual ~IPropertyGridObjectProxy() { }

    //! Get the proxy for the class information
    //! \return Proxy for the class information
    virtual const IPropertyGridClassInfoProxy * GetClassInfoProxy() const = 0;

    //! Get the number of registered properties for the current class only
    //! \return Number of successfully registered properties for the current class only
    virtual unsigned int GetNumClassProperties() const = 0;

    //! Get the record of a property for the current class only
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \return Record of the property (information about the property)
    virtual const PropertyRecord & GetClassPropertyRecord(unsigned int index) const = 0;

    //! Read a property of the current class into a buffer
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) = 0;

    //! Write a property of the current class using the content of a buffer
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \param inputBuffer Input buffer with content to copy to the property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) = 0;


    //! Get the number of registered properties, including parent classes (but not derived classes)
    //! \return Number of successfully registered properties
    virtual unsigned int GetNumProperties() const = 0;

    //! Get the record of a property, including parent classes (but not derived classes)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Record of the property (information about the property)
    virtual const PropertyRecord & GetPropertyRecord(unsigned int index) const = 0;

    //! Read a property into a buffer
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) = 0;

    //! Write the property using the content of a buffer
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \param inputBuffer Input buffer with content to copy to the property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) = 0;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_SHARED_IPROPERTYGRIDOBJECTPROXY_H
