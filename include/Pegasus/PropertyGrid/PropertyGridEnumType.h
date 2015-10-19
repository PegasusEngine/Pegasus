/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PropertyEnumType.h
//! \author Kleber Garcia
//! \date   October 5, 2015
//! \brief  Definition of a property grid enumeration type. Contains metadata necessary.

#ifndef PEGASUS_PROPERTYGRID_PROP_GRID_enumTypeName
#define PEGASUS_PROPERTYGRID_PROP_GRID_enumTypeName

#include "Pegasus/Utils/DependsOnStatic.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/PropertyGrid/PropertyGridStaticAllocator.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/PropertyGrid/Proxy/PropertyGridEnumTypeInfo.h"
    
#endif


//! Macro to start declaring an enumeration. Pass the enumeration type.
//! \param enumTypeName the name of the enumeration
//! \note To be used in a global scope, no namespace support.
#define BEGIN_DECLARE_ENUM(enumTypeName)\
    class enumTypeName : public ::Pegasus::PropertyGrid::BaseEnumType, public ::Pegasus::Utils::DependsOnStatic<::Pegasus::PropertyGrid::PropertyGridManager, ::Pegasus::PropertyGrid::PropertyGridManager::GetInstance>\
    {\
    public:\
        enumTypeName() {}\
        enumTypeName(const char* valueName) : ::Pegasus::PropertyGrid::BaseEnumType(valueName) { mValue = ++::Pegasus::PropertyGrid::EnumBuilder::sCurrVal; }\
        enumTypeName(const enumTypeName& other) : ::Pegasus::PropertyGrid::BaseEnumType(other.GetName(), other.GetValue()) {}\
        static const char* GetTypeName() { return #enumTypeName; }\
    };\

//! Macro to declare a single enumeration element. This will be a +1 from the previous enum value.
//! \param enumTypeName the type of this enumeration. 
//! \param enumValueName the value name of the enumeration to use.
//! \note To be used in a global scope, no namespace support.
#define DECLARE_ENUM(enumTypeName, enumValueName)\
    extern const enumTypeName enumValueName;

//! Macro to end enumaration declaration
#define END_DECLARE_ENUM()\

//! Macro to start implementation of enumeration. 
//! \param enumTypeName the name of the enumeration
//! \note To be used in a global scope, no namespace support.
//! ensure this enumeration implementation is located in a cpp file.
#define BEGIN_IMPLEMENT_ENUM(enumTypeName)\
    class Begin_##enumTypeName##_Register {\
    public:\
        Begin_##enumTypeName##_Register() {\
            ::Pegasus::PropertyGrid::EnumBuilder::sCurrVal = 0;\
            ::Pegasus::PropertyGrid::PropertyGridManager::GetInstance().BeginDeclareEnum(#enumTypeName);\
        }\
    };\
    Begin_##enumTypeName##_Register sBegin_##enumTypeName##_Register;
    
//! Macro to implement an enumeration value.
//! \param enumTypeName the type of this enumeration. 
//! \param enumValueName the value name of the enumeration to use.
//! \note To be used in a global scope, no namespace support. Use it a cpp file.
#define IMPLEMENT_ENUM(enumTypeName, enumValName)\
    class RegisterEnum_##enumTypeName##_##enumValName {\
    public:\
        RegisterEnum_##enumTypeName##_##enumValName() {\
            ::Pegasus::PropertyGrid::PropertyGridManager::GetInstance().RegisterEnum(&##enumValName);\
        }\
    };\
    const enumTypeName enumValName##(#enumValName);\
    RegisterEnum_##enumTypeName##_##enumValName sRegisterEnum_##enumTypeName##_##enumValName;

//! Macro to end enumaration declaration
#define END_IMPLEMENT_ENUM()
        
namespace Pegasus
{
    namespace PropertyGrid {

        class BaseEnumType;

        //! Internal class use to assign the current value of an enumeration.
        class EnumBuilder {
        public:
            static int sCurrVal;
        };

        //! Basic enumeration type.

                
        //! Type information of this enumeration
        class EnumTypeInfo {
        public:
            //! Constructor.
            EnumTypeInfo(); 

            //Destructor
            ~EnumTypeInfo();

            //! Sets the name of this enumeration
            void SetName(const char* name) { mName = name; }

            //! Returns the name of this enumeration type.
            const char* GetName() const { return mName; }

            //! Gets a list of all the enumerations values of this enumeration.
            Utils::Vector<const BaseEnumType*>& GetEnumerations() { return mEnumValues; }

            //! Gets a const list of all the enumeration values of this enumeration.
            const Utils::Vector<const BaseEnumType*>& GetEnumerations() const { return mEnumValues; }

#if PEGASUS_ENABLE_PROXIES
            //! Returns the proxy of this enum type info
            const IEnumTypeInfoProxy* GetProxy() const { return &mProxy; }
#endif
    
        private:
            //! List of enumeration values.
            Utils::Vector<const BaseEnumType*> mEnumValues;

            //! Name of this enumeration
            const char* mName;

#if PEGASUS_ENABLE_PROXIES
            //! Proxy
            EnumTypeInfoProxy mProxy;
#endif
        };
    }
}

#endif
