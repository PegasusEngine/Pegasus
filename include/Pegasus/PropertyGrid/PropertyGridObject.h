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
#include "Pegasus/PropertyGrid/Shared/PropertyEventDefs.h"

//forward declarations
namespace Pegasus {
namespace AssetLib {
   class Asset;
   class Object;
}
}

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
#define BEGIN_DECLARE_PROPERTIES(className, parentClassName)                                                       \
    DECLARE_PROPERTIES_BEGIN_DECLARATION_HELPER(className, #parentClassName)                                        \
    DECLARE_PROPERTIES_CLASS_INFO(className)                                                                        \

    
//! Macro to declare a property
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \param defaultValue Default value of the property, set when an object with the property is 
//! \warning To be used after \a BEGIN_DECLARE_PROPERTIES() and before END_DECLARE_PROPERTIES()
#define DECLARE_PROPERTY(type, name, defaultValue)                                                                 \
    DECLARE_PROPERTY_MEMBER_AND_ACCESSORS(type, name)                                                               \
    DECLARE_PROPERTY_DEFAULT_VALUE_ACCESSORS(type, name)                                                            \
    DECLARE_PROPERTY_DECLARATION_HELPER(type, name, defaultValue)                                                   \


//! Macro to stop declaring a set of properties
//! \note To be used inside a class declaration, after calling \a DECLARE_PROPERTY()
#define END_DECLARE_PROPERTIES()                                                                                   \
    DECLARE_PROPERTIES_END_DECLARATION_HELPER()                                                                     \


//! Macro to start declaring a set of properties in the implementation file
//! \param className Class name of the property grid owner
//! \note To be used inside a .cpp file, before calling \a IMPLEMENT_PROPERTY()
#define BEGIN_IMPLEMENT_PROPERTIES(className)                                                                      \
    className::BeginPropertyDeclarationHelper className::sBeginPropertyDeclarationHelper;                           \
    const PPG::PropertyGridClassInfo * const className::sClassInfo                                                  \
        = PPG::PropertyGridManager::GetInstance().GetClassInfo(#className);                                         \


//! Macro to declare a property in the implementation file, after the namespace declaration
//! \param className Name of the class the properties belong to
//! \param name Name of the property, starting with an uppercase letter
//! \warning To be used after \a BEGIN_IMPLEMENT_PROPERTIES() and before END_IMPLEMENT_PROPERTIES()
#define IMPLEMENT_PROPERTY(className, name)                                                                        \
    className::PropertyDeclarationHelper##name className::sPropertyDeclarationHelper##name;                         \


//! Macro to stop declaring a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a .cpp file, after calling \a IMPLEMENT_PROPERTY()
#define END_IMPLEMENT_PROPERTIES(className)                                                                        \
    className::EndPropertyDeclarationHelper className::sEndPropertyDeclarationHelper;                               \


//! Macro to start initializing a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a class' constructor, before calling \a INIT_PROPERTY()
#define BEGIN_INIT_PROPERTIES(className)                                                                            \
    // TODO


//! Macro to initialize a property in the implementation file, in the constructor of the class
//! \param name Name of the property, starting with an uppercase letter
#define INIT_PROPERTY(name)                                                                                        \
    Set##name##ToDefault();                                                                                        \
    mProperty##name##Index = APPEND_PROPERTY_POINTER(mProperty##name);                                             \


//! Macro to start initializing a set of properties in the implementation file
//! \param className Name of the class the properties belong to
//! \note To be used inside a class' constructor, before calling \a INIT_PROPERTY()
#define END_INIT_PROPERTIES()                                                                                       \
    PG_ASSERTSTR(GetNumClassPropertyPointers() == GetStaticClassInfo()->GetNumClassProperties(),                              \
                 "Invalid declaration of class properties for class %s with parent %s: GetNumClassPropertyPointers()==%u != GetNumClassProperties()==%u", GetStaticClassInfo()->GetClassName(), GetStaticClassInfo()->GetParentClassName(), GetNumClassPropertyPointers(), GetStaticClassInfo()->GetNumClassProperties());    \

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
              PEGASUS_EVENT_DISPATCH(((Pegasus::PropertyGrid::PropertyGridObject*)this), Pegasus::PropertyGrid::ValueChangedEventIndexed, Pegasus::PropertyGrid::PROPERTYCATEGORY_CLASS, mProperty##name##Index ); \
              InvalidatePropertyGrid(); }                                                                           \
    private:                                                                                                        \
        PPG::PropertyDefinition<type>::VarType mProperty##name;                                                     \
        int mProperty##name##Index;                                                                                 \


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
                        sizeof(type), #name, PPG::PropertyDefinition<type>::GetTypeName(), &mDefaultValue);         \
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


//! Add a class property pointer to the object's class property pointer array
//! \param p Class property for which the pointer is desired
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
#define APPEND_PROPERTY_POINTER(p) AppendClassPropertyPointer(static_cast<void *>(&p), sizeof(p))
#else
#define APPEND_PROPERTY_POINTER(p) AppendClassPropertyPointer(static_cast<void *>(&p))
#endif

//----------------------------------------------------------------------------------------

class PropertyGridObject;

//! Class used to read content of a property safely. Stores a const pointer to the class and 
//! memory owning the property to be read. Only provides API accessors for reading.
class PropertyReadAccessor
{
public:

    //! Default constructor
    //! \note Defined only to create arrays of accessors. However the initial state is invalid,
    //!       the accessors must be assigned to be valid
    PropertyReadAccessor()
        :   mConstObj(nullptr)
        ,   mConstPtr(nullptr)
#if PEGASUS_USE_EVENTS
        ,   mCategory(PROPERTYCATEGORY_INVALID)
        ,   mIndex(-1)
#endif
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
            return *static_cast<const T *>(mConstPtr);
        }


    //! Read the property into a buffer
    //! \param outputBuffer Buffer that is filled with the property data (!= nullptr)
    //! \param outputBufferSize Size of the output buffer in bytes (> 0)
    //! \warning The output buffer size must match the registered size of this accessor
    void Read(void * outputBuffer, unsigned int outputBufferSize) const;

    //------------------------------------------------------------------------------------

protected:

    // Make PropertyGridObject a friend to access the private constructor
    // (to prevent external users from modifying the internal pointer)
    friend class PropertyGridObject;

    //! Constructor
    //! \param obj Pointer to the property grid object owning the property (!= nullptr)
    //! \param ptr Pointer to the property (!= nullptr)
    //! \param size Size of the object pointed by the pointer in bytes (> 0)
    //!             (used for size checking in the accessors when PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR == 1)
    inline PropertyReadAccessor(
          const PropertyGridObject * obj
        , const void * ptr
#if PEGASUS_USE_EVENTS
        , PropertyCategory category
        , int index 
#endif

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
        , unsigned int size
#endif
                           )
        :   mConstObj(obj)
        ,   mConstPtr(ptr)
#if PEGASUS_USE_EVENTS
        ,   mCategory(category)
        ,   mIndex(index)
#endif

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
        ,   mSize(size)
#endif
        { }

    //! Non-null pointer to the property grid object owning the property
    //! (nullptr only if using the default constructor)
    const PropertyGridObject * mConstObj;

    //! Pointer to the const property, nullptr if using the default constructor or in case of error
    const void * mConstPtr;

#if PEGASUS_USE_EVENTS
    PropertyCategory mCategory;
    int mIndex;
#endif

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    //! Size of the object pointed by the pointer in bytes (> 0)
    //! (used for size checking in the accessors when PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR == 1)
    unsigned int mSize;
#endif
};

//! Class used to read and write the content of a property.
//! This wrapper prevents bad casts to non-const pointers that would not invalidate the property grid properly,
//! and it sets the dirty flag of the PropertyGridObject's property grid each time the value is set through Set<>()
//! or through the Write API call.
class PropertyAccessor : public PropertyReadAccessor
{
public:

    //! Default constructor
    //! \note Defined only to create arrays of accessors. However the initial state is invalid,
    //!       the accessors must be assigned to be valid
    PropertyAccessor()
        :   mObj(nullptr)
        ,   mPtr(nullptr)
    { }

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
            PEGASUS_EVENT_DISPATCH(mObj, ValueChangedEventIndexed, mCategory, mIndex);
    }

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
#if PEGASUS_USE_EVENTS
        , PropertyCategory category
        , int index 
#endif

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
        , unsigned int size
#endif
                           )
        :   PropertyReadAccessor(obj, ptr
#if PEGASUS_USE_EVENTS
            , category
            , index 
#endif
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
            , size
#endif
            )
        ,   mObj(obj)
        ,   mPtr(ptr)
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
        DECLARE_PROPERTY(String64, Name, "");

    END_DECLARE_PROPERTIES()

    //------------------------------------------------------------------------------------

public:
    PEGASUS_EVENT_DECLARE_DISPATCHER(IPropertyListener)

    //! Constructor
    PropertyGridObject();

    //! Destructor
    virtual ~PropertyGridObject();


    //! \todo CHECK THAT THE NEXT TODO IS APPLICABLE

    //! \todo IMPORTANT Implement copy constructor and assignment operator.
    //!       When a Camera is copied for example, the property grid is copied, but not the members.
    //!       They should be.


    // Class properties (defined when declaring the classes at compile time, identical for all instances)

    //! Get the number of registered properties for the current derived class only
    //! \return Number of successfully registered properties for the current derived class only
    inline unsigned int GetNumDerivedClassProperties() const
        { return GetClassInfo()->GetNumDerivedClassProperties(); }

    //! Get the record of a property for the current derived class only
    //! \param index Index of the property (0 <= index < GetNumDerivedClassProperties())
    //! \return Record of the property (information about the property)
    inline const PropertyRecord & GetDerivedClassPropertyRecord(unsigned int index) const
        { return GetClassInfo()->GetDerivedClassPropertyRecord(index); }

    //! Get an accessor to a property for the current derived class only
    //! \param index Index of the property (0 <= index < GetNumDerivedClassProperties())
    //! \return Accessor for the property
    PropertyAccessor GetDerivedClassPropertyAccessor(unsigned int index);

    //! Get the number of registered class properties, including parent classes (but not classes deriving from the current class)
    //! \return Number of successfully registered class properties
    inline unsigned int GetNumClassProperties() const
        { return GetClassInfo()->GetNumClassProperties(); }

    //! Get the record of a class property, including parent classes (but not classes deriving from the current class)
    //! \param index Index of the class property (0 <= index < GetNumClassProperties())
    //! \return Record of the class property (information about the class property)
    const PropertyRecord & GetClassPropertyRecord(unsigned int index) const
        { return GetClassInfo()->GetClassPropertyRecord(index); }

    //! Get an accessor to a class property, including parent classes (but not classes deriving from the current class)
    //! \param index Index of the class property (0 <= index < GetNumClassProperties())
    //! \return Accessor for the class property
    PropertyAccessor GetClassPropertyAccessor(unsigned int index);

    //! Get an accessor to a property, including parent classes (but not classes deriving from the current class)
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Accessor for the property
    const PropertyReadAccessor GetClassReadPropertyAccessor(unsigned int index) const;

    //------------------------------------------------------------------------------------

    // Object properties (defined at runtime, per instance)

    //! Add an object property to the instance
    //! \param type Type of the property, PROPERTYTYPE_xxx constant
    //! \param size Size in bytes of the property (> 0)
    //! \param name Name of the property, starting with an uppercase letter (non-empty),
    //!             (has to be a pointer to a global constant, does not store the string)
	//! \param typeName Name of the type of the property
    //! \param defaultValuePtr Pointer to the default value of the property
    void AddObjectProperty(PropertyType type, int typeSize, const char * name, const char * typeName, const void * defaultValuePtr);

    //! Add an object property to the instance using a known type
    //! \param name Name of the property, starting with an uppercase letter (non-empty),
    //!             (has to be a pointer to a global constant, does not store the string)
	//! \param defaultValuePtr Pointer to the default value of the property
    template <typename T>
    inline void AddObjectProperty(const char * name, const T * defaultValuePtr);

    //! Remove an object property from list
    //! \param index Index of the object property to remove
    void RemoveObjectProperty(unsigned int index);

    //! Clear the list of object properties
    void ClearObjectProperties();

    //! Get the number of object properties (can change any time and for any object)
    //! \return Number of available object properties
    inline unsigned int GetNumObjectProperties() const
        { return mObjectProperties.GetSize(); }

    //! Get the record of an object property
    //! \param index Index of the object property (0 <= index < GetNumObjectProperties())
    //! \return Record of the object property (information about the property)
    const PropertyRecord & GetObjectPropertyRecord(unsigned int index) const
        { return mObjectProperties[index].record; }

    //! Get an accessor to an object property
    //! \param index Index of the object property (0 <= index < GetNumObjectProperties())
    //! \return Accessor for the object property
    PropertyAccessor GetObjectPropertyAccessor(unsigned int index);

    //------------------------------------------------------------------------------------
    
    //! Invalidate the property grid (sets the dirty flag)
    //! \note Called automatically by setters, but can be used to force the dirty flag manually
    inline void InvalidatePropertyGrid() { mPropertyGridDirty = true; }

    //! Pegasus event function to invalidate the data.
    inline void InvalidateData() { InvalidatePropertyGrid(); }

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
    inline PropertyGridObjectProxy * GetPropertyGridProxy() { return &mProxy; }
    inline const PropertyGridObjectProxy * GetPropertyGridProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
protected:

    //! Add a class property pointer to the object's property pointer array
    //! \param ptr Pointer to the class property to add
    //! \param propertySize Size of the class property pointed by the pointer in bytes (> 0)
	//! \return the index of this property in the list
    //! \warning To be used only by the INIT_PROPERTY() macro
    int AppendClassPropertyPointer(  void * ptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                    , unsigned int propertySize
#endif
                                   );

    //! Get the number of class property pointers that have been added to the array
    //! \return Number of class property pointers that have been added to the array
    //! \note This should match GetNumClassProperties(), otherwise that means that the number
    //!       of declaration/implementation/initialization macros does not match
    inline unsigned int GetNumClassPropertyPointers() const { return mClassPropertyPointers.GetSize(); }

    
    //! Called when a node is dumped into an asset
    //! \param obj - the object to write the contents of this object to.
    virtual void WriteToObject(AssetLib::Asset* parentAsset, AssetLib::Object* obj) const;

    //! Called when a node is read from an object
    //! \parama obj - the object to read the contents from.
    virtual bool ReadFromObject(AssetLib::Asset* parentAsset, AssetLib::Object* obj);

    //------------------------------------------------------------------------------------
    
private:

    //! List of pointers to the class properties, from the base class down to the class
    //! the object is instantiated as
    Utils::Vector<void *> mClassPropertyPointers;

#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    //! Size of the class properties in bytes (> 0), used for sanity checks in PropertyAccessor
    Utils::Vector<unsigned int> mClassPropertySizes;
#endif


    //! Structure defining the properties and value of an object property
    typedef struct ObjectProperty
    {
        //! Specifications of the property, such as the type
        PropertyRecord record;

        //! Pointer to the variable containing the property value,
        //! dynamically allocated
        unsigned char * valuePtr;
    };

    //! List of object properties, unique to the instance
    Utils::Vector<ObjectProperty> mObjectProperties;


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

//----------------------------------------------------------------------------------------

template <typename T>
inline void PropertyGridObject::AddObjectProperty(const char * name, const T * defaultValuePtr)
{
    AddObjectProperty(PropertyDefinition<T>::PROPERTY_TYPE,
                      sizeof(T),
                      name,
                      PropertyDefinition<T>::GetTypeName(),
                      defaultValuePtr);
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRIDOBJECT_H
