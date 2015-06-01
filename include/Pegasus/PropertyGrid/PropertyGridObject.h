/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridObject.h
//! \author	Kevin Boulanger
//! \date	10th February 2015
//! \brief	Property grid object, parent of every class that defines a set of editable properties

#ifndef PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H
#define PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H

#include "Pegasus/PropertyGrid/Property.h"
#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"

namespace Pegasus {
namespace PropertyGrid {


// Macros to declare properties in a class.

// In the header file, inside the class definition, declare properties as following (for a base class):
//     BEGIN_DECLARE_PROPERTIES_BASE(ClassName)
//         DECLARE_PROPERTY(int, NumLoops, 5)
//         DECLARE_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 0.0f, 0.0f))
//             etc.
//     END_DECLARE_PROPERTIES()

// In the header file, inside the class definition, declare properties as following (for a derived class):
//     BEGIN_DECLARE_PROPERTIES(ClassName, ParentClassName)
//         DECLARE_PROPERTY(int, NumLoops, 5)
//         DECLARE_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 0.0f, 0.0f))
//             etc.
//     END_DECLARE_PROPERTIES()

// In the implementation file, after the namespace declaration:
//     BEGIN_IMPLEMENT_PROPERTIES(ClassName)
//         IMPLEMENT_PROPERTY(ClassName, NumLoops)
//         IMPLEMENT_PROPERTY(ClassName, Position)
//             etc.
//     END_IMPLEMENT_PROPERTIES()

// In the implementation file, in the constructor of the class:
//     INIT_PROPERTY(NumLoops)
//     INIT_PROPERTY(Position)
//             etc.


//! Macro to start declaring a set of properties (when the class is a base class)
//! \param className Name of the class the properties belong to
//! \note To be used inside a class declaration, before calling \a DECLARE_PROPERTY()
#define BEGIN_DECLARE_PROPERTIES_BASE(className)                                                                    \
    DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(className, "")                                                      \
    //private:                                                                                                        \
    //    static const char * sClassName;                                                                             \


//! Macro to start declaring a set of properties (when the class is a derived class)
//! \param className Name of the class the properties belong to
//! \param parentClassName Name of the parent of the class the properties belong to
//! \note To be used inside a class declaration, before calling \a DECLARE_PROPERTY()
#define BEGIN_DECLARE_PROPERTIES2(className, parentClassName)                                                   \
    DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(className, #parentClassName)                                        \
    //private:                                                                                                        \
    //    static const char * sClassName;                                                                             \

    
//! Macro to declare a property
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \param defaultValue Default value of the property, set when an object with the property is 
//! \warning To be used after \a BEGIN_DECLARE_PROPERTIES() and before END_DECLARE_PROPERTIES()
#define DECLARE_PROPERTY2(type, name, defaultValue)                                                                 \
    DECLARE_PROPERTY_MEMBER_AND_ACCESSORS(type, name)                                                               \
    DECLARE_PROPERTY_DEFAULT_VALUE_ACCESSORS(type, name)                                                            \
    DECLARE_PROPERTY_DECLARATION_HELPER(type, name, defaultValue)                                                   \


//! Macro to stop declaring a set of properties
//! \note To be used inside a class declaration, after calling \a DECLARE_PROPERTY()
#define END_DECLARE_PROPERTIES2()                                                                                   \
    DECLARE_PROPERTIES_END_DECLARATION_HELPER()                                                                     \


//! Macro to start declaring a set of properties in the implementation file
//! \param className Class name of the property grid owner
//! \note To be used inside a .cpp file, before calling \a IMPLEMENT_PROPERTY()
#define BEGIN_IMPLEMENT_PROPERTIES2(className)                                                                      \
    className::BeginPropertyDeclarationHelper className::sBeginPropertyDeclarationHelper;                           \

    //const char * className::sClassName = #className;                                \
    //void className::InitProperties()                                                \
    //{                                                                               \

//! Macro to declare a property in the implementation file, after the namespace declaration
//! \param className Name of the class the properties belong to
//! \param name Name of the property, starting with an uppercase letter
#define IMPLEMENT_PROPERTY2(className, name)                                                                        \
    className::PropertyDeclarationHelper##name className::sPropertyDeclarationHelper##name;                         \


//! Macro to stop declaring a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a .cpp file, after calling \a IMPLEMENT_PROPERTY()
#define END_IMPLEMENT_PROPERTIES2(className)                                                                        \
    className::EndPropertyDeclarationHelper className::sEndPropertyDeclarationHelper;                               \


//! Macro to initialize a property in the implementation file, in the constructor of the class
//! \param name Name of the property, starting with an uppercase letter
#define INIT_PROPERTY2(name)                                                                                        \
    Set##name##ToDefault();                                                                                         \

//----------------------------------------------------------------------------------------

// Private macros


//! Shortcut for the property grid namespace, to make the following macros more readable
#define PPG Pegasus::PropertyGrid


//! Declare a private helper class, whose constructor allows starting declaring properties
//! \param className Name of the class the properties belong to
//! \param parentClassNameString Name of the parent of the class the properties belong to,
//!                              empty string for a base class
#define DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(className, parentClassNameString)                               \
    private:                                                                                                        \
        class BeginPropertyDeclarationHelper : public Pegasus::Utils::DependsOnStatic<                              \
                                                                    PPG::PropertyGridManager,                       \
                                                                    PPG::PropertyGridManager::GetInstance>          \
        {                                                                                                           \
            public:                                                                                                 \
                BeginPropertyDeclarationHelper::BeginPropertyDeclarationHelper()                                    \
                {                                                                                                   \
                    PPG::PropertyGridManager::GetInstance().BeginDeclareProperties(#className,                      \
                                                                                   parentClassNameString);          \
                }                                                                                                   \
        };                                                                                                          \
        static BeginPropertyDeclarationHelper sBeginPropertyDeclarationHelper;                                      \


//! Declare the property itself and the corresponding getter (Get<PropertyName>()) and setter (Set<PropertyName>(value))
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \todo Handle the invalidation of the property when the setter is called
#define DECLARE_PROPERTY_MEMBER_AND_ACCESSORS(type, name)                                                           \
    public:                                                                                                         \
        inline PPG::PropertyDefinition<type>::ReturnType Get##name() const                                          \
            { return mProperty##name; }                                                                             \
        inline void Set##name(PPG::PropertyDefinition<type>::ParamType value)                                       \
            { PPG::PropertyDefinition<type>::CopyProperty(mProperty##name, value);                                  \
              /*GetPropertyGrid().Invalidate();*/ }                                                                 \
    private:                                                                                                        \
        PPG::PropertyDefinition<type>::VarType mProperty##name;                                                     \


//! Declare the property default value getter (Get<PropertyName>DefaultValue()) and setter (Set<PropertyName>ToDefault())
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
#define DECLARE_PROPERTY_DEFAULT_VALUE_ACCESSORS(type, name)                                                        \
    public:                                                                                                         \
        inline PPG::PropertyDefinition<type>::ReturnType Get##name##DefaultValue() const                            \
            { return sPropertyDeclarationHelper##name.GetDefaultValue(); }                                          \
        inline void Set##name##ToDefault()                                                                          \
            { Set##name(Get##name##DefaultValue()); }                                                               \


//! Declare a private helper class, whose constructor allows the initialization of the default value
//! and the declaration of the property for the property grid manager
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \param defaultValue Default value of the property, set when an object with the property is 
#define DECLARE_PROPERTY_DECLARATION_HELPER(type, name, defaultValue)                                               \
    private:                                                                                                        \
        class PropertyDeclarationHelper##name : public Pegasus::Utils::DependsOnStatic<                             \
                                                                    PPG::PropertyGridManager,                       \
                                                                    PPG::PropertyGridManager::GetInstance>          \
        {                                                                                                           \
            public:                                                                                                 \
                PropertyDeclarationHelper##name::PropertyDeclarationHelper##name()                                  \
                {                                                                                                   \
                    PPG::PropertyDefinition<type>::CopyProperty(mDefaultValue, defaultValue);                       \
                    PPG::PropertyGridManager::GetInstance().DeclareProperty(                                        \
                        PPG::PropertyDefinition<type>::PROPERTY_TYPE,                                               \
                        sizeof(type), #name, &mDefaultValue);                                                       \
                }                                                                                                   \
                inline PPG::PropertyDefinition<type>::ReturnType GetDefaultValue() const                            \
                    { return mDefaultValue; }                                                                       \
            private:                                                                                                \
                PPG::PropertyDefinition<type>::VarType mDefaultValue;                                               \
        };                                                                                                          \
        static PropertyDeclarationHelper##name sPropertyDeclarationHelper##name;                                    \


//! Declare a private helper class, whose constructor allows ending declaring properties
#define DECLARE_PROPERTIES_END_DECLARATION_HELPER()                                                                 \
    private:                                                                                                        \
        class EndPropertyDeclarationHelper : public Pegasus::Utils::DependsOnStatic<                                \
                                                                    PPG::PropertyGridManager,                       \
                                                                    PPG::PropertyGridManager::GetInstance>          \
        {                                                                                                           \
            public:                                                                                                 \
                EndPropertyDeclarationHelper::EndPropertyDeclarationHelper()                                        \
                {                                                                                                   \
                    PPG::PropertyGridManager::GetInstance().EndDeclareProperties();                                 \
                }                                                                                                   \
        };                                                                                                          \
        static EndPropertyDeclarationHelper sEndPropertyDeclarationHelper;                                          \

//----------------------------------------------------------------------------------------

//! Property grid object, parent of every class that defines a set of editable properties.
//! When properties are declared, their association accessors are created (SetName() and GetName()).
//! The property grid object stores the property definitions and gives access to string-based accessors.
class PropertyGridObject
{
public:

    //! Constructor
    PropertyGridObject();

    //! \todo CHECK THAT THE NEXT TODO IS APPLICABLE

    //! \todo IMPORTANT Implement copy constructor and assignment operator.
    //!       When a Camera is copied for example, the property grid is copied, but not the members.
    //!       They should be.

    //! Register a property
    //! \param T Type of the property, use Math:: in front of types from the math library
    //! \param name Name of the property, starting with an uppercase letter
    //! \param varPtr Pointer to the property variable
    //! \param className Class name of the property grid owner
    //! \note Called by \a IMPLEMENT_PROPERTY()
    //template <typename T>
    //void RegisterProperty(const char * name, T * varPtr, const char * className);

    //! Invalidate the property grid, to be called after a member is updated,
    //! to tell the property grid owner to regenerate its data
    //inline void Invalidate() { mDirty = true; }

    //! Test if the property grid is dirty, meaning that at least one member has changed
    //! \return True if the dirty flag is set
    //inline bool IsDirty() const { return mDirty; }

    //! Validate the property grid, to be called by the property grid owner
    //! to tell its data has been regenerated with the current member data
    //inline void Validate() { mDirty = false; }

    //------------------------------------------------------------------------------------

    // Accessors for the proxy interface

//#if PEGASUS_ENABLE_PROXIES

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
    //void SetProperty(const char * name, void * valuePtr);

    //! Get the value of a property by name
    //! \param name Name of the property, non-empty string
    //! \return Pointer to the value, nullptr in case of error
    //const void * GetProperty(const char * name) const;

//#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
//private:

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
    //bool mDirty;

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

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H
