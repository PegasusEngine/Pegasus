/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppBsReflectionInfo.h
//! \author Kleber Garcia
//! \date   July 26th, 2015
//! \brief  This class registers libraries that are exposed to the editor for documentation purposes.
//!         Use this api to see the available functions in blockscript

#ifndef PEGASUS_APP_BS_REFLECTION_INFO_H
#define PEGASUS_APP_BS_REFLECTION_INFO_H

#include "Pegasus/Application/Shared/IAppBsReflectionInfo.h"
#include "Pegasus/Utils/Vector.h"

//fwd declarations
namespace Pegasus
{
    namespace Alloc {
        class IAllocator;
    }

    namespace BlockScript {
        class BlockLib;
        class TypeDesc;
        class FunDesc;
    }
}

namespace Pegasus
{
namespace App
{

//! Reflection data for a type.
class BsTypeInfo : public IBsTypeInfo
{
public:

    //! Constructor
    BsTypeInfo(Alloc::IAllocator* alloc, const BlockScript::TypeDesc* typeDesc); 
    
    //! Destructor
    virtual ~BsTypeInfo();

    //! Returns the name of this type.
    //! \return the name of this type
    virtual const char* GetName() const;

    //! Returns the modifier name of this type
    //! \return modifier name. Examples are "Structs", "Enums" etc
    virtual const char* GetModifierName() const;

    //! Returns the number of children this type has. Structs have children generally
    //! \return the number of children holding this type info
    virtual int GetChildrenCount() const;

    //! Returns a pointer to the children array 
    virtual const NameTypePair* GetChildren() const;

    //! Gets the properties count
    virtual int GetPropertiesCount() const;

    //! Gets the enums count
    virtual int GetEnumsCount() const;

    //! Gets the properties of this type if its an Object, null otherwise
    virtual const NameTypePair*       GetProperties() const;

    //! Gets the enums values of this type, if its an enum modifier
    virtual const char*const*   GetEnums() const;

private:
    Alloc::IAllocator* mAlloc;
    const BlockScript::TypeDesc* mTypeDesc;
    Utils::Vector<NameTypePair> mChildren;
    Utils::Vector<NameTypePair> mProps;
    Utils::Vector<const char*>  mEnums;
};

//! Class representing function information
class BsFunInfo : public IBsFunInfo
{
public:

    //! Constructor
    BsFunInfo(Alloc::IAllocator* alloc, const BlockScript::FunDesc* funDesc);

    //! Destructor
    virtual BsFunInfo::~BsFunInfo();

    //! Returns the name of this function
    //! \return the name of this function
    virtual const char* GetName() const;

    //! Returns the return type of this function
    //! \return the return type of this function
    virtual const IBsTypeInfo* GetReturnType() const;

    //! \return the number of function arguments this function has
    virtual int GetArgCount() const;

    //! \return the array of argument structs
    virtual const NameTypePair* GetArgs() const;

private:
    Alloc::IAllocator* mAlloc;
    const BlockScript::FunDesc* mFunDesc;
    Utils::Vector<NameTypePair> mArgs;
    BsTypeInfo mReturnType;
    
};

//! Class representing a blockscript library information.
//! Use this in the editor for reflection / documentation purposes
class AppBlockscriptLibInfo : public IAppBlockscriptLibInfo
{

public:
    
    //! Constructor
    AppBlockscriptLibInfo(Alloc::IAllocator* allocator, const BlockScript::BlockLib* lib);

    //!Destructor
    virtual ~AppBlockscriptLibInfo();

    //! \return the name of this lib
    virtual const char* GetName() const;

    //! \return the number of types
    virtual int GetTypeCounts() const;

    //! \return an array of type pointers this library holds
    virtual IBsTypeInfo* const * GetTypes() const;

    //! \return the count of functions this library holds
    virtual int GetFunctionCounts() const;

    //! \return an array of blockscript information containing functions
    virtual IBsFunInfo*const* GetFuns() const;

private:
    
    Alloc::IAllocator* mAlloc;
    const BlockScript::BlockLib* mLib;
    Utils::Vector<IBsTypeInfo*> mTypes;
    Utils::Vector<IBsFunInfo*> mFuns;

};

//! Class representing blockscript reflection information.
class AppBsReflectionInfo : public IAppBsReflectionInfo
{
public:

    //! Constructor
    AppBsReflectionInfo(Alloc::IAllocator* allocator);

    //! Destructor
    virtual ~AppBsReflectionInfo();

    //! \return the count of libraries
    virtual int GetLibraryCounts() const;

    //! \return an array of library pointers
    virtual IAppBlockscriptLibInfo*const* GetLibs() const;

    //! Register this blockscript reflection
    void RegisterLib(const BlockScript::BlockLib* lib);

private:
    Alloc::IAllocator* mAlloc;
    Utils::Vector<IAppBlockscriptLibInfo*> mLibs;
};

}
}

#endif
