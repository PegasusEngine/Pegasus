/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridObject.h
//! \author	Karolyn Boulanger
//! \date	10th February 2015
//! \brief	Property grid object, parent of every class that defines a set of editable properties

#ifndef PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H
#define PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H

#include "Pegasus/PropertyGrid/Property.h"
#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/PropertyGrid/Proxy/PropertyGridObjectProxy.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Utils/Vector.h"

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
//     END_IMPLEMENT_PROPERTIES(ClassName)

// In the implementation file, in the constructor of the class:
//     BEGIN_INIT_PROPERTIES(ClassName)
//         INIT_PROPERTY(NumLoops)
//         INIT_PROPERTY(Position)
//             etc.
//     END_INIT_PROPERTIES()


//! Shortcut for the property grid namespace, to make the following macros more readable
#define PPG Pegasus::PropertyGrid

    
//! Macro to start declaring a set of properties (when the class is a base class)
//! \param className Name of the class the properties belong to
//! \note To be used inside a class declaration, before calling \a DECLARE_PROPERTY()
#define BEGIN_DECLARE_PROPERTIES_BASE(className)                                                                    \
    DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(className, "PropertyGridObject")                                    \
    DECLARE_PROPERTIES_CLASS_INFO(className)                                                                        \


//! Macro to start declaring a set of properties (when the class is a derived class)
//! \param className Name of the class the properties belong to
//! \param parentClassName Name of the parent of the class the properties belong to
//! \note To be used inside a class declaration, before calling \a DECLARE_PROPERTY()
#define BEGIN_DECLARE_PROPERTIES2(className, parentClassName)                                                       \
    DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(className, #parentClassName)                                        \
    DECLARE_PROPERTIES_CLASS_INFO(className)                                                                        \

    
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
    const PPG::PropertyGridClassInfo * const className::sClassInfo                                                  \
        = PPG::PropertyGridManager::GetInstance().GetClassInfo(#className);                                         \


//! Macro to declare a property in the implementation file, after the namespace declaration
//! \param className Name of the class the properties belong to
//! \param name Name of the property, starting with an uppercase letter
//! \warning To be used after \a BEGIN_IMPLEMENT_PROPERTIES() and before END_IMPLEMENT_PROPERTIES()
#define IMPLEMENT_PROPERTY2(className, name)                                                                        \
    className::PropertyDeclarationHelper##name className::sPropertyDeclarationHelper##name;                         \


//! Macro to stop declaring a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a .cpp file, after calling \a IMPLEMENT_PROPERTY()
#define END_IMPLEMENT_PROPERTIES2(className)                                                                        \
    className::EndPropertyDeclarationHelper className::sEndPropertyDeclarationHelper;                               \


//! Macro to start initializing a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a class' constructor, before calling \a INIT_PROPERTY()
#define BEGIN_INIT_PROPERTIES(className)                                                                            \
    // TODO


//! Macro to initialize a property in the implementation file, in the constructor of the class
//! \param name Name of the property, starting with an uppercase letter
#define INIT_PROPERTY2(name)                                                                                        \
    Set##name##ToDefault();                                                                                         \
    APPEND_PROPERTY_POINTER(mProperty##name);                                                   \


//! Macro to start initializing a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a class' constructor, before calling \a INIT_PROPERTY()
#define END_INIT_PROPERTIES()                                                                                       \
    PG_ASSERTSTR(GetNumPropertyPointers() == GetStaticClassInfo()->GetNumProperties(),                              \
                 "Invalid declaration of properties for class %s with parent %s: GetNumPropertyPointers()==%u != GetNumProperties()==%u", GetStaticClassInfo()->GetClassName(), GetStaticClassInfo()->GetParentClassName(), GetNumPropertyPointers(), GetStaticClassInfo()->GetNumProperties());    \

//----------------------------------------------------------------------------------------

// Private macros


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


//! Declare the GetClassInfo() and GetStaticClassInfo() functions for the current class
//! and their corresponding static member. The first function returns the class info corresponding
//! to the factory class, the second function returns the class info depending on the class of the current pointer
//! \param className Name of the current class
#define DECLARE_PROPERTIES_CLASS_INFO(className)                                                                    \
    public:                                                                                                         \
        virtual const PPG::PropertyGridClassInfo * GetClassInfo() const { return className::sClassInfo; }           \
        static const PPG::PropertyGridClassInfo * GetStaticClassInfo() { return className::sClassInfo; }            \
    private:                                                                                                        \
        static const PPG::PropertyGridClassInfo * const sClassInfo;                                                 \


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
              InvalidatePropertyGrid(); }                                                                           \
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
                        sizeof(type), #name, PPG::PropertyDefinition<type>::GetName(), &mDefaultValue);                                                       \
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


//! Add a property pointer to the object property pointer array
//! \param p Property for which the pointer is desired
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
#define APPEND_PROPERTY_POINTER(p) AppendPropertyPointer(static_cast<void *>(&p), sizeof(p))
#else
#define APPEND_PROPERTY_POINTER(p) AppendPropertyPointer(static_cast<void *>(&p))
#endif

//----------------------------------------------------------------------------------------

class PropertyGridObject;

//! Class used to read and write the content of a property.
//! This wrapper prevents bad casts to non-const pointers that would not invalidate the property grid properly,
//! and it sets the dirty flag of the PropertyGridObject's property grid each time the value is set through Set<>()
class PropertyAccessor
{
public:

    //! Default constructor
    //! \note Defined only to create arrays of accessors. However the initial state is invalid,
    //!       the accessors must be assigned to be valid
    PropertyAccessor()
        :   mObj(nullptr)
        ,   mPtr(nullptr)
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
        ,   mSize(0)
#endif
    { }

    //! Read-only accessor to the property
    //! \return Value or const reference to the property, depending on the type
    template <typename T>
    inline typename PPG::PropertyDefinition<T>::ReturnType Get() const
        {
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
            PG_ASSERTSTR(sizeof(T) == mSize, "Wrong template type when getting the value of a property.");
#endif
            return *static_cast<const T *>(mPtr);
        }

    //! Setter of the property
    //! \note Sets the dirty flag of the PropertyGridObject's property grid
    //! \param value New value of the property
    template <typename T>
    inline void Set(typename PPG::PropertyDefinition<T>::ParamType value) const
        {
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
            PG_ASSERTSTR(sizeof(T) == mSize, "Wrong template type when setting the value of a property.");
#endif
            *static_cast<T *>(mPtr) = value;
            InvalidatePropertyGrid();
        }

    //! Read the property into a buffer
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    void Read(void * outputBuffer, unsigned int outputBufferSize) const;

    //! Write the property using the content of a buffer
    //! \param inputBuffer Input buffer with content to copy to the property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    void Write(const void * inputBuffer, unsigned int inputBufferSize) const;

    //------------------------------------------------------------------------------------

private:

    // Make PropertyGridObject a friend to access the private constructor
    // (to prevent external users from modifying the internal pointer)
    friend class PropertyGridObject;

    //! Constructor
    //! \param obj Pointer to the property grid object owning the property (!= nullptr)
    //! \param ptr Pointer to the property (!= nullptr)
    //! \param size Size of the object pointed by the pointer in bytes (> 0)
    //!             (used for size checking in the accessors when PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR == 1)
    inline PropertyAccessor(
          PropertyGridObject * obj
        , void * ptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
        , unsigned int size
#endif
                           )
        :   mObj(obj)
        ,   mPtr(ptr)
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
        ,   mSize(size)
#endif
        { }

    //! Invalidate the property grid of the attached PropertyGridObject
    //! \note Has to not be inline, because PropertyGridObject is not declared yet.
    //!       We cannot move this class' declaration after PropertyGridObject
    //!       since the latter has functions returning PropertyAccessor by value
    inline void InvalidatePropertyGrid() const;

    //! Non-null pointer to the property grid object owning the property
    //! (nullptr only if using the default constructor)
    PropertyGridObject * mObj;

    //! Pointer to the property, nullptr if using the default constructor or in case of error
    void * mPtr;

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    //! Size of the object pointed by the pointer in bytes (> 0)
    //! (used for size checking in the accessors when PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR == 1)
    unsigned int mSize;
#endif
};

//----------------------------------------------------------------------------------------

//! Property grid object, parent of every class that defines a set of editable properties.
//! When properties are declared, their association accessors are created (SetName() and GetName()).
//! The property grid object stores the property definitions and gives access to string-based accessors.
class PropertyGridObject
{
    // Declare the base property object class, however the BEGIN_DECLARE_PROPERTIES(_BASE) macros
    // cannot be used since there is no parent defined
    DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(PropertyGridObject, "")
    DECLARE_PROPERTIES_CLASS_INFO(PropertyGridObject)

        //! \todo Make that property for editor/data only and not for REL
        DECLARE_PROPERTY2(String64, Name, "");

    END_DECLARE_PROPERTIES2()

    //------------------------------------------------------------------------------------

public:

    //! Constructor
    PropertyGridObject();

    //! \todo CHECK THAT THE NEXT TODO IS APPLICABLE

    //! \todo IMPORTANT Implement copy constructor and assignment operator.
    //!       When a Camera is copied for example, the property grid is copied, but not the members.
    //!       They should be.

    //! Get the number of registered properties for the current class only
    //! \return Number of successfully registered properties for the current class only
    inline unsigned int GetNumClassProperties() const
        { return GetClassInfo()->GetNumClassProperties(); }

    //! Get the record of a property for the current class only
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \return Record of the property (information about the property)
    inline const PropertyRecord & GetClassPropertyRecord(unsigned int index) const
        { return GetClassInfo()->GetClassProperty(index); }

    //! Get an accessor to a property for the current class only
    //! \param index Index of the property (0 <= index < GetNumClassProperties())
    //! \return Accessor for the property
    PropertyAccessor GetClassPropertyAccessor(unsigned int index);

    //! Get the number of registered properties, including parent classes (but not derived classes)
    //! \return Number of successfully registered properties
    inline unsigned int GetNumProperties() const
        { return GetClassInfo()->GetNumProperties(); }

    //! Get the record of a property, including parent classes (but not derived classes)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Record of the property (information about the property)
    const PropertyRecord & GetPropertyRecord(unsigned int index) const
        { return GetClassInfo()->GetProperty(index); }

    //! Get an accessor to a property, including parent classes (but not derived classes)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Accessor for the property
    PropertyAccessor GetPropertyAccessor(unsigned int index);

    //------------------------------------------------------------------------------------
    
    //! Invalidate the property grid (sets the dirty flag)
    //! \note Called automatically by setters, but can be used to force the dirty flag manually
    inline void InvalidatePropertyGrid() { mPropertyGridDirty = true; }

    //! Test if the property grid is dirty, meaning that at least one member has changed
    //! \return True if the dirty flag is set
    inline bool IsPropertyGridDirty() const { return mPropertyGridDirty; }

    //! Validate the property grid, to be called by the property grid owner
    //! to tell its data has been regenerated with the current member data
    //! \warning Make sure data of the owner has been regenerated before setting that flag
    //! \warning Make sure that function is called regularly, nothing else will reset the flag otherwise
    inline void ValidatePropertyGrid() { mPropertyGridDirty = false; }

    //------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the property grid object
    //! \return Proxy associated with the property grid object
    //@{
    inline PropertyGridObjectProxy * GetProxy() { return &mProxy; }
    inline const PropertyGridObjectProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
protected:

    //! Add a property pointer to the object property pointer array
    //! \param ptr Pointer to the property to add
    //! \param propertySize Size of the property pointed by the pointer in bytes (> 0)
    //! \warning To be used only by the INIT_PROPERTY() macro
    void AppendPropertyPointer(  void * ptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                               , unsigned int propertySize
#endif
                              );

    //! Get the number of property pointers that have been added to the array
    //! \return Number of property pointers that have been added to the array
    //! \note This should match GetNumProperties(), otherwise that means that the number
    //!       of declaration/implementation/initialization macros does not match
    inline unsigned int GetNumPropertyPointers() const { return mPropertyPointers.GetSize(); }

    //------------------------------------------------------------------------------------
    
private:

    //! List of pointers to the members, from the base class down to the class
    //! the object is instantiated as
    Utils::Vector<void *> mPropertyPointers;

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    //! Size of the properties in bytes (> 0), used for sanity checks in PropertyAccessor
    Utils::Vector<unsigned int> mPropertySizes;
#endif

    //! Set to true after a property is updated,
    //! to tell the property grid object owner to regenerate its data
    bool mPropertyGridDirty;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the property grid object
    PropertyGridObjectProxy mProxy;

#endif  // PEGASUS_ENABLE_PROXIES
};

//----------------------------------------------------------------------------------------

// Implementation

inline void PropertyAccessor::InvalidatePropertyGrid() const
{
    mObj->InvalidatePropertyGrid();
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H
