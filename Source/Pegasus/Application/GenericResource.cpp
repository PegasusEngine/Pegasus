/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GenericResource.cpp
//! \author Kleber Garcia
//! \date   August 26th 2016
//! \brief  A generic resource is an extendable type, that will automatically:
//!         - setup a blockscript type to represent a custom resource (usually from a render system)
//!         - setup property editor for such types on blockscript

#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Application;
using namespace Pegasus::BlockScript;
using namespace Pegasus::PropertyGrid;


//TODO:  put this in a nice header file
extern 
bool PropertyGridPropertyCallback(const PropertyGrid::PropertyAccessor* accessor, const Pegasus::BlockScript::PropertyCallbackContext& context);

extern
void GlobalCache_PrototypeRegisterGenericResource(BlockScript::FunCallbackContext& context);

extern
void GlobalCache_PrototypeFindGenericResource(BlockScript::FunCallbackContext& context);


namespace Pegasus
{
namespace Application
{

//! Property implementations
BEGIN_IMPLEMENT_PROPERTIES(GenericResource)
END_IMPLEMENT_PROPERTIES(GenericResource)

GenericResource::GenericResource(Alloc::IAllocator* allocator)
: RefCounted(allocator)
{
    BEGIN_INIT_PROPERTIES(GenericResource)
    END_INIT_PROPERTIES()
}

void Update_Callback(FunCallbackContext& context)
{
    Application::RenderCollection* container = static_cast<Application::RenderCollection*>(context.GetVmState()->GetUserContext());
    FunParamStream stream(context);
    PG_ASSERT(container != nullptr);
    RenderCollection::CollectionHandle handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Application::GenericResource* theResource = RenderCollection::GetResource<Application::GenericResource>(container, handle);
        theResource->Update();
    }
}

bool Update_PropertyAccessCallback(const PropertyCallbackContext& context)
{
    Application::RenderCollection* container = static_cast<Application::RenderCollection*>(context.state->GetUserContext());
    PG_ASSERT(container != nullptr);
    if (context.objectHandle != RenderCollection::INVALID_HANDLE)
    {
        Application::GenericResource* theResource = RenderCollection::GetResource<Application::GenericResource>(container, context.objectHandle);
        PG_ASSERT(theResource != nullptr);
        PG_ASSERT((unsigned)(context.propertyDesc->mGuid) < theResource->GetNumClassProperties());
        PropertyGrid::PropertyAccessor accessor = theResource->GetClassPropertyAccessor(context.propertyDesc->mGuid);
        return PropertyGridPropertyCallback(&accessor, context);
    }
    return false;
}

void GenericResource::RegisterGenericResourceType(
    const PropertyGrid::PropertyGridClassInfo* propertyGridClass,
    BlockScript::BlockLib* lib,
    BlockScript::FunctionDeclarationDesc* extraMethods,
    unsigned int extraMethodsSize)
{
    BlockScript::ClassTypeDesc nodeDef;
    nodeDef.classTypeName = propertyGridClass->GetClassName();
    nodeDef.methodDescriptors[0].functionName = "Update";
    nodeDef.methodDescriptors[0].returnType = "int";
    nodeDef.methodDescriptors[0].argumentTypes[0] = nodeDef.classTypeName;
    nodeDef.methodDescriptors[0].argumentNames[0] = "this";
    nodeDef.methodDescriptors[0].argumentTypes[1] = nullptr;
    nodeDef.methodDescriptors[0].argumentNames[1] = nullptr;
    nodeDef.methodDescriptors[0].callback = Update_Callback;
    nodeDef.methodsCount = 1;

    PG_ASSERT(extraMethodsSize + 1 < MAX_OBJECT_METHOD_DESCRIPTORS);
    extraMethodsSize = extraMethodsSize + 1 < MAX_OBJECT_METHOD_DESCRIPTORS ? extraMethodsSize : MAX_OBJECT_METHOD_DESCRIPTORS - 1;
    for (unsigned int i = 0; i < extraMethodsSize; ++i)
    {
        nodeDef.methodDescriptors[nodeDef.methodsCount++] = extraMethods[i];
    }

    int nextPropertyId = 0;
    Utils::Vector<ObjectPropertyDesc> typeDescriptions;
    for (unsigned propertyId = 0; propertyId < propertyGridClass->GetNumClassProperties(); ++propertyId)
    {
        ObjectPropertyDesc& propDesc = typeDescriptions.PushEmpty(); 
        const PropertyRecord& record = propertyGridClass->GetClassPropertyRecord(propertyId);

        const char* recordTypeName = record.typeName;
        //patch the type name for certain types.
        if (record.type == PropertyGrid::PROPERTYTYPE_BOOL || record.type == PropertyGrid::PROPERTYTYPE_UINT)
        {
            recordTypeName = "int";
        }
        else if (record.type == PropertyGrid::PROPERTYTYPE_STRING64)
        {
            recordTypeName = "string";
        }

        propDesc.propertyTypeName = recordTypeName;
        propDesc.propertyName = record.name;
        propDesc.propertyUniqueId = nextPropertyId;
        ++nextPropertyId;
    }

    nodeDef.propertyDescriptors = typeDescriptions.Data();
    nodeDef.propertyCount = typeDescriptions.GetSize();
    nodeDef.getPropertyCallback = Update_PropertyAccessCallback;
    lib->CreateClassTypes(&nodeDef, 1);

    BlockScript::FunctionDeclarationDesc globalFunctions[2];
    globalFunctions[0].functionName = "RegisterGlobalResource";
    globalFunctions[0].returnType = "int";
    globalFunctions[0].argumentTypes[0] = "string";
    globalFunctions[0].argumentNames[0] = "Name";
    globalFunctions[0].argumentTypes[1] = propertyGridClass->GetClassName();
    globalFunctions[0].argumentNames[1] = "Obj";
    globalFunctions[0].argumentTypes[2] = nullptr;
    globalFunctions[0].argumentNames[2] = nullptr;
    globalFunctions[0].callback = GlobalCache_PrototypeRegisterGenericResource;

    char findFunctionName[256];
    const char* findFunPrefix = "FindGlobal";
    PG_ASSERT(Utils::Strlen(findFunPrefix) + Utils::Strlen(propertyGridClass->GetClassName()) + 1 < 256);
    findFunctionName[0] = '\0';
    Utils::Strcat(findFunctionName, findFunPrefix);
    Utils::Strcat(findFunctionName, propertyGridClass->GetClassName());
    
    globalFunctions[1].functionName = findFunctionName;
    globalFunctions[1].returnType = propertyGridClass->GetClassName();
    globalFunctions[1].argumentTypes[0] = "string";
    globalFunctions[1].argumentNames[0] = "Name";
    globalFunctions[1].argumentTypes[1] = nullptr;
    globalFunctions[1].argumentNames[1] = nullptr;
    globalFunctions[1].callback = GlobalCache_PrototypeFindGenericResource;

    lib->CreateIntrinsicFunctions(globalFunctions, sizeof(globalFunctions)/sizeof(globalFunctions[0]));
}


}
}
