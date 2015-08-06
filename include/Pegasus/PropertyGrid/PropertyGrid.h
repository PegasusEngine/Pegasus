/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGrid.h
//! \author	Karolyn Boulanger
//! \date	24th April 2014
//! \brief	Property grid, defining a set of editable properties

#ifndef PEGASUS_PROPERTYGRID_PROPERTYGRID_H
#define PEGASUS_PROPERTYGRID_PROPERTYGRID_H

#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"

namespace Pegasus {
namespace PropertyGrid {


//class PropertyRecord : public Utils::DependsOnStatic<PropertyGridManager, PropertyGridManager::GetInstance>
//{
//    PropertyRecord()
//    {
//        PropertyGridManager::GetInstance().DeclareProperty()
//    }
//};
//
// class ClassNameProperties
// {
//    public:                                                                         \
//        inline PropertyGrid::PropertyDefinition<type>::ReturnType GetName() const { return mName; }         \
//        inline void SetName(PropertyGrid::PropertyDefinition<type>::ParamType value) { PropertyGrid::PropertyDefinition<type>::SetProperty(value, mName); /*GetPropertyGrid().Invalidate();*/ }     \
//        inline static type GetNameDefaultValue() const { return defaultValue; } \
//    private:                                                                        \
//        PropertyGrid::PropertyDefinition<type>::VarType mName;            \
//        static PropertyRecord<...> sNamePropertyRecord;
//
// } mProperties;


// Macros to declare properties in a class.

// In the header file, inside the class definition, declare properties as following:
//     BEGIN_DECLARE_PROPERTIES(ClassName)
//         DECLARE_PROPERTY(int, NumLoops)
//         DECLARE_PROPERTY(Math::Vec3, Position)
//             etc.
//     END_DECLARE_PROPERTIES()

// In the implementation file, after the namespace declaration:
//     BEGIN_IMPLEMENT_PROPERTIES()
//         IMPLEMENT_PROPERTY(int, NumLoops, 1)
//         IMPLEMENT_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 1.0f, 2.0f))
//             etc.
//     END_IMPLEMENT_PROPERTIES()


//! Macro to start declaring a set of properties
//! \param *** className Name of the class the properties belong to
//! \note To be used inside a class declaration, before calling \a DECLARE_PROPERTY()
#define BEGIN_DECLARE_PROPERTIES(/*className*/)                                                  \
    public:                                                                         \
        virtual void InitProperties();                                              \
    private:                                                                        \
        static const char * sClassName;                                             \


//! Macro to declare a property
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \warning To be used after \a BEGIN_DECLARE_PROPERTIES() and before END_DECLARE_PROPERTIES()
#define DECLARE_PROPERTY(type, name)                                                \
    public:                                                                         \
        inline PropertyGrid::PropertyDefinition<type>::ReturnType Get##name() const { return mProperty##name; }         \
        inline void Set##name(PropertyGrid::PropertyDefinition<type>::ParamType value) { PropertyGrid::PropertyDefinition<type>::CopyProperty(mProperty##name, value); GetPropertyGrid().Invalidate(); }     \
    private:                                                                        \
        PropertyGrid::PropertyDefinition<type>::VarType mProperty##name;            \


//! Macro to stop declaring a set of properties
//! \note To be used inside a class declaration, after calling \a DECLARE_PROPERTY()
#define END_DECLARE_PROPERTIES()


//! Macro to start declaring a set of properties in the implementation file
//! \param className Class name of the property grid owner
//! \note To be used inside a .cpp file, before calling \a IMPLEMENT_PROPERTY()
#define BEGIN_IMPLEMENT_PROPERTIES(className)                                       \
    const char * className::sClassName = #className;                                \
    void className::InitProperties()                                                \
    {                                                                               \


//! Macro to declare a property in the implementation file
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \param defaultValue Default value of the property, use Math::Type(values) with math types
//! \warning To be used after \a BEGIN_IMPLEMENT_PROPERTIES() and before END_IMPLEMENT_PROPERTIES()
#define IMPLEMENT_PROPERTY(type, name, defaultValue)                                \
    Pegasus::PropertyGrid::PropertyDefinition<type>::CopyProperty(mProperty##name, defaultValue);                                                 \
//    GetPropertyGrid().RegisterProperty<type>(#name, &mProperty##name, sClassName);  \


//! Macro to stop declaring a set of properties in the implementation file
//! \note To be used inside a .cpp file, after calling \a IMPLEMENT_PROPERTY()
#define END_IMPLEMENT_PROPERTIES()                                                  \
    }                                                                               \

//----------------------------------------------------------------------------------------

//! Property grid, defining a set of editable properties.
//! When properties are declared, their association accessors are created (SetName() and GetName()).
//! The property grid object stores the property definitions and gives access to string-based accessors.
//! \warning For a class to support properties, it needs to store an instance of this class,
//!          accessible through a GetPropertyGrid() accessor, returning a reference.
//!          It also needs to define an empty virtual void InitProperties() function.
//! \note Typically used inside \a Graph::Node, but can be used within any other class
class PropertyGrid
{
public:

    //! Constructor
    PropertyGrid();

    //! \todo IMPORTANT Implement copy constructor and assignment operator.
    //!       When a Camera is copied for example, the property grid is copied, but not the members.
    //!       They should be.

    //! Register a property
    //! \param T Type of the property, use Math:: in front of types from the math library
    //! \param name Name of the property, starting with an uppercase letter
    //! \param varPtr Pointer to the property variable
    //! \param className Class name of the property grid owner
    //! \note Called by \a IMPLEMENT_PROPERTY()
    template <typename T>
    void RegisterProperty(const char * name, T * varPtr, const char * className);

    //! Invalidate the property grid, to be called after a member is updated,
    //! to tell the property grid owner to regenerate its data
    inline void Invalidate() { mDirty = true; }

    //! Test if the property grid is dirty, meaning that at least one member has changed
    //! \return True if the dirty flag is set
    inline bool IsDirty() const { return mDirty; }

    //! Validate the property grid, to be called by the property grid owner
    //! to tell its data has been regenerated with the current member data
    inline void Validate() { mDirty = false; }

    //------------------------------------------------------------------------------------

    // Accessors for the proxy interface

#if PEGASUS_ENABLE_PROXIES

    ////! Get the number of registered properties using \a RegisterProperty()
    ////! \return Number of successfully registered properties
    //unsigned int GetNumProperties() const;

    ////! Get the type of a property
    ////! \param index Index of the property (0 <= index < GetNumProperties())
    ////! \return Type of the property, PROPERTYTYPE_xxx constant, PROPERTYTYPE_INVALID in case of error
    //PropertyType GetPropertyType(unsigned int index) const;

    ////! Get the name of a property
    ////! \param index Index of the property (0 <= index < GetNumProperties())
    ////! \return Name of the property, empty string in case of error
    //const char * GetPropertyName(unsigned int index) const;

    ////! Get the name of the class owning the property
    ////! \param index Index of the property (0 <= index < GetNumProperties())
    ////! \return Name of the class owning the property, empty string in case of error
    //const char * GetPropertyClassName(unsigned int index) const;

    //! Set the value of a property by name
    //! \param name Name of the property, non-empty string
    //! \param valuePtr Non-nullptr pointer to the value
    //! \note The value is copied, not its pointer
    //! \note No property is set if the parameters are invalid
    void SetProperty(const char * name, void * valuePtr);

    //! Get the value of a property by name
    //! \param name Name of the property, non-empty string
    //! \return Pointer to the value, nullptr in case of error
    const void * GetProperty(const char * name) const;

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
private:

    //! Record for one property, containing information such as the name
    //struct PropertyRecord
    //{
    //    PropertyType type;          //!< Type of the property, PROPERTYTYPE_xxx constant
    //    int size;                   //!< Size in bytes of the property (> 0)
    //    const char * name;          //!< Name of the property, starting with an uppercase letter (non-empty)
    //    void * varPtr;              //!< Pointer to the variable (!= nullptr)
    //    const char * className;     //!< Name of the class owning the property (non-empty)
    //};

    //! Register a property
    //! \param type Type of the property, PROPERTYTYPE_xxx constant
    //! \param name Name of the property, starting with an uppercase letter
    //! \param varPtr Pointer to the property variable
    //! \param className Class name of the property grid owner
    //! \note Called by \a IMPLEMENT_PROPERTY() through the templated \a RegisterProperty() function
    //void RegisterProperty(PropertyType type, int size, const char * name,
    //                      void * varPtr, const char * className);

    //! Set to true after a member is updated,
    //! to tell the property grid owner to regenerate its data
    bool mDirty;

    //! \todo Store list of PropertyRecord, filled by RegisterProperty()
};


//----------------------------------------------------------------------------------------

// Implementation

//template <typename T>
//void PropertyGrid::RegisterProperty(const char * name, T * varPtr, const char * className)
//{
//    RegisterProperty(PropertyDefinition<T>::PROPERTY_TYPE, sizeof(T), name, varPtr, className);
//}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRID_H
