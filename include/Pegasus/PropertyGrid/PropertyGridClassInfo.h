/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassInfo.h
//! \author	Kevin Boulanger
//! \date	28th January 2015
//! \brief	Information about a class containing a property grid

#ifndef PEGASUS_PROPERTYGRID_PROPERTYGRIDCLASSINFO_H
#define PEGASUS_PROPERTYGRID_PROPERTYGRIDCLASSINFO_H

#include "Pegasus/PropertyGrid/Property.h"
#include "Pegasus/PropertyGrid/Proxy/PropertyGridClassInfoProxy.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus {
namespace PropertyGrid {


//! Information about a class containing a property grid
class PropertyGridClassInfo
{
public:

    //! Constructor
    PropertyGridClassInfo();

    // Destructor
    // (defined to work properly with Utils::Vector)
    ~PropertyGridClassInfo() { };

    //! Get the name of the class owner of this class data
    //! \return String for the name of the owner class
    inline const char * GetClassName() const { return mClassName; }

    //! Get the name of the parent class of the class owner of this class data
    //! \return String for the name of the parent class of the owner class, empty if the class is a base class
    inline const char * GetParentClassName() const { return mParentClassName; }

    //! Get the class information of the parent if defined
    //! \return Pointer to the parent class info if defined, nullptr if no parent is defined
    inline const PropertyGridClassInfo * GetParentClassInfo() const { return mParentClassInfo; }

    //----------------------------------------------------------------------------------------
    
    //! Record for one property, containing information such as the name
    struct PropertyRecord
    {
        PropertyType type;          //!< Type of the property, PROPERTYTYPE_xxx constant
        int size;                   //!< Size in bytes of the property (> 0)
        const char * name;          //!< Name of the property, starting with an uppercase letter (non-empty)
                                    //!< (has to be a pointer to a global constant, does not store the string)
        void * defaultValuePtr;     //!< Pointer to the default value of the property
    };


    //! Get the number of registered properties for the current class only
    //! \return Number of successfully registered properties for the current class only
    inline unsigned int GetNumClassProperties() const { return mClassPropertyRecords.GetSize(); }

    //! Get the record of a property for the current class only
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \return Record of the property
    const PropertyRecord & GetClassProperty(unsigned int index) const;

    //! Get the number of registered properties, including parent classes (but not derived classes)
    //! \return Number of successfully registered properties
    inline unsigned int GetNumProperties() const { return mNumProperties; }

    //! Get the record of a property, including parent classes (but not derived classes)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Record of the property
    const PropertyRecord & GetProperty(unsigned int index) const;

    //------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the property grid class info
    //! \return Proxy associated with the property grid class info
    //@{
    inline PropertyGridClassInfoProxy * GetProxy() { return &mProxy; }
    inline const PropertyGridClassInfoProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
private:

    // No copies allowed
    PG_DISABLE_COPY(PropertyGridClassInfo);


    //! Set the name of the class owner of this class data
    //! \warning Has to be called at least once for the class info to be valid
    //! \param className Non-empty string for the name of the owner class (not copied, the owner must be external)
    //! \param parentClassName String for the name of the parent of the owner class (not copied, the owner must be external),
    //!                        empty string if this class is a base class
    void SetClassName(const char * className, const char * parentClassName);

    //! Give the class information of the parent if defined
    //! \note If not called, the class is considered as having no parent
    //! \param parentClassInfo Pointer to the parent class info if defined
    void SetParentClassInfo(PropertyGridClassInfo * parentClassInfo);

    //! Declare a property
    //! \param type Type of the property, PROPERTYTYPE_xxx constant
    //! \param size Size in bytes of the property (> 0)
    //! \param name Name of the property, starting with an uppercase letter (non-empty)
    //! \param defaultValuePtr Pointer to the default value of the property
    //! \note Called by \a IMPLEMENT_PROPERTY() through the templated \a RegisterProperty() function
    void DeclareProperty(PropertyType type, int size, const char * name, void * defaultValuePtr);

    //! Compute the number of properties (mNumProperties), including parent classes (but not derived classes)
    //! \note The classes are not declared in a specific order,
    //!       the parent's number of properties is not known yet at declaration time
    void UpdateNumPropertiesFromParents();

    //! Name of the class owning the property (non-empty when valid)
    //! \warning Not copied, the owner must be external
    const char * mClassName;

    //! Name of the parent of the class owning the property, empty string if this class is a base class
    //! \warning Not copied, the owner must be external
    const char * mParentClassName;

    //! Pointer to the class info of the parent class, nullptr if there is no parent class
    PropertyGridClassInfo * mParentClassInfo;


    //! List of property records for the current class
    Utils::Vector<PropertyRecord> mClassPropertyRecords;

    //! Number of properties for the current class and parent classes combined
    unsigned int mNumProperties;

    // Give access to \a RegisterProperty from the manager
    friend class PropertyGridManager;


#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the property grid class info
    PropertyGridClassInfoProxy mProxy;

#endif  // PEGASUS_ENABLE_PROXIES
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRIDCLASSINFO_H
