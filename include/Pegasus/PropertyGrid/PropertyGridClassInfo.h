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

    //! Get the number of registered properties using \a RegisterProperty()
    //! \return Number of successfully registered properties
    inline unsigned int GetNumProperties() const { return mPropertyRecords.GetSize(); }

    //! Get the type of a property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Type of the property, PROPERTYTYPE_xxx constant, PROPERTYTYPE_INVALID in case of error
    PropertyType GetPropertyType(unsigned int index) const;

    //! Get the size of a property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Size in bytes of the property, 0 in case of error
    unsigned int GetPropertySize(unsigned int index) const;

    //! Get the name of a property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Name of the property, empty string in case of error
    const char * GetPropertyName(unsigned int index) const;

    //! Get the name of the class owning the property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Name of the class owning the property, empty string in case of error
    
    //! \todo IMPLEMENT, used for having properties sorted by parent class
    //const char * GetPropertyClassName(unsigned int index) const;

    //------------------------------------------------------------------------------------
    
private:

    // No copies allowed
    PG_DISABLE_COPY(PropertyGridClassInfo);


    //! Set the name of the class owner of this class data
    //! \warning Has to be called at least once for the class info to be valid
    //! \param className Non-empty string for the name of the owner class (not copied, the owner must be external)
    void SetClassName(const char * className);

    //! Register a property
    //! \param type Type of the property, PROPERTYTYPE_xxx constant
    //! \param name Name of the property, starting with an uppercase letter
    //! \param varPtr ***** Pointer to the property variable
    //! \param className ***** Class name of the property grid owner
    //! \note Called by \a IMPLEMENT_PROPERTY() through the templated \a RegisterProperty() function
    void RegisterProperty(PropertyType type, int size, const char * name/*,
        //! \todo **** Check if we need those variables
                          void * varPtr, const char * className*/);


    //! Name of the class owning the property (non-empty when valid)
    //! \warning Not copied, the owner must be external
    const char * mClassName;

    //! Record for one property, containing information such as the name
    struct PropertyRecord
    {
        PropertyType type;          //!< Type of the property, PROPERTYTYPE_xxx constant
        int size;                   //!< Size in bytes of the property (> 0)
        const char * name;          //!< Name of the property, starting with an uppercase letter (non-empty)
                                    //!< (has to be a pointer to a global constant, does not store the string)
    //! \todo **** Check if we need those variables
        /*void * varPtr;*/              //!< Pointer to the variable (!= nullptr)
        /*const char * className;*/     //!< Name of the class owning the property (non-empty)
    };

    //! List of property records
    Utils::Vector<PropertyRecord> mPropertyRecords;

    // Give access to \a RegisterProperty from the manager
    friend class PropertyGridManager;
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRIDCLASSINFO_H
