/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IAppBsReflectionInfo.h
//! \author Kleber Garcia
//! \date   July 26th, 2015
//! \brief  This class registers libraries that are exposed to the editor for documentation purposes.
//!         Use this api to see the available functions in blockscript

#ifndef PEGASUS_IAPP_BS_REFLECTION_INFO_H
#define PEGASUS_IAPP_BS_REFLECTION_INFO_H

namespace Pegasus
{
namespace App
{

class IBsTypeInfo;
    
//! Internal structure representign a function argument (passed in)
struct NameTypePair
{
    const char* mName;
    IBsTypeInfo* mType;
};

//! Reflection data for a type.
class IBsTypeInfo
{
public:

    //! Returns the name of this type.
    //! \return the name of this type
    virtual const char* GetName() const = 0;

    //! Returns the modifier name of this type
    //! \return modifier name. Examples are "Structs", "Enums" etc
    virtual const char* GetModifierName() const = 0;

    //! Returns the number of children this type has. Structs have children generally
    //! \return the number of children holding this type info
    virtual int GetChildrenCount() const = 0;

    //! Returns a pointer to the children array 
    virtual const NameTypePair* GetChildren() const = 0;

    //! Gets the properties count
    virtual int GetPropertiesCount() const = 0;

    //! Gets the enums count
    virtual int GetEnumsCount() const = 0;

    //! Gets the properties of this type if its an Object, null otherwise
    virtual const NameTypePair*       GetProperties() const = 0;

    //! Gets the enums values of this type, if its an enum modifier
    virtual const char*const*   GetEnums() const = 0;

};

//! Class representing function information
class IBsFunInfo
{
public:

    //! Returns the name of this function
    //! \return the name of this function
    virtual const char* GetName() const = 0;

    //! Returns the return type of this function
    //! \return the return type of this function
    virtual const IBsTypeInfo* GetReturnType() const = 0;

    //! \return the number of function arguments this function has
    virtual int GetArgCount() const = 0;

    //! \return the array of argument structs
    virtual const NameTypePair* GetArgs() const = 0;
    
};


//! Class representing a blockscript library information.
//! Use this in the editor for reflection / documentation purposes
class IAppBlockscriptLibInfo
{

public:

    //! \return the name of this lib
    virtual const char* GetName() const = 0;

    //! \return the number of types
    virtual int GetTypeCounts() const = 0;

    //! \return an array of type pointers this library holds
    virtual IBsTypeInfo*const* GetTypes() const = 0;

    //! \return the count of functions this library holds
    virtual int GetFunctionCounts() const = 0;

    //! \return an array of blockscript information containing functions
    virtual IBsFunInfo*const* GetFuns() const = 0;

};

//! Class representing blockscript reflection information.
class IAppBsReflectionInfo
{
public:
    //! \return the count of libraries
    virtual int GetLibraryCounts() const = 0;

    //! \return an array of library pointers
    virtual IAppBlockscriptLibInfo*const* GetLibs() const = 0;
};

}
}

#endif 
