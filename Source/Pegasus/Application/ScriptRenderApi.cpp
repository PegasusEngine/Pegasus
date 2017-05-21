/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ScriptRenderApi.cpp
//! \author Kleber Garcia
//! \date   December 4th, 2014
//! \brief  BlockScript registration of Parr callbacks, so scripts can call, use and abuse
//!         of rendering nodes.

#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Application/ScriptRenderApi.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunDesc.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/BlockScript/BLockScriptAst.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Color.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Timeline/BlockRuntimeScriptListener.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::BlockScript;
using namespace Pegasus::Application;
using namespace Pegasus::BlockScript::Ast;
using namespace Pegasus::Math;

//global type, used for dynamic type checking
static void RegisterTypes        (BlockLib* lib, Core::IApplicationContext* context);
static void RegisterFunctions    (BlockLib* lib);

///////////////////////////////////////////////////////////////////////////////////
//! Forward declaration of API function wrappers
///////////////////////////////////////////////////////////////////////////////////

static Application::RenderCollection* GetContainer(BsVmState* state);

////Program Methods//////////////////////////////////////////
void Program_SetShaderStage(FunCallbackContext& context);

////Mesh Methods/////////////////////////////////////////////
void MeshOperator_AddOperatorInput(FunCallbackContext& context);
void MeshOperator_AddGeneratorInput(FunCallbackContext& context);
void Mesh_SetGeneratorInput(FunCallbackContext& context);
void Mesh_SetOperatorInput(FunCallbackContext& context);

/////Texture Methods/////////////////////////////////////////
void TextureOperator_AddOperatorInput(FunCallbackContext& context);
void TextureOperator_AddGeneratorInput(FunCallbackContext& context);
void Texture_SetGeneratorInput(FunCallbackContext& context);
void Texture_SetOperatorInput(FunCallbackContext& context);

/////Node Manager Methods////////////////////////////////////
void Node_LoadProgram(FunCallbackContext& context);
void Node_CreateTexture(FunCallbackContext& context);
void Node_CreateTextureGenerator(FunCallbackContext& context);
void Node_CreateTextureOperator(FunCallbackContext& context);
void Node_CreateMesh(FunCallbackContext& context);
void Node_CreateMeshGenerator(FunCallbackContext& context);
void Node_CreateMeshOperator(FunCallbackContext& context);

/////Render API Functions////////////////////////////////////
void Render_CreateUniformBuffer(FunCallbackContext& context);
void Render_SetBuffer(FunCallbackContext& context);
void Render_GetUniformLocation(FunCallbackContext& context);
void Render_SetUniformBuffer(FunCallbackContext& context);
void Render_SetUniformTexture(FunCallbackContext& context);
void Render_SetUniformTextureRenderTarget(FunCallbackContext& context);
void Render_SetProgram(FunCallbackContext& context);
void Render_SetMesh(FunCallbackContext& context);
void Render_UnbindMesh(FunCallbackContext& context);
void Render_UnbindComputeOutputs(FunCallbackContext& context);
void Render_UnbindRenderTargets(FunCallbackContext& context);
void Render_UnbindComputeResources(FunCallbackContext& context);
void Render_UnbindPixelResources(FunCallbackContext& context);
void Render_UnbindVertexResources(FunCallbackContext& context);
void Render_SetViewport(FunCallbackContext& context);
void Render_SetViewport2(FunCallbackContext& context);
void Render_SetViewport3(FunCallbackContext& context);
void Render_SetRenderTarget(FunCallbackContext& context);
void Render_SetRenderTarget2(FunCallbackContext& context);
void Render_SetRenderTargets(FunCallbackContext& context);
void Render_SetRenderTargets2(FunCallbackContext& context);
void Render_SetDefaultRenderTarget(FunCallbackContext& context);
void Render_SetPrimitiveMode(FunCallbackContext& context);
void Render_Clear(FunCallbackContext& context);
void Render_SetClearColorValue(FunCallbackContext& context);
void Render_SetRasterizerState(FunCallbackContext& context);
void Render_SetBlendingState(FunCallbackContext& context);
void Render_SetDepthClearValue(FunCallbackContext& context);
void Render_Draw(FunCallbackContext& context);
void Render_CreateRenderTarget(FunCallbackContext& context);
void Render_CreateDepthStencil(FunCallbackContext& context);
void Render_CreateRasterizerState(FunCallbackContext& context);
void Render_CreateBlendingState(FunCallbackContext& context);

#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
#define CHECK_PERMISSIONS(_renderCollection, funcall, perms) \
    if (!(_renderCollection->GetPermissions() & perms))\
    {\
        PG_LOG('ERR_', "Cannot call \"%s\" on this context. Invalid permissions.", funcall);\
        return;\
    }
#else
#define CHECK_PERMISSIONS(_renderCollection, funcall, perms)
#endif

/////Global cache Functions////////////////////////////////////
template<typename T, bool isWindowIdUsed=false> void GlobalCache_Register(FunCallbackContext& context);
template<typename T, bool isWindowIdUsed=false> void GlobalCache_Find(FunCallbackContext& context);

template<bool isWindowIdUsed=false>
void Templated_GlobalCache_PrototypeRegisterGenericResource(FunCallbackContext& context)
{
    GlobalCache_Register<Application::GenericResource,isWindowIdUsed>(context);
}

template<bool isWindowIdUsed=false>
void Templated_GlobalCache_PrototypeFindGenericResource(FunCallbackContext& context)
{
    GlobalCache_Find<Application::GenericResource,isWindowIdUsed>(context);
    Application::RenderCollection* collection = GetContainer(context.GetVmState());
    //check if the types are correct, if not error out and submit an invalid handle.
    PG_ASSERT(sizeof(RenderCollection::CollectionHandle) == context.GetOutputBufferSize());
    RenderCollection::CollectionHandle* requestedReturn = static_cast<RenderCollection::CollectionHandle*>(context.GetRawOutputBuffer());
    if (*requestedReturn != RenderCollection::INVALID_HANDLE)
    {
        const FunDesc* fundDesc =  context.GetFunDesc();
        const char* requestedReturnType = fundDesc->GetDec()->GetReturnType()->GetName();
        Application::GenericResource* genericResource = RenderCollection::GetResource<Application::GenericResource>(collection, *requestedReturn);
        if (Utils::Strcmp(genericResource->GetClassInfo()->GetClassName(), requestedReturnType) != 0)
        {
            PG_LOG('ERR_', "Error while trying to find generic resource. Incompatible types, Pegasus cannot cast to a %s. Resource is registered as a %s",requestedReturnType,genericResource->GetClassInfo()->GetClassName());
            *requestedReturn = RenderCollection::INVALID_HANDLE; // set as invalid handle and bail!
        }
    }
}

void GlobalCache_PrototypeRegisterGenericResource(FunCallbackContext& context)
{
    Templated_GlobalCache_PrototypeRegisterGenericResource<false>(context);
}

void GlobalCache_PrototypeRegisterGenericResourceWindowId(FunCallbackContext& context)
{
    Templated_GlobalCache_PrototypeRegisterGenericResource<true>(context);
}

void GlobalCache_PrototypeFindGenericResource(FunCallbackContext& context)
{
    Templated_GlobalCache_PrototypeFindGenericResource<false>(context);
}

void GlobalCache_PrototypeFindGenericResourceWindowId(FunCallbackContext& context)
{
    Templated_GlobalCache_PrototypeFindGenericResource<true>(context);
}

// property callback functions
template<typename T>
bool TemplatePropertyCallback   (const Pegasus::BlockScript::PropertyCallbackContext& context);

/////  Declaration of all Texture node properties  /////
struct CoreClassProperties
{
    CoreClassProperties()
    : mName(nullptr) {} 

    const char* mName;
    Utils::Vector<Pegasus::BlockScript::ObjectPropertyDesc> mPropertiesDescs;
#if PEGASUS_ENABLE_ASSERT
    Utils::Vector<const PropertyGrid::PropertyRecord*> mSanityCheckProperties;
#endif
};


///////////////////////////////////////////////////////////////////////////////////
//! Node / Render API registration functions. These are pure metadata attachments to
//! the blockscript runtime lib
///////////////////////////////////////////////////////////////////////////////////

void Pegasus::Application::RegisterRenderApi(BlockScript::BlockLib* rtLib, Core::IApplicationContext* context)
{
    RegisterTypes(rtLib, context);
    RegisterFunctions(rtLib);
}

static void RegisterRenderEnums(BlockLib* lib)
{
    using namespace Pegasus::Render;

    const EnumDeclarationDesc enumDefs[] = {
        {
            "PegasusDepthFunc",
            { 
                { "NONE_DF",RasterizerConfig::NONE_DF },
                { "GREATER_DF",RasterizerConfig::GREATER_DF },
                { "LESSER_DF",RasterizerConfig::LESSER_DF },
                { "GREATER_EQUAL_DF",RasterizerConfig::GREATER_EQUAL_DF },
                { "LESSER_EQUAL_DF",RasterizerConfig::LESSER_EQUAL_DF },
                { "EQUAL_DF",RasterizerConfig::EQUAL_DF }
            },
            RasterizerConfig::COUNT_DF //COUNT
        },
        {
            "PegasusCullMode",
            { 
                { "NONE_CM",RasterizerConfig::NONE_CM },
                { "CCW_CM",RasterizerConfig::CCW_CM },
                { "CW_CM",RasterizerConfig::CW_CM }
            },
            RasterizerConfig::COUNT_CM //COUNT
        },
        {
            "BlendOperator",
            {
                { "NONE_BO", BlendingConfig::NONE_BO },
                { "ADD_BO", BlendingConfig::NONE_BO },
                { "SUB_BO", BlendingConfig::NONE_BO }
            },
            BlendingConfig::COUNT_BO
        },
        {
            "Multiplicator",
            {
                { "ZERO_M",BlendingConfig::ZERO_M },
                { "ONE_M",BlendingConfig::ONE_M },
                { "SRC_ALPHA", BlendingConfig::SRC_ALPHA },
                { "INV_SRC_ALPHA", BlendingConfig::INV_SRC_ALPHA },
                { "DEST_ALPHA", BlendingConfig::DEST_ALPHA },
                { "INV_DEST_ALPHA", BlendingConfig::INV_DEST_ALPHA }
            },
            BlendingConfig::COUNT_M
        },
        {
            "PrimitiveMode",
            {
                { "PRIMITIVE_TRIANGLE_LIST", Pegasus::Render::PRIMITIVE_TRIANGLE_LIST },
                { "PRIMITIVE_TRIANGLE_STRIP",Pegasus::Render::PRIMITIVE_TRIANGLE_STRIP},
                { "PRIMITIVE_LINE_LIST",     Pegasus::Render::PRIMITIVE_LINE_LIST     },
                { "PRIMITIVE_LINE_STRIP",    Pegasus::Render::PRIMITIVE_LINE_STRIP    },  
                { "PRIMITIVE_POINTS",        Pegasus::Render::PRIMITIVE_POINTS        },
                { "PRIMITIVE_AUTOMATIC",     Pegasus::Render::PRIMITIVE_AUTOMATIC     }
            },
            Pegasus::Render::PRIMITIVE_COUNT //this includes automatic
        },
        {
            "Format",
            {
                { "FORMAT_RGBA_32_FLOAT" , Pegasus::Core::FORMAT_RGBA_32_FLOAT },
                { "FORMAT_RGBA_32_UINT" , Pegasus::Core::FORMAT_RGBA_32_UINT },
                { "FORMAT_RGBA_32_SINT" , Pegasus::Core::FORMAT_RGBA_32_SINT },
                { "FORMAT_RGBA_32_TYPELESS" , Pegasus::Core::FORMAT_RGBA_32_TYPELESS },
                { "FORMAT_RGB_32_FLOAT" , Pegasus::Core::FORMAT_RGB_32_FLOAT },
                { "FORMAT_RGB_32_UINT" , Pegasus::Core::FORMAT_RGB_32_UINT },
                { "FORMAT_RGB_32_SINT" , Pegasus::Core::FORMAT_RGB_32_SINT },
                { "FORMAT_RGB_32_TYPELESS" , Pegasus::Core::FORMAT_RGB_32_TYPELESS },
                { "FORMAT_RG_32_FLOAT" , Pegasus::Core::FORMAT_RG_32_FLOAT },
                { "FORMAT_RG_32_UINT" , Pegasus::Core::FORMAT_RG_32_UINT },
                { "FORMAT_RG_32_SINT" , Pegasus::Core::FORMAT_RG_32_SINT },
                { "FORMAT_RG_32_TYPELESS" , Pegasus::Core::FORMAT_RG_32_TYPELESS },
                { "FORMAT_RGBA_16_FLOAT" , Pegasus::Core::FORMAT_RGBA_16_FLOAT },
                { "FORMAT_RGBA_16_UINT" , Pegasus::Core::FORMAT_RGBA_16_UINT },
                { "FORMAT_RGBA_16_SINT" , Pegasus::Core::FORMAT_RGBA_16_SINT },
                { "FORMAT_RGBA_16_UNORM" , Pegasus::Core::FORMAT_RGBA_16_UNORM },
                { "FORMAT_RGBA_16_SNORM" , Pegasus::Core::FORMAT_RGBA_16_SNORM },
                { "FORMAT_RGBA_16_TYPELESS" , Pegasus::Core::FORMAT_RGBA_16_TYPELESS },
                { "FORMAT_RGBA_8_UINT" , Pegasus::Core::FORMAT_RGBA_8_UINT },
                { "FORMAT_RGBA_8_SINT" , Pegasus::Core::FORMAT_RGBA_8_SINT },
                { "FORMAT_RGBA_8_UNORM" , Pegasus::Core::FORMAT_RGBA_8_UNORM },
                { "FORMAT_RGBA_8_UNORM_SRGB" , Pegasus::Core::FORMAT_RGBA_8_UNORM_SRGB },
                { "FORMAT_RGBA_8_SNORM" , Pegasus::Core::FORMAT_RGBA_8_SNORM },
                { "FORMAT_RGBA_8_TYPELESS" , Pegasus::Core::FORMAT_RGBA_8_TYPELESS },
                { "FORMAT_D32_FLOAT" , Pegasus::Core::FORMAT_D32_FLOAT },
                { "FORMAT_R32_FLOAT" , Pegasus::Core::FORMAT_R32_FLOAT },
                { "FORMAT_R32_UINT" , Pegasus::Core::FORMAT_R32_UINT },
                { "FORMAT_R32_SINT" , Pegasus::Core::FORMAT_R32_SINT },
                { "FORMAT_R32_TYPELESS" , Pegasus::Core::FORMAT_R32_TYPELESS },
                { "FORMAT_D16_UNORM" , Pegasus::Core::FORMAT_D16_UNORM },
                { "FORMAT_R16_FLOAT" , Pegasus::Core::FORMAT_R16_FLOAT },
                { "FORMAT_R16_UINT" , Pegasus::Core::FORMAT_R16_UINT },
                { "FORMAT_R16_SINT" , Pegasus::Core::FORMAT_R16_SINT },
                { "FORMAT_R16_UNORM" , Pegasus::Core::FORMAT_R16_UNORM },
                { "FORMAT_R16_SNORM" , Pegasus::Core::FORMAT_R16_SNORM },
                { "FORMAT_R16_TYPELESS" , Pegasus::Core::FORMAT_R16_TYPELESS },
                { "FORMAT_RG16_FLOAT",  Pegasus::Core::FORMAT_RG16_FLOAT },
                { "FORMAT_RG16_UINT",   Pegasus::Core::FORMAT_RG16_UINT },
                { "FORMAT_RG16_SINT",   Pegasus::Core::FORMAT_RG16_SINT },
                { "FORMAT_RG16_UNORM",  Pegasus::Core::FORMAT_RG16_UNORM },
                { "FORMAT_RG16_SNORM",  Pegasus::Core::FORMAT_RG16_SNORM },
                { "FORMAT_RG16_TYPELESS", Pegasus::Core::FORMAT_RG16_TYPELESS },
                { "FORMAT_R8_UNORM" , Pegasus::Core::FORMAT_R8_UNORM },
                { "FORMAT_R8_SINT" , Pegasus::Core::FORMAT_R8_SINT },
                { "FORMAT_R8_UINT" , Pegasus::Core::FORMAT_R8_UINT },
                { "FORMAT_R8_SNORM" , Pegasus::Core::FORMAT_R8_SNORM },
                { "FORMAT_R8_TYPELESS" , Pegasus::Core::FORMAT_R8_TYPELESS }
            },
            Pegasus::Core::FORMAT_MAX_COUNT //this includes automatic
        }
    };

    //Sanity checks:
#if PEGASUS_ENABLE_ASSERT
    const char* assertstr = "Make sure to register the proper enumeration manually in blockscript.";
    PG_ASSERTSTR(RasterizerConfig::COUNT_DF == 6, assertstr);
    PG_ASSERTSTR(RasterizerConfig::COUNT_CM == 3,assertstr);
    PG_ASSERTSTR(BlendingConfig::COUNT_BO   == 3,assertstr);
    PG_ASSERTSTR(BlendingConfig::COUNT_M    == 6,assertstr);
    PG_ASSERTSTR(Pegasus::Render::PRIMITIVE_COUNT == 6,assertstr);
#endif
    lib->CreateEnumTypes(enumDefs, sizeof(enumDefs)/sizeof(enumDefs[0]));

}

static void RegisterPropertyGridEnums(BlockLib* lib, Core::IApplicationContext* context)
{
    //Create enums registered in blockscript.
    PropertyGrid::PropertyGridManager* pgm = context->GetPropertyGridManager();
    const EnumDeclarationDesc editorTypeEnum = {
        "ExternEditorType",
        {
            { "EDITOR_TYPE_DEFAULT", Timeline::BlockRuntimeScriptListener::EDITOR_TYPE_DEFAULT},
            { "EDITOR_TYPE_CHECKBOX",Timeline::BlockRuntimeScriptListener::EDITOR_TYPE_CHECKBOX},
            { "EDITOR_TYPE_COLOR",   Timeline::BlockRuntimeScriptListener::EDITOR_TYPE_COLOR},
            { "EDITOR_TYPE_SLIDER",  Timeline::BlockRuntimeScriptListener::EDITOR_TYPE_SLIDER},
        },     
        Timeline::BlockRuntimeScriptListener::EDITOR_TYPE_COUNT
    };
    
    PG_ASSERTSTR(
          Timeline::BlockRuntimeScriptListener::EDITOR_TYPE_COUNT == 4
        , "The editor count enumeration must match that one of the registered property grid."
    );

    Utils::Vector<EnumDeclarationDesc> blockscriptEnumRegistration;
    blockscriptEnumRegistration.PushEmpty() = editorTypeEnum;
    if (pgm->GetNumRegisteredEnumInfos() > 0)
    {
        for (unsigned int i = 0; i < pgm->GetNumRegisteredEnumInfos(); ++i)
        {
            const PropertyGrid::EnumTypeInfo* info = pgm->GetEnumInfo(i);
            EnumDeclarationDesc& desc = blockscriptEnumRegistration.PushEmpty();
            desc.typeName = info->GetName();
            const Utils::Vector<const PropertyGrid::BaseEnumType*>& enumerations = info->GetEnumerations();
            int finalEnumCount = 0;
            for (unsigned int e = 0; e < enumerations.GetSize(); ++e)
            {
                if (e == MAX_ENUM_MEMBER_LIST - 1)  
                {
                    PG_FAILSTR("Maximum number of enum registration reached in blockscript property grid registration. Increase this number!");
                    break;
                }

                const PropertyGrid::BaseEnumType* enumValue = enumerations[e];
                desc.enumList[e].enumName = enumValue->GetName();
                desc.enumList[e].enumVal = enumValue->GetValue();
                ++finalEnumCount;
            }
            desc.count = finalEnumCount;
        }

    }

    lib->CreateEnumTypes(blockscriptEnumRegistration.Data(), blockscriptEnumRegistration.GetSize());
}

static void RegisterRenderStructs(BlockLib* lib)
{
    //creating an internal render pointer size (in case of 64 bit)
    TypeDesc* ptrType = lib->GetSymbolTable()->CreateScalarType(
        "void_ptr",
        TypeDesc::E_INT //int ALU engine
    );

    ptrType->SetByteSize(sizeof(void*));


    const StructDeclarationDesc structDefs[] = {
       {  
            "Uniform",  // the size of this struct will be patched bellow, since pegasus api still
            { nullptr },// does not support registration of static array members
            { nullptr }
        }, 
        {
            "RenderTargetConfig",
            {"int"  , "int"   , "Format", nullptr },
            {"Width", "Height", "format",  nullptr }
        },
        {
            "DepthStencilConfig",
            {"int"  , "int"   , "int", nullptr },
            {"Width", "Height", "HasStencil",  nullptr }
        },
        {
            "Viewport",
            {"int"    , "int"    , "int"  , "int"   , nullptr },
            {"XOffset", "YOffset", "Width", "Height", nullptr }
        },
        {
            "RasterizerConfig",
            {"PegasusCullMode", "PegasusDepthFunc", nullptr },
            {"CullMode"       , "DepthFunc"       , nullptr }
        },
        {
            "BlendingConfig",
            {"BlendOperator"   , "Multiplicator", "Multiplicator", nullptr },
            {"BlendingOperator", "Source"       , "Dest"         , nullptr }
        }
    };

    const int structDefSize = sizeof(structDefs) / sizeof(structDefs[0]);
    lib->CreateStructTypes(structDefs, structDefSize);

    //patch uniform type size
    TypeDesc* uniformType = lib->GetSymbolTable()->GetTypeForPatching("Uniform");
    PG_ASSERT(uniformType != nullptr);
    uniformType->SetByteSize(sizeof(Render::Uniform));
}

const PropertyGrid::PropertyGridClassInfo* GetNodeDefRegisteredClass(
    const PropertyGrid::PropertyGridClassInfo* candidate, 
    const ClassTypeDesc* registeredClasses, 
    int registeredClassesTypes)
{
    
    const PropertyGrid::PropertyGridClassInfo* curr = candidate; 
    do
    {
        for (int i = 0; i < registeredClassesTypes; ++i)
        {
            if (!Utils::Strcmp(curr->GetClassName(), registeredClasses[i].classTypeName))
            {
                return curr;
            }
        }
        curr = curr->GetParentClassInfo();
    } while (curr != nullptr);

    return nullptr;
}

void LinearizeProperties(
    Utils::Vector<CoreClassProperties>& outCoreClasses, 
    PropertyGrid::PropertyGridManager* propGridMgr,
    const ClassTypeDesc* nodeDefs,
    int nodeDefsSizes
)
{
    for (unsigned classIt = 0; classIt < propGridMgr->GetNumRegisteredClasses(); ++classIt)
    {
        const PropertyGrid::PropertyGridClassInfo* classInfo = &propGridMgr->GetClassInfo(classIt);

        const PropertyGrid::PropertyGridClassInfo* targetClassInfo = 
        GetNodeDefRegisteredClass(classInfo, nodeDefs, nodeDefsSizes); //Get the class that is 'basic', meaning the one
                                                                       //that blockscript has registered, so we can dump there all
                                                                       //the properties and all its children properties

        if (targetClassInfo != nullptr) // means this class has nothing in its inheritance chain registered in blockscript
        {
            const char* targetBaseClassName = targetClassInfo->GetClassName();
            
            CoreClassProperties* targetBaseClassProps = nullptr;
            //find the base class property pool
            for (unsigned int baseClassPoolIt = 0; baseClassPoolIt < outCoreClasses.GetSize(); ++baseClassPoolIt)
            {
                if (!Utils::Strcmp(outCoreClasses[baseClassPoolIt].mName, targetBaseClassName))
                {
                    targetBaseClassProps = &outCoreClasses[baseClassPoolIt];
                }
            }
    
            if (targetBaseClassProps == nullptr)
            {
                targetBaseClassProps = &(outCoreClasses.PushEmpty());
                targetBaseClassProps->mName = targetBaseClassName;
            }
    
            //now we have a target pool to dump all the properties cached of such parent class
            for (unsigned propIt = 0; propIt < classInfo->GetNumDerivedClassProperties(); ++propIt)
            {
                int uniqueId = targetBaseClassProps->mPropertiesDescs.GetSize();                
                const PropertyGrid::PropertyRecord& record = classInfo->GetDerivedClassPropertyRecord(propIt);
    
                //For now only support the following types of properties in blockscript for modification
                const char* typeName = record.typeName;
                //patch the type name for certain types.
                if (record.type == PropertyGrid::PROPERTYTYPE_BOOL || record.type == PropertyGrid::PROPERTYTYPE_UINT)
                {
                    typeName = "int";
                }

                if (!(
                      record.type == PropertyGrid::PROPERTYTYPE_BOOL
                  ||  record.type == PropertyGrid::PROPERTYTYPE_FLOAT
                  ||  record.type == PropertyGrid::PROPERTYTYPE_VEC2
                  ||  record.type == PropertyGrid::PROPERTYTYPE_VEC3
                  ||  record.type == PropertyGrid::PROPERTYTYPE_VEC4
                  ||  record.type == PropertyGrid::PROPERTYTYPE_INT                  
                  ||  record.type == PropertyGrid::PROPERTYTYPE_UINT     
                  ||  record.type == PropertyGrid::PROPERTYTYPE_CUSTOM_ENUM
                ))
                {
                    continue;
                }
    #if PEGASUS_ENABLE_ASSERT
                targetBaseClassProps->mSanityCheckProperties.PushEmpty() = &record;
    #endif  
                Pegasus::BlockScript::ObjectPropertyDesc& objPropDesc = targetBaseClassProps->mPropertiesDescs.PushEmpty(); 
                objPropDesc.propertyTypeName = typeName;
                objPropDesc.propertyName = record.name;
                objPropDesc.propertyUniqueId = uniqueId;
            }
        }
    }
}

static void RegisterNodes(BlockLib* lib, Core::IApplicationContext* context)
{
    PropertyGrid::PropertyGridManager* propGridMgr = context->GetPropertyGridManager();
    Application::RenderCollectionFactory* renderCollectionFactory = context->GetRenderCollectionFactory();

    ClassTypeDesc nodeDefs[] = {
        {
            "Buffer",
            {}, 0, nullptr, 0, nullptr 
        },
        {
            "RenderTarget",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "DepthStencil",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "VolumeTexture",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "CubeMap",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "BlendingState",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "RasterizerState",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "ShaderStage",
            {}, 0, nullptr, 0, nullptr
        },
        {
            "ProgramLinkage",
            { //method list
                { "SetShaderStage", "int", {"ProgramLinkage", "ShaderStage", nullptr}, {"this", "stage", nullptr}, Program_SetShaderStage }
            },
            1,
            nullptr, 0, nullptr
        },
        {
            "MeshGenerator",
            {},0,
            nullptr, 0, 
            TemplatePropertyCallback<Mesh::MeshGenerator>
        },
        {
            "MeshOperator",
            {
                { "AddGeneratorInput", "int", { "MeshOperator", "MeshGenerator", nullptr }, { "this", "meshGenerator", nullptr }, MeshOperator_AddGeneratorInput },
                { "AddOperatorInput",  "int", { "MeshOperator", "MeshOperator", nullptr },  { "this", "meshOperator", nullptr },  MeshOperator_AddOperatorInput  }
            },
            2,
            nullptr, 0, 
            TemplatePropertyCallback<Mesh::MeshOperator>
        },
        {
            "Mesh",
            {
                { "SetGeneratorInput", "int", {"Mesh", "MeshGenerator", nullptr}, {"this", "meshGenerator", nullptr}, Mesh_SetGeneratorInput },
                { "SetOperatorInput", "int", {"Mesh", "MeshOperator", nullptr}, {"this", "meshOperator", nullptr}, Mesh_SetOperatorInput }
            },
            2,
            nullptr, 0, nullptr
        },
        {
            "TextureGenerator",
            {}, 0,
            nullptr, 0,
            TemplatePropertyCallback<Texture::TextureGenerator>
        },
        {
            "TextureOperator",
            {
                { "AddGeneratorInput", "int", { "TextureOperator", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, TextureOperator_AddGeneratorInput },
                { "AddOperatorInput",  "int", { "TextureOperator", "TextureOperator", nullptr },  { "this", "texOperator", nullptr },  TextureOperator_AddOperatorInput  }
            },
            2,
            nullptr, 0,
            TemplatePropertyCallback<Texture::TextureOperator>
        },
        {
            "Texture",
            {
                { "SetGeneratorInput", "int", { "Texture", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, Texture_SetGeneratorInput },
                { "SetOperatorInput", "int", { "Texture", "TextureOperator", nullptr }, { "this", "texOperator", nullptr }, Texture_SetOperatorInput },
            },
            2,
            nullptr, 0, nullptr
        },
        {
            "GenericResource",
            {}, 0, nullptr, 0, nullptr
        },
    };

    const int nodeDefsSize = sizeof(nodeDefs)/sizeof(nodeDefs[0]);

    //gather the properties and accumulate them on the base classes
    Utils::Vector<CoreClassProperties> coreClasses; 
    LinearizeProperties(coreClasses, propGridMgr, nodeDefs, nodeDefsSize);

    //patch the node Defs with the appropiate property lists gathered
    for (int i = 0; i < nodeDefsSize; ++i)
    {
        ClassTypeDesc& desc = nodeDefs[i];
        
        for (unsigned int d = 0; d < coreClasses.GetSize(); ++d)
        {
            CoreClassProperties& coreClassProp = coreClasses[d];
            if (!Utils::Strcmp(desc.classTypeName, coreClassProp.mName))
            {
                desc.propertyDescriptors = coreClassProp.mPropertiesDescs.Data();
                desc.propertyCount = coreClassProp.mPropertiesDescs.GetSize();
                renderCollectionFactory->RegisterProperties(desc);
                break;
            }
        }
    }

    //Register the node Defs into blockscript
    lib->CreateClassTypes(nodeDefs, sizeof(nodeDefs)/sizeof(nodeDefs[0]));

    //now that types are recovered, lets do a sanity check on property grid sizes vs block script discovered size association
    //if we have properties that could stomp on memory we will check them right here.
#if PEGASUS_ENABLE_ASSERT
    for (unsigned int i = 0; i < coreClasses.GetSize(); ++i)
    {
        CoreClassProperties& coreClassProp = coreClasses[i];
        const TypeDesc* blockScriptType = lib->GetSymbolTable()->GetTypeByName(coreClassProp.mName);
        if (blockScriptType == nullptr) continue; //means is a class we are not interested in but uses the property grid
        PG_ASSERTSTR(blockScriptType->GetModifier() == TypeDesc::M_REFERECE, "Type registered on blockscript is not a c++ class");
        
        for (unsigned int p = 0; p < coreClassProp.mSanityCheckProperties.GetSize(); ++p)
        {
            const PropertyGrid::PropertyRecord* record = coreClassProp.mSanityCheckProperties[p];
            //linearly walk over the node property link list, find the property and check sizes
            const PropertyNode* propNode = blockScriptType->GetPropertyNode();
            while (propNode != nullptr && Utils::Strcmp(propNode->mName, record->name))
            {
                propNode = propNode->mNext;
            }

            PG_ASSERTSTR(propNode != nullptr, "Property node was not registered!");
            PG_ASSERTSTR(
                record->size <= propNode->mType->GetByteSize() 
                || ( propNode->mType->GetModifier() == Pegasus::BlockScript::TypeDesc::M_ENUM
                     &&  sizeof(int) == propNode->mType->GetByteSize()
                   ), 
                "FATAL! record byte size won't fit inside blockscripts properties storage tables, causing memory stomps. Class: %s, property: %s",
                coreClassProp.mName,
                record->name
            );
        }
        
    }
#endif
}

static void RegisterTypes(BlockLib* lib, Core::IApplicationContext* context)
{
    RegisterRenderEnums(lib);
    RegisterRenderStructs(lib);
    RegisterPropertyGridEnums(lib, context);
    RegisterNodes(lib, context);
}

static void RegisterFunctions(BlockLib* lib)
{
    const FunctionDeclarationDesc funDeclarations[] = {
        {
            "LoadProgram",
            "ProgramLinkage",
            { "string", nullptr },
            { "path", nullptr },
            Node_LoadProgram
        },
        {
            "CreateTexture",
            "Texture",
            { nullptr },
            { nullptr },
            Node_CreateTexture
        },
        {
            "CreateTextureGenerator",
            "TextureGenerator",
            { "string", nullptr },
            { "typeDesc", nullptr },
            Node_CreateTextureGenerator
        },
        {
            "CreateTextureOperator",
            "TextureOperator",
            { "string", nullptr },
            { "typeId", nullptr },
            Node_CreateTextureGenerator
        },
        {
            "CreateMesh",
            "Mesh",
            { nullptr },
            { nullptr },
            Node_CreateMesh
        },
        {
            "CreateMeshGenerator",
            "MeshGenerator",
            { "string", nullptr },
            { "typeId", nullptr },
            Node_CreateMeshGenerator
        },
        {
            "CreateMeshOperator",
            "MeshOperator",
            { "string", nullptr },
            { "typeId", nullptr },
            Node_CreateMeshOperator
        },
        // Render API registration
        {
            "CreateUniformBuffer",
            "Buffer",
            { "int",        nullptr },
            { "bufferSize", nullptr },
            Render_CreateUniformBuffer
        },
        {
            "SetBuffer",
            "int",
            { "Buffer", "*" },
            { "dstBuffer", "sourceBuffer" },
            Render_SetBuffer
        },
        {
            "GetUniformLocation",
            "Uniform",
            { "ProgramLinkage","string", nullptr },
            { "program","uniformName", nullptr },
            Render_GetUniformLocation
        },
        {
            "SetUniformBuffer",
            "int",
            { "Uniform","Buffer", nullptr },
            { "uniform","buffer", nullptr },
            Render_SetUniformBuffer
        },
        {
            "SetUniformTexture",
            "int",
            { "Uniform","Texture", nullptr },
            { "uniform","texture", nullptr },
            Render_SetUniformTexture
        },
        {
            "SetUniformTextureRenderTarget",
            "int",
            { "Uniform", "RenderTarget", nullptr },
            { "uniform", "renderTarget", nullptr },
            Render_SetUniformTextureRenderTarget
        },
        {
            "SetProgram",
            "int",
            { "ProgramLinkage", nullptr },
            { "program", nullptr },
            Render_SetProgram
        },
        {
            "SetMesh",
            "int",
            { "Mesh", nullptr },
            { "mesh", nullptr },
            Render_SetMesh
        },
        {
            "UnbindMesh",
            "int",
            { nullptr },
            { nullptr },
            Render_UnbindMesh
        },
        {
            "UnbindComputeOutputs",
            "int",
            { nullptr },
            { nullptr },
            Render_UnbindComputeOutputs
        },
        {
            "UnbindRenderTargets",
            "int",
            { nullptr },
            { nullptr },
            Render_UnbindRenderTargets
        },
        {
            "UnbindPixelResources",
            "int",
            { nullptr },
            { nullptr },
            Render_UnbindPixelResources
        },
        {
            "UnbindComputeResources",
            "int",
            { nullptr },
            { nullptr },
            Render_UnbindComputeResources
        },
        {
            "UnbindVertexResources",
            "int",
            { nullptr },
            { nullptr },
            Render_UnbindVertexResources
        },
        {
            "SetViewport",
            "int",
            { "Viewport", nullptr },
            { "vp", nullptr },
            Render_SetViewport
        },
        {
            "SetViewport",
            "int",
            { "RenderTarget", nullptr },
            { "vp", nullptr },
            Render_SetViewport2
        },
        {
            "SetViewport",
            "int",
            { "DepthStencil", nullptr },
            { "vp", nullptr },
            Render_SetViewport3
        },
        {
            "SetRenderTarget",
            "int",
            { "RenderTarget", nullptr },
            { "renderTarget", nullptr },
            Render_SetRenderTarget
        },
        {
            "SetRenderTarget",
            "int",
            { "RenderTarget", "DepthStencil", nullptr },
            { "renderTarget", "depthStencilTarget", nullptr },
            Render_SetRenderTarget2
        },
        {
            "SetRenderTargets",
            "int",
            { "int"               , "*"              , "DepthStencil", nullptr },
            { "renderTargetCounts", "renderTargets[]", "depthStencilTarget", nullptr },
            Render_SetRenderTargets
        },
        {
            "SetRenderTargets",
            "int",
            { "int"               , "*"              ,  nullptr },
            { "renderTargetCounts", "renderTargets[]",  nullptr },
            Render_SetRenderTargets2
        },
        {
            "SetDefaultRenderTarget",
            "int",
            { nullptr },
            { nullptr },
            Render_SetDefaultRenderTarget
        },
        {
            "SetPrimitiveMode",
            "int",
            { "PrimitiveMode", nullptr },
            { "Mode",          nullptr },
            Render_SetPrimitiveMode
        },
        {
            "Clear",
            "int",
            { "int", "int", "int", nullptr },
            { "color", "depth", "stencil", nullptr },
            Render_Clear
        },
        {
            "SetClearColorValue",
            "int",
            { "float4", nullptr },
            { "clearCol", nullptr },
            Render_SetClearColorValue
        },
        {
            "SetRasterizerState",
            "int",
            { "RasterizerState", nullptr },
            { "rasterState", nullptr },
            Render_SetRasterizerState
        },
        {
            "SetBlendingState",
            "int",
            { "BlendingState", nullptr },
            { "blendingState", nullptr },
            Render_SetBlendingState
        },
        {
            "SetDepthClearValue",
            "int",
            { "float", nullptr },
            { "d", nullptr },
            Render_SetDepthClearValue
        },
        {
            "Draw",
            "int",
            { nullptr },
            { nullptr },
            Render_Draw
        },
        {
            "CreateRenderTarget",
            "RenderTarget",
            { "RenderTargetConfig", nullptr },
            { "config", nullptr },
            Render_CreateRenderTarget
        },
        {
            "CreateDepthStencil",
            "DepthStencil",
            { "DepthStencilConfig", nullptr },
            { "config", nullptr },
            Render_CreateDepthStencil
        },
        {
            "CreateRasterizerState",
            "RasterizerState",
            { "RasterizerConfig", nullptr },
            { "config", nullptr },
            Render_CreateRasterizerState
        },
        {
            "CreateBlendingState",
            "BlendingState",
            { "BlendingConfig", nullptr },
            { "config", nullptr },
            Render_CreateBlendingState
        }
    };

    lib->CreateIntrinsicFunctions(funDeclarations, sizeof(funDeclarations) / sizeof(funDeclarations[0]));
    
#define RES_PROCESS(resourceType, memberName, typeName, hasProperties, canUpdate) \
        {\
            "GlobalRegister" typeName,\
            "int",\
            { "string", typeName, nullptr },\
            { "Name", typeName, nullptr },\
            GlobalCache_Register<resourceType,false>\
        },\
        {\
            "GlobalRegister" typeName,\
            "int",\
            { "string", "int", typeName, nullptr },\
            { "Name", "windowId", typeName, nullptr },\
            GlobalCache_Register<resourceType,true>\
        },\
        {\
            "GlobalFind" typeName,\
            typeName,\
            { "string", nullptr },\
            { "Name", nullptr },\
            GlobalCache_Find<resourceType,false> \
        },\
        {\
            "GlobalFind" typeName,\
            typeName,\
            { "string", "int", nullptr },\
            { "Name", "windowId", nullptr },\
            GlobalCache_Find<resourceType,true> \
        },\

    const FunctionDeclarationDesc resourceFuncDecl[] = {
#include "..\Source\Pegasus\Application\RenderResources.inl"
    };
#undef RES_PROCESS

    lib->CreateIntrinsicFunctions(resourceFuncDecl, sizeof(resourceFuncDecl) / sizeof(resourceFuncDecl[0]));
}


/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//! Implementation of API wrappers / manipulation of a node container
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static Application::RenderCollection* GetContainer(BsVmState* state)
{
    Application::RenderCollection* container = static_cast<Application::RenderCollection*>(state->GetUserContext());
    PG_ASSERT(container != nullptr);
    return container;
}

/////////////////////////////////////////////////////////////
//!> Program Node functions
/////////////////////////////////////////////////////////////
void Program_SetShaderStage(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* container = GetContainer(state);    
    FunParamStream stream(context);

    RenderCollection::CollectionHandle& programId = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle& stageId = stream.NextArgument<RenderCollection::CollectionHandle>();
    
    int retVal = 0;
    if (programId != RenderCollection::INVALID_HANDLE && stageId != RenderCollection::INVALID_HANDLE)
    {
        PG_ASSERT(programId >= 0 && programId < RenderCollection::ResourceCount<Shader::ProgramLinkage>(container));
        PG_ASSERT(stageId   >= 0 && stageId < RenderCollection::ResourceCount<Shader::ShaderStage>(container));
        Shader::ShaderStageRef currShader = RenderCollection::GetResource<Shader::ShaderStage>(container, stageId);
        RenderCollection::GetResource<Shader::ProgramLinkage>(container, programId)->SetShaderStage(currShader);
        retVal = 1;
    }
    else
    {
        PG_LOG('ERR_', "Failed setting shader stage.");
    }
    stream.SubmitReturn(retVal);
}

/////////////////////////////////////////////////////////////
//!> Mesh Node functions
/////////////////////////////////////////////////////////////
void MeshOperator_AddOperatorInput(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();    
    RenderCollection* collection = GetContainer(state);
    FunParamStream stream(context);
    
    RenderCollection::CollectionHandle& meshOpHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle& opToAddHandle  = stream.NextArgument<RenderCollection::CollectionHandle>();

    if ( meshOpHandle != RenderCollection::INVALID_HANDLE && opToAddHandle != RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshOperatorRef srcMesh = RenderCollection::GetResource<Mesh::MeshOperator>(collection, meshOpHandle);
        Mesh::MeshOperatorRef dstMesh = RenderCollection::GetResource<Mesh::MeshOperator>(collection, opToAddHandle);
        srcMesh->AddOperatorInput(dstMesh);
    }
    else
    {
        PG_LOG('ERR_', "Invalid meshes being set in ->AddOperatorInput");
    }
}

void MeshOperator_AddGeneratorInput(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();    
    RenderCollection* collection = GetContainer(state);
    FunParamStream stream(context);
    
    RenderCollection::CollectionHandle& meshOpHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle& opToAddHandle  = stream.NextArgument<RenderCollection::CollectionHandle>();

    if ( meshOpHandle != RenderCollection::INVALID_HANDLE && opToAddHandle != RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshOperatorRef srcMesh = RenderCollection::GetResource<Mesh::MeshOperator>(collection, meshOpHandle);
        Mesh::MeshGeneratorRef meshGeneratorRef = RenderCollection::GetResource<Mesh::MeshGenerator>(collection, opToAddHandle);
        srcMesh->AddGeneratorInput(meshGeneratorRef);
    }
    else
    {
        PG_LOG('ERR_', "Invalid meshes being set in ->AddGeneratorInput");
    }
}

void Mesh_SetOperatorInput(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();    
    RenderCollection* collection = GetContainer(state);
    FunParamStream stream(context);
    
    RenderCollection::CollectionHandle& meshHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle& opHandle  = stream.NextArgument<RenderCollection::CollectionHandle>();

    if ( meshHandle != RenderCollection::INVALID_HANDLE && opHandle != RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshRef mesh = RenderCollection::GetResource<Mesh::Mesh>(collection, meshHandle);
        Mesh::MeshOperatorRef meshOpRef = RenderCollection::GetResource<Mesh::MeshOperator>(collection, opHandle);
        mesh->SetOperatorInput(meshOpRef);
    }
    else
    {
        PG_LOG('ERR_', "Invalid meshes being set in ->SetOperatorInput");
    }
}

void Mesh_SetGeneratorInput(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();    
    RenderCollection* collection = GetContainer(state);
    FunParamStream stream(context);
    
    RenderCollection::CollectionHandle& meshHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle& genHandle  = stream.NextArgument<RenderCollection::CollectionHandle>();
    
    if ( meshHandle != RenderCollection::INVALID_HANDLE && genHandle != RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshRef mesh = RenderCollection::GetResource<Mesh::Mesh>(collection, meshHandle);
        Mesh::MeshGeneratorRef meshGeneratorRef = RenderCollection::GetResource<Mesh::MeshGenerator>(collection, genHandle);
        mesh->SetGeneratorInput(meshGeneratorRef);
    }
    else
    {
        PG_LOG('ERR_', "Invalid meshes being set in ->SetGeneratorInput");
    }
}

/////////////////////////////////////////////////////////////
//!> Texture Node functions
/////////////////////////////////////////////////////////////

void TextureOperator_AddOperatorInput(FunCallbackContext& context)
{
}

void TextureOperator_AddGeneratorInput(FunCallbackContext& context)
{
}

void Texture_SetOperatorInput(FunCallbackContext& context)
{
}

void Texture_SetGeneratorInput(FunCallbackContext& context)
{
}

/////////////////////////////////////////////////////////////
//!> Node Manager functions
/////////////////////////////////////////////////////////////

void Node_LoadProgram(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* container = GetContainer(state);
    CHECK_PERMISSIONS(container, "LoadProgram", PERMISSIONS_ASSET_LOAD);

    const char* path = stream.NextBsStringArgument();

    Shader::ProgramLinkageRef program = container->GetAppContext()->GetShaderManager()->LoadProgram(path);
    if (program != nullptr)
    {
        //force shader compilation
        bool unused = false;
        program->GetUpdatedData(unused);

        stream.SubmitReturn( RenderCollection::AddResource<Shader::ProgramLinkage>(container,program));
    }
    else
    {
        stream.SubmitReturn( Application::RenderCollection::INVALID_HANDLE ); //an invalid id
    }
}

void Node_CreateTexture(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);  
    Pegasus::Texture::TextureConfiguration blankConfig;
    Pegasus::Texture::TextureRef t = collection->GetAppContext()->GetTextureManager()->CreateTextureNode(blankConfig);
    if (t != nullptr)
    {
        RenderCollection::CollectionHandle handle = RenderCollection::AddResource<Texture::Texture>(collection, t);
        stream.SubmitReturn(handle);
    }
    else
    {
        PG_LOG('ERR_', "Cannot create texture. Invalid handle returned");
        stream.SubmitReturn(RenderCollection::INVALID_HANDLE);
    }
}

void Node_CreateTextureGenerator(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    const char* name = stream.NextBsStringArgument();
    Pegasus::Texture::TextureConfiguration blankConfig;
    Pegasus::Texture::TextureGeneratorRef t = collection->GetAppContext()->GetTextureManager()->CreateTextureGeneratorNode(name, blankConfig);
    if (t != nullptr)
    {
        RenderCollection::CollectionHandle handle = RenderCollection::AddResource<Texture::TextureGenerator>(collection, t);
        stream.SubmitReturn(handle);
    }
    else
    {
        PG_LOG('ERR_', "Cannot create texture. Invalid handle returned");
        stream.SubmitReturn(RenderCollection::INVALID_HANDLE);
    }
}

void Node_CreateTextureOperator(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    const char* name = stream.NextBsStringArgument();
    Pegasus::Texture::TextureConfiguration blankConfig;
    Pegasus::Texture::TextureOperatorRef t = collection->GetAppContext()->GetTextureManager()->CreateTextureOperatorNode(name, blankConfig);
    if (t != nullptr)
    {
        RenderCollection::CollectionHandle handle = RenderCollection::AddResource<Texture::TextureOperator>(collection, t);
        stream.SubmitReturn(handle);
    }
    else
    {
        PG_LOG('ERR_', "Invalid handle returned.");
        stream.SubmitReturn(RenderCollection::INVALID_HANDLE);
    }
}

void Node_CreateMesh(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    Core::IApplicationContext* appCtx = collection->GetAppContext();
    Mesh::MeshManager* meshManager = appCtx->GetMeshManager();
    Mesh::MeshRef newMesh = meshManager->CreateMeshNode();
    RenderCollection::CollectionHandle handle = RenderCollection::AddResource<Mesh::Mesh>(collection, newMesh);
    stream.SubmitReturn(handle);
}

void Node_CreateMeshGenerator(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    Core::IApplicationContext* appCtx = collection->GetAppContext();
    Mesh::MeshManager* meshManager = appCtx->GetMeshManager();

    //get the input
    const char* name = stream.NextBsStringArgument();

    //create new mesh generator
    Mesh::MeshGeneratorRef meshGenerator = meshManager->CreateMeshGeneratorNode(name);
    RenderCollection::CollectionHandle handle = RenderCollection::INVALID_HANDLE;
    if (meshGenerator != nullptr)
    {
        handle = RenderCollection::AddResource<Mesh::MeshGenerator>(collection, meshGenerator);
    }
    stream.SubmitReturn(handle);
}

void Node_CreateMeshOperator(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    Core::IApplicationContext* appCtx = collection->GetAppContext();
    Mesh::MeshManager* meshManager = appCtx->GetMeshManager();

    //get the input
    const char* name = stream.NextBsStringArgument();

    //create new mesh generator
    Mesh::MeshOperatorRef meshOperator = meshManager->CreateMeshOperatorNode(name);
    RenderCollection::CollectionHandle handle = RenderCollection::INVALID_HANDLE;
    if (meshOperator != nullptr)
    {
        handle = RenderCollection::AddResource<Mesh::MeshOperator>(collection, meshOperator);
    }
    stream.SubmitReturn(handle);
    
}


/////////////////////////////////////////////////////////////
//!> Render functions
/////////////////////////////////////////////////////////////
void Render_CreateUniformBuffer(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "CreateUniformBuffer", PERMISSIONS_RENDER_API_CALL);

    int& bufferSize = stream.NextArgument<int>();

    if ((bufferSize & 15) != 0)
    {
        PG_LOG('ERR_', "Error: cannot create buffer with unaligend size. Size must be 16 byte aligned.");
        stream.SubmitReturn( RenderCollection::INVALID_HANDLE ); 
    }
    else
    {
        Render::BufferRef buffer = Render::CreateUniformBuffer(bufferSize);
        stream.SubmitReturn( RenderCollection::AddResource<Render::Buffer>(renderCollection, buffer));

    }
}

void Render_SetBuffer(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == (sizeof(int)/*buffer ref*/ + sizeof(int)/*raw ram ref*/));

    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    Application::RenderCollection* collection = GetContainer(state);
    CHECK_PERMISSIONS(collection, "SetBuffer", PERMISSIONS_RENDER_API_CALL);
    int& ramRef = stream.NextArgument<int>(); //since the second parameter is a *, we can just gets its pointer in memory
    char* bufferPointer = state->Ram() + ramRef;
 
    if (handle != Application::RenderCollection::INVALID_HANDLE)
    {
        Render::BufferRef buff = RenderCollection::GetResource<Render::Buffer>(collection, handle);
        Render::SetBuffer(buff, bufferPointer);
    }
    else
    {
        PG_LOG('ERR_', "Trying to set an undefined buffer.");
    }
   
}

void Render_GetUniformLocation(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "GetUniformLocation", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& programId = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (programId != RenderCollection::INVALID_HANDLE)
    {
        Shader::ProgramLinkageRef program = RenderCollection::GetResource<Shader::ProgramLinkage>(renderCollection, programId);
        const char* uniformNameStr = stream.NextBsStringArgument();

        //this is more confusing but avoids an extra memory copy, contrasted if we were using stream.SubmitReturn
        PG_ASSERT(context.GetOutputBufferSize() == sizeof(Render::Uniform));
        Render::Uniform* outUniform = static_cast<Render::Uniform*>(context.GetRawOutputBuffer());
        Render::GetUniformLocation(program, uniformNameStr, *outUniform);
    }
    else
    {
        PG_LOG('ERR_', "Program passed for GetUniformLocation is invalid");
    }
}

void Render_SetUniformBuffer(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == (sizeof(Render::Uniform) + sizeof(int)));
    
    FunParamStream stream(context);
    BsVmState * state = context.GetVmState();
    Render::Uniform& uniform  = stream.NextArgument<Render::Uniform>(); 
    int& bufferHandle = stream.NextArgument<int>();
    Application::RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetUniformBuffer", PERMISSIONS_RENDER_API_CALL);

    if (bufferHandle != Application::RenderCollection::INVALID_HANDLE)
    {
        Render::BufferRef buffer = RenderCollection::GetResource<Render::Buffer>(renderCollection, bufferHandle);
        bool res = Render::SetUniformBuffer(uniform, buffer);
        if (!res)
        {
            PG_LOG('ERR_', "Error setting uniform. Check that uniform exists and that program is set.");
        }
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid buffer");
    }
}

void Render_SetUniformTexture(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState * state = context.GetVmState();
    Render::Uniform& uniform  = stream.NextArgument<Render::Uniform>(); 
    RenderCollection::CollectionHandle& texHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    Application::RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetUniformTexture", PERMISSIONS_RENDER_API_CALL);

    if (texHandle != Application::RenderCollection::INVALID_HANDLE)
    {
        Texture::TextureRef texture = RenderCollection::GetResource<Texture::Texture>(renderCollection, texHandle);
        bool res = Render::SetUniformTexture(uniform, texture);
        if (!res)
        {
            PG_LOG('ERR_', "Error setting uniform texture. Check that uniform exists and that program is set.");
        }
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid texture");
    }
}

void Render_SetUniformTextureRenderTarget(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState * state = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(state);
    Render::Uniform& uniform = stream.NextArgument<Render::Uniform>();
    RenderCollection::CollectionHandle& renderTargetId = stream.NextArgument<RenderCollection::CollectionHandle>();

    CHECK_PERMISSIONS(renderCollection, "SetUniformTextureRenderTarget", PERMISSIONS_RENDER_API_CALL);
    if (renderTargetId != Application::RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTargetRef renderTarget = RenderCollection::GetResource<Render::RenderTarget>(renderCollection, renderTargetId);
        Render::SetUniformTextureRenderTarget(uniform, renderTarget);
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid render target");
    }
    
}

void Render_SetProgram(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState * state = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetProgram", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& programId = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (programId != Application::RenderCollection::INVALID_HANDLE)
    {
        Shader::ProgramLinkageRef program = RenderCollection::GetResource<Shader::ProgramLinkage>(renderCollection, programId);
        Render::SetProgram(program);
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid program");
    }
}

void Render_SetMesh(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetMesh", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& meshId = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (meshId != Application::RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshRef mesh = RenderCollection::GetResource<Mesh::Mesh>(renderCollection, meshId);
        Render::SetMesh(mesh);
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid mesh");
    }
}

void Render_UnbindMesh(FunCallbackContext& context)
{
    Pegasus::Render::UnbindMesh();
}

void Render_UnbindComputeOutputs(FunCallbackContext& context)
{
    Pegasus::Render::UnbindComputeOutputs();
}

void Render_UnbindRenderTargets(FunCallbackContext& context)
{
    Pegasus::Render::UnbindRenderTargets();
}

void Render_UnbindComputeResources(FunCallbackContext& context)
{
    Pegasus::Render::UnbindComputeResources();
}
void Render_UnbindVertexResources(FunCallbackContext& context)
{
    Pegasus::Render::UnbindVertexResources();
}
void Render_UnbindPixelResources(FunCallbackContext& context)
{
    Pegasus::Render::UnbindPixelResources();
}

void Render_SetViewport(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    CHECK_PERMISSIONS(GetContainer(state), "SetViewport", PERMISSIONS_RENDER_API_CALL);
    Render::Viewport& viewport = stream.NextArgument<Render::Viewport>();
    Render::SetViewport(viewport);
}

void Render_SetViewport2(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    CHECK_PERMISSIONS(collection, "SetViewport", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTargetRef rt = RenderCollection::GetResource<Render::RenderTarget>(collection, handle);
        Pegasus::Render::SetViewport(rt);
    }
    else
    {
        PG_LOG('ERR_', "Invalid Render Target passed to set viewport.");
    }
}

void Render_SetViewport3(FunCallbackContext& context)
{
}

void Render_SetRenderTarget(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetRenderTarget", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& rtHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (rtHandle != RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTargetRef rt = RenderCollection::GetResource<Render::RenderTarget>(renderCollection,rtHandle);
        Render::SetRenderTarget(rt);
    }
    else
    {
        PG_LOG('ERR_', "Invalid render target being set");
    }
}
void Render_SetRenderTarget2(FunCallbackContext& context) 
{ 
    //TODO: implement depth render targets
    PG_LOG('ERR_', "Unimplemented.");
}

void Render_SetRenderTargets(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetRenderTarget", PERMISSIONS_RENDER_API_CALL);
    int targetCounts = stream.NextArgument<int>();
    int targetsOffset = stream.NextArgument<int>();
    
    if (targetCounts >= Pegasus::Render::Constants::MAX_RENDER_TARGETS)
    {
        PG_LOG('ERR_', "Can't set %i number of targets. Target number must be from 0 to %i", targetCounts, Pegasus::Render::Constants::MAX_RENDER_TARGETS);
        return;
    }

    //check the type here of the unknown pointer passed as the second parameter
    const TypeDesc* unknownType = context.GetArgExps()->GetTail()->GetExp()->GetTypeDesc();
    if (unknownType->GetModifier() != TypeDesc::M_ARRAY || 
        Utils::Strcmp(unknownType->GetChild()->GetName(), "RenderTarget")) //quick check, we should use Equals 
                                                      //but its slower. This type is guaranteed 
                                                      //to be a singleton so its quicker to compare ptrs.
    {
        PG_LOG('ERR_', "Second argument passed on SetRenderTargets must be an array of RenderTarget. Function failed.");
        return;
    }

    
    PG_ASSERT(targetsOffset < state->GetRamSize());
    char* targetsPtr = state->Ram() + targetsOffset;
    RenderCollection::CollectionHandle* handles = reinterpret_cast<RenderCollection::CollectionHandle*>(targetsPtr);

    //dump all into temp buffer
    Pegasus::Render::RenderTargetRef targets[Pegasus::Render::Constants::MAX_RENDER_TARGETS];
    for (int i = 0; i < targetCounts; ++i)
    {
        if (handles[i] != RenderCollection::INVALID_HANDLE)
        {
            PG_LOG('ERR_', "Trying to set incorrect handle in SetRenderTargets!");
            return;
        }
        else
        {
            targets[i] = RenderCollection::GetResource<Render::RenderTarget>(renderCollection, handles[i]);
        }
    }

    Pegasus::Render::SetRenderTargets(targetCounts, targets);
}

void Render_SetRenderTargets2(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* renderCollection = GetContainer(state);
    CHECK_PERMISSIONS(renderCollection, "SetRenderTargets", PERMISSIONS_RENDER_API_CALL);
    int targetCounts = stream.NextArgument<int>();
    int targetsOffset = stream.NextArgument<int>(); 
    
    if (targetCounts >= Pegasus::Render::Constants::MAX_RENDER_TARGETS)
    {
        PG_LOG('ERR_', "Can't set %i number of targets. Target number must be from 0 to %i", targetCounts, Pegasus::Render::Constants::MAX_RENDER_TARGETS);
        return;
    }

    //check the type here
    //TODO - implement functions
}

void Render_SetDefaultRenderTarget(FunCallbackContext& context)
{
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
    RenderCollection* renderCollection = GetContainer(context.GetVmState());
    CHECK_PERMISSIONS(renderCollection, "SetDefaultRenderTarget", PERMISSIONS_RENDER_API_CALL);
#endif
    Pegasus::Render::DispatchDefaultRenderTarget();
}

void Render_SetPrimitiveMode(FunCallbackContext& context)
{
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
    RenderCollection* renderCollection = GetContainer(context.GetVmState());
    CHECK_PERMISSIONS(renderCollection, "SetPrimitiveMode", PERMISSIONS_RENDER_API_CALL);
#endif
    PG_ASSERT(context.GetInputBufferSize() == sizeof(Pegasus::Render::PrimitiveMode));
    FunParamStream stream(context);
    Render::SetPrimitiveMode(stream.NextArgument<Pegasus::Render::PrimitiveMode>());
}

void Render_Clear(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == 3 * sizeof(int));
    FunParamStream stream(context);
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
    RenderCollection* renderCollection = GetContainer(context.GetVmState());
    CHECK_PERMISSIONS(renderCollection, "SetPrimitiveMode", PERMISSIONS_RENDER_API_CALL);
#endif
    bool col = stream.NextArgument<int>() != 0;
    bool depth = stream.NextArgument<int>() != 0;
    bool stencil = stream.NextArgument<int>() != 0;
    Render::Clear(col, depth, stencil);
}

void Render_SetClearColorValue(FunCallbackContext& context)
{
    FunParamStream stream(context);
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
    RenderCollection* renderCollection = GetContainer(context.GetVmState());
    CHECK_PERMISSIONS(renderCollection, "SetClearColorValue", PERMISSIONS_RENDER_API_CALL);
#endif
    Math::ColorRGBA& color = stream.NextArgument<Math::ColorRGBA>();
    Render::SetClearColorValue(color);
}

void Render_SetRasterizerState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    CHECK_PERMISSIONS(collection, "SetRasterizerState", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Render::RasterizerStateRef rasterState = RenderCollection::GetResource<Render::RasterizerState>(collection, handle);
        Render::SetRasterizerState(rasterState);
    }
    else
    {
        PG_LOG('ERR_', "Attempting to set Invalid rasterizer state");
    }
}

void Render_SetBlendingState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    CHECK_PERMISSIONS(collection, "SetBlendingState", PERMISSIONS_RENDER_API_CALL);
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Render::BlendingStateRef blendState = RenderCollection::GetResource<Render::BlendingState>(collection, handle);
        Render::SetBlendingState(blendState);
    }
    else
    {
        PG_LOG('ERR_', "Attempting to set Invalid rasterizer state");
    }
}

void Render_SetDepthClearValue(FunCallbackContext& context)
{
    FunParamStream stream(context);
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
    RenderCollection* renderCollection = GetContainer(context.GetVmState());
    CHECK_PERMISSIONS(renderCollection, "setDepthClearValue", PERMISSIONS_RENDER_API_CALL);
#endif
    float& depthClearVal = stream.NextArgument<float>();
    Render::SetDepthClearValue(depthClearVal);
}

void Render_Draw(FunCallbackContext& context)
{
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
    RenderCollection* renderCollection = GetContainer(context.GetVmState());
    CHECK_PERMISSIONS(renderCollection, "Draw", PERMISSIONS_RENDER_API_CALL);
#endif
    Render::Draw();
}

void Render_CreateRenderTarget(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* vmState = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(vmState);
    CHECK_PERMISSIONS(renderCollection, "CreateRenderTarget", PERMISSIONS_RENDER_API_CALL);
    Render::RenderTargetConfig& config = stream.NextArgument<Render::RenderTargetConfig>();
    Render::RenderTargetRef rt = Render::CreateRenderTarget(config);
    stream.SubmitReturn( RenderCollection::AddResource<Render::RenderTarget>( renderCollection, rt));
}

void Render_CreateDepthStencil(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* vmState = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(vmState);
    CHECK_PERMISSIONS(renderCollection, "CreateDepthStencil", PERMISSIONS_RENDER_API_CALL);
    Render::DepthStencilConfig& config = stream.NextArgument<Render::DepthStencilConfig>();
    Render::DepthStencilRef rt = Render::CreateDepthStencil(config);
    stream.SubmitReturn( RenderCollection::AddResource<Render::DepthStencil>(renderCollection, rt));
}

void Render_CreateRasterizerState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    CHECK_PERMISSIONS(collection, "CreateRasterizerState", PERMISSIONS_RENDER_API_CALL);
    Render::RasterizerConfig& rasterConfig = stream.NextArgument<Render::RasterizerConfig>();
    Render::RasterizerStateRef rasterState = Render::CreateRasterizerState(rasterConfig);
    stream.SubmitReturn( RenderCollection::AddResource<Render::RasterizerState>(collection, rasterState) );
}

void Render_CreateBlendingState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    CHECK_PERMISSIONS(collection, "CreateBlendingState", PERMISSIONS_RENDER_API_CALL);
    
    Render::BlendingConfig* blendConfig = static_cast<Render::BlendingConfig*>(context.GetRawOutputBuffer());
    Render::BlendingStateRef blendState = Render::CreateBlendingState(*blendConfig);
    stream.SubmitReturn( RenderCollection::AddResource<Render::BlendingState>(collection, blendState) );
}


bool PropertyGridPropertyCallback(const PropertyGrid::PropertyAccessor* accessor, const Pegasus::BlockScript::PropertyCallbackContext& context)
{
    if (accessor != nullptr)
    {
        const Pegasus::BlockScript::TypeDesc* typeDesc = context.propertyDesc->mType;
        //for enums, we use type marshalling:
        // we just copy the int value (in case of setting), and in case of getting
        if (Pegasus::BlockScript::TypeDesc::M_ENUM == typeDesc->GetModifier())
        {
            const PropertyGrid::EnumTypeInfo* info = PropertyGrid::PropertyGridManager::GetInstance().GetEnumInfo(typeDesc->GetName());
            const Utils::Vector<const PropertyGrid::BaseEnumType*>& enums = info->GetEnumerations();
            PG_ASSERT(sizeof(int) == typeDesc->GetByteSize());
            if (context.isRead)
            {
                PropertyGrid::BaseEnumType enumType;
                accessor->Read(&enumType, sizeof(PropertyGrid::BaseEnumType));
                int v = enumType.GetValue();
                *static_cast<int*>(context.destBuffer) = v;
            }
            else
            {
                int enumValue = *static_cast<const int*>(context.srcBuffer);
                int enumIndex = enumValue - 1;
                PG_ASSERT(enumIndex >= 0 && static_cast<unsigned int>(enumIndex) < enums.GetSize());
                const PropertyGrid::BaseEnumType* enumValuePtr = enums[enumIndex];
                accessor->Write(enumValuePtr, sizeof(*enumValuePtr));
            }
            return true;
        }
        else
        {
            if (context.isRead)
            {
                accessor->Read(context.destBuffer, context.propertyDesc->mType->GetByteSize());
            }
            else
            {
                accessor->Write(context.srcBuffer, context.propertyDesc->mType->GetByteSize());
            }
            return true;
        }
    }
    return false;
}

template<typename T>
bool TemplatePropertyCallback   (const Pegasus::BlockScript::PropertyCallbackContext& context)
{
    RenderCollection* collection = GetContainer(context.state);
    const PropertyGrid::PropertyAccessor* accessor = RenderCollection::GetPropAccessor<T>(collection, context.objectHandle, context.propertyDesc->mGuid);
    return PropertyGridPropertyCallback(accessor, context);
}

GlobalCache::CacheName CreateHash(const char* name, int windowId)
{
    GlobalCache::CacheName outHash;
    outHash.s.lowDword   = Pegasus::Utils::HashStr(name);
    outHash.s.highDword = static_cast<unsigned int>(windowId);
    return outHash;
}

template<typename T, bool isWindowIdUsed>
void GlobalCache_Register<T>(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* vmState = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(vmState);
    CHECK_PERMISSIONS(renderCollection, "GlobalCache_Register", PERMISSIONS_RENDER_GLOBAL_CACHE_WRITE);
    const char* name = stream.NextBsStringArgument();
    RenderCollection::CollectionHandle handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    bool isSuccess = false;
    int windowId = -1;
    if (isWindowIdUsed)
    {
        windowId = stream.NextArgument<int>();
    }

    if (handle != RenderCollection::INVALID_HANDLE)
    {
        GlobalCache::CacheName hash = CreateHash(name, windowId);
        T* resource = RenderCollection::GetResource<T>(renderCollection, handle);
        GlobalCache* gc = renderCollection->GetGlobalCache();
        PG_ASSERT(gc != nullptr);
        GlobalCache::Register<T>(gc, hash, resource);
    }
    else
    {
        PG_LOG('ERR_', "Trying to store invalid resource in cache name: \"%s\"", name);
    }
    stream.SubmitReturn<int>(isSuccess ? 1 : 0);
}

template<typename T, bool isWindowIdUsed>
void GlobalCache_Find<T>(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* vmState = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(vmState);
    CHECK_PERMISSIONS(renderCollection, "GlobalCache_Register", PERMISSIONS_RENDER_GLOBAL_CACHE_READ);
    GlobalCache* gc = renderCollection->GetGlobalCache();
    PG_ASSERT(gc != nullptr);
    const char* name = stream.NextBsStringArgument();
    int windowId = -1;
    if (isWindowIdUsed)
    {
        windowId = stream.NextArgument<int>();
    }

    GlobalCache::CacheName hash = CreateHash(name, windowId);
    RenderCollection::CollectionHandle collectionHandle = RenderCollection::ResolveResourceFromGlobalCache<T>(renderCollection, hash);
    if (collectionHandle == RenderCollection::INVALID_HANDLE)
    {
        PG_LOG('ERR_', "No global resource found, with the name of %s. Make sure is registered from the master timeline script.", name);
    }
    stream.SubmitReturn<RenderCollection::CollectionHandle>(collectionHandle);
}
