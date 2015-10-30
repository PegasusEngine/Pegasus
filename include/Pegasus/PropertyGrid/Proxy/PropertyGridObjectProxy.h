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

    //! Get the number of registered properties for the current class only
    //! \return Number of successfully registered properties for the current class only
    virtual unsigned int GetNumClassProperties() const override;

    //! Get the record of a property for the current class only
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \return Record of the property (information about the property)
    virtual const PropertyRecord & GetClassPropertyRecord(unsigned int index) const override;

    //! Read a property of the current class into a buffer
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) override;

    //! Write a property of the current class using the content of a buffer
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \param inputBuffer Input buffer with content to copy to the property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) override;


    //! Get the number of registered properties, including parent classes (but not derived classes)
    //! \return Number of successfully registered properties
    virtual unsigned int GetNumProperties() const override;

    //! Get the record of a property, including parent classes (but not derived classes)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Record of the property (information about the property)
    virtual const PropertyRecord & GetPropertyRecord(unsigned int index) const override;

    //! Read a property into a buffer
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    virtual void ReadProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) override;

    //! Write the property using the content of a buffer
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \param inputBuffer Input buffer with content to copy to the property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) override;

    //------------------------------------------------------------------------------------

private:

    //! Proxied property grid object
    PropertyGridObject * const mObject;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_PROPERTYGRID_PROXY_PROPERTYGRIDOBJECTPROXY_H
