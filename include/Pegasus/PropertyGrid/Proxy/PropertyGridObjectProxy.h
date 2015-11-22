/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridObjectProxy.h
//! \author	Karolyn Boulanger
//! \date	14th September 2015
//! \brief	Proxy object, used by the editor to interact with a property grid object

#ifndef PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDOBJECTPROXY_H
#define PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDOBJECTPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Shared/IPropertyGridObjectProxy.h"

namespace Pegasus {
    namespace PropertyGrid {
        class PropertyGridObject;
    }
}

namespace Pegasus {
namespace PropertyGrid {


//! Proxy object, used by the editor to interact with a property grid object
class PropertyGridObjectProxy : public IPropertyGridObjectProxy
{
public:

    //! Constructor
    //! \param Object associated with the proxy (!= nullptr)
    PropertyGridObjectProxy(PropertyGridObject * object);

    //! Destructor
    virtual ~PropertyGridObjectProxy();

    //! Get the object associated with the proxy
    //! \return Object associated with the proxy (!= nullptr)
    inline const PropertyGridObject * GetObject() const { return mObject; }


    //! Get the proxy for the class information
    //! \return Proxy for the class information
    virtual const IPropertyGridClassInfoProxy * GetClassInfoProxy() const override;

    //! Get the number of registered properties for the current derived class only
    //! \return Number of successfully registered properties for the current derived class only
    virtual unsigned int GetNumDerivedClassProperties() const override;

    //! Get the record of a property for the current derived class only
    //! \param index Index of the property (0 <= index < GetNumDerivedClassProperties())
    //! \return Record of the class property (information about the property)
    virtual const PropertyRecord & GetDerivedClassPropertyRecord(unsigned int index) const override;

    //! Read a property of the current derived class into a buffer
    //! \param index Index of the property (0 <= index < GetNumDerivedClassProperties())
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadDerivedClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) override;

    //! Write a property of the current derived class using the content of a buffer
    //! \param index Index of the property (0 <= index < GetNumDerivedClassProperties())
    //! \param inputBuffer Input buffer with content to copy to the property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteDerivedClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) override;


    //! Get the number of registered class properties, including parent classes (but not derived classes)
    //! \return Number of successfully registered class properties
    virtual unsigned int GetNumClassProperties() const override;

    //! Get the record of a class property, including parent classes (but not derived classes)
    //! \param index Index of the class property (0 <= index < GetNumClassProperties())
    //! \return Record of the class property (information about the class property)
    virtual const PropertyRecord & GetClassPropertyRecord(unsigned int index) const override;

    //! Read a class property into a buffer
    //! \param index Index of the class property (0 <= index < GetNumClassProperties())
    //! \param outputBuffer Buffer that is filled with the class property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) override;

    //! Write the class property using the content of a buffer
    //! \param index Index of the class property (0 <= index < GetNumClassProperties())
    //! \param inputBuffer Input buffer with content to copy to the class property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) override;


    //! Get the number of object properties (can change any time and for any object)
    //! \return Number of available object properties
    virtual unsigned int GetNumObjectProperties() const override;

    //! Get the record of an object property
    //! \param index Index of the object property (0 <= index < GetNumObjectProperties())
    //! \return Record of the object property (information about the property)
    virtual const PropertyRecord & GetObjectPropertyRecord(unsigned int index) const override;

    //! Read an object property into a buffer
    //! \param index Index of the object property (0 <= index < GetNumObjectProperties())
    //! \param outputBuffer Buffer that is filled with the object property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadObjectProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) override;

    //! Write the object property using the content of a buffer
    //! \param index Index of the pbject property (0 <= index < GetNumObjectProperties())
    //! \param inputBuffer Input buffer with content to copy to the object property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteObjectProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) override;

    //------------------------------------------------------------------------------------

private:

    //! Proxied property grid object
    PropertyGridObject * const mObject;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDOBJECTPROXY_H
