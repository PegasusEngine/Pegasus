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
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/IddStrPool.h"
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

////Utility methods//////////////////////////////////////////
void Util_GetWidthHeightAspect(FunCallbackContext& context);

////Program Methods//////////////////////////////////////////
void Program_SetShaderStage(FunCallbackContext& context);

////Mesh Methods/////////////////////////////////////////////
void MeshGenerator_SetGeneratorInput(FunCallbackContext& context);

/////Texture Methods/////////////////////////////////////////
void TextureGenerator_AddOperatorInput(FunCallbackContext& context);
void TextureGenerator_AddGeneratorInput(FunCallbackContext& context);
void TextureOperator_AddOperatorInput(FunCallbackContext& context);
void TextureOperator_AddGeneratorInput(FunCallbackContext& context);
void Texture_SetOperatorInput(FunCallbackContext& context);
void Texture_SetGeneratorInput(FunCallbackContext& context);

/////Node Manager Methods////////////////////////////////////
void Node_LoadProgram(FunCallbackContext& context);
void Node_CreateTexture(FunCallbackContext& context);
void Node_CreateTextureGenerator(FunCallbackContext& context);
void Node_CreateTextureOperator(FunCallbackContext& context);
void Node_CreateMesh(FunCallbackContext& context);
void Node_CreateMeshGenerator(FunCallbackContext& context);

/////Render API Functions////////////////////////////////////
void Render_CreateUniformBuffer(FunCallbackContext& context);
void Render_SetBuffer(FunCallbackContext& context);
void Render_GetUniformLocation(FunCallbackContext& context);
void Render_SetUniformBuffer(FunCallbackContext& context);
void Render_SetUniformTexture(FunCallbackContext& context);
void Render_SetUniformTextureRenderTarget(FunCallbackContext& context);
void Render_SetProgram(FunCallbackContext& context);
void Render_SetMesh(FunCallbackContext& context);
void Render_SetViewport(FunCallbackContext& context);
void Render_SetViewport2(FunCallbackContext& context);
void Render_SetViewport3(FunCallbackContext& context);
void Render_SetRenderTarget(FunCallbackContext& context);
void Render_SetRenderTarget2(FunCallbackContext& context);
void Render_SetRenderTargets(FunCallbackContext& context);
void Render_SetRenderTargets2(FunCallbackContext& context);
void Render_SetDefaultRenderTarget(FunCallbackContext& context);
void Render_Clear(FunCallbackContext& context);
void Render_SetClearColorValue(FunCallbackContext& context);
void Render_SetRasterizerState(FunCallbackContext& context);
void Render_SetBlendingState(FunCallbackContext& context);
void Render_SetDepthClearValue(FunCallbackContext& context);
void Render_Draw(FunCallbackContext& context);
void Render_CreateRenderTarget(FunCallbackContext& context);
void Render_CreateRasterizerState(FunCallbackContext& context);
void Render_CreateBlendingState(FunCallbackContext& context);

// property callback functions
bool MeshOperatorPropertyCallback    (const Pegasus::BlockScript::PropertyCallbackContext& callback);
bool MeshGeneratorPropertyCallback   (const Pegasus::BlockScript::PropertyCallbackContext& callback);
bool TextureOperatorPropertyCallback (const Pegasus::BlockScript::PropertyCallbackContext& callback);
bool TextureGeneratorPropertyCallback(const Pegasus::BlockScript::PropertyCallbackContext& callback);

/////  Declaration of all Texture node properties  /////
struct CoreClassProperties
{
    CoreClassProperties()
    : mName(nullptr) {} 

    const char* mName;
    Utils::Vector<Pegasus::BlockScript::ObjectPropertyDesc> mPropertiesDescs;
#if PEGASUS_ENABLE_ASSERT
    Utils::Vector<const PropertyGrid::PropertyGridClassInfo::PropertyRecord*> mSanityCheckProperties;
#endif
};

const char* gPropertyBlockscriptTypeNames[PropertyGrid::NUM_PROPERTY_TYPES] =
{
    "int", //PROPERTYTYPE_BOOL = 0,
    "int", //PROPERTYTYPE_INT,
    "int", //PROPERTYTYPE_UINT,

    "float",//PROPERTYTYPE_FLOAT,
    "float2",//PROPERTYTYPE_VEC2,
    "float3",//PROPERTYTYPE_VEC3,
    "float4",//PROPERTYTYPE_VEC4,

    "float3",//PROPERTYTYPE_COLOR8RGB,
    "float4",//PROPERTYTYPE_COLOR8RGBA,

    "string"//PROPERTYTYPE_STRING64,
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
            },
            BlendingConfig::COUNT_M
        }
    };
    
    lib->CreateEnumTypes(enumDefs, sizeof(enumDefs)/sizeof(enumDefs[0]));
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
            {"int"  , "int"   , nullptr },
            {"Width", "Height", nullptr }
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
            for (int baseClassPoolIt = 0; baseClassPoolIt < outCoreClasses.GetSize(); ++baseClassPoolIt)
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
            for (unsigned propIt = 0; propIt < classInfo->GetNumClassProperties(); ++propIt)
            {
                int uniqueId = targetBaseClassProps->mPropertiesDescs.GetSize();
                Pegasus::BlockScript::ObjectPropertyDesc& objPropDesc = targetBaseClassProps->mPropertiesDescs.PushEmpty(); 
                const PropertyGrid::PropertyGridClassInfo::PropertyRecord& record = classInfo->GetClassProperty(propIt);
    #if PEGASUS_ENABLE_ASSERT
                targetBaseClassProps->mSanityCheckProperties.PushEmpty() = &record;
    #endif
                objPropDesc.propertyTypeName = gPropertyBlockscriptTypeNames[record.type];
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
            "DepthStencilTarget",
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
            MeshGeneratorPropertyCallback
        },
        {
            "Mesh",
            {
                { "SetGeneratorInput", "int", {"Mesh", "MeshGenerator", nullptr}, {"this", "meshGenerator", nullptr}, MeshGenerator_SetGeneratorInput }
            },
            1,
            nullptr, 0, nullptr
        },
        {
            "TextureGenerator",
            {}, 0,
            nullptr, 0,
            TextureGeneratorPropertyCallback
        },
        {
            "TextureOperator",
            {
                { "AddGeneratorInput", "int", { "TextureOperator", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, TextureOperator_AddGeneratorInput },
                { "AddOperatorInput",  "int", { "TextureOperator", "TextureOperator", nullptr },  { "this", "texOperator", nullptr },  TextureOperator_AddOperatorInput  }
            },
            2,
            nullptr, 0, TextureOperatorPropertyCallback
        },
        {
            "Texture",
            {
                { "SetGeneratorInput", "int", { "Texture", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, Texture_SetGeneratorInput },
                { "SetOperatorInput",  "int", { "Texture", "TextureOperator", nullptr },  { "this", "texOperator", nullptr }, Texture_SetOperatorInput }
            },
            2,
            nullptr, 0, nullptr
        }
    };

    const int nodeDefsSize = sizeof(nodeDefs)/sizeof(nodeDefs[0]);

    //gather the properties and accumulate them on the base classes
    Utils::Vector<CoreClassProperties> coreClasses; 
    LinearizeProperties(coreClasses, propGridMgr, nodeDefs, nodeDefsSize);

    //patch the node Defs with the appropiate property lists gathered
    for (int i = 0; i < nodeDefsSize; ++i)
    {
        ClassTypeDesc& desc = nodeDefs[i];
        
        for (int d = 0; d < coreClasses.GetSize(); ++d)
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
    for (int i = 0; i < coreClasses.GetSize(); ++i)
    {
        CoreClassProperties& coreClassProp = coreClasses[i];
        const TypeDesc* blockScriptType = lib->GetSymbolTable()->GetTypeByName(coreClassProp.mName);
        if (blockScriptType == nullptr) continue; //means is a class we are not interested in but uses the property grid
        PG_ASSERTSTR(blockScriptType->GetModifier() == TypeDesc::M_REFERECE, "Type registered on blockscript is not a c++ class");
        
        for (int p = 0; p < coreClassProp.mSanityCheckProperties.GetSize(); ++p)
        {
            const PropertyGrid::PropertyGridClassInfo::PropertyRecord* record = coreClassProp.mSanityCheckProperties[p];
            //linearly walk over the node property link list, find the property and check sizes
            const PropertyNode* propNode = blockScriptType->GetPropertyNode();
            while (propNode != nullptr && Utils::Strcmp(propNode->mName, record->name))
            {
                propNode = propNode->mNext;
            }

            PG_ASSERTSTR(propNode != nullptr, "Property node was not registered!");
            PG_ASSERTSTR(
                record->size <= propNode->mType->GetByteSize() , 
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
    RegisterNodes(lib, context);
}

static void RegisterFunctions(BlockLib* lib)
{
    const FunctionDeclarationDesc funDeclarations[] = {
        //Misc utils
        {
            "GetWidthHeightAspect",
            "float4",
            { nullptr },
            { nullptr },
            Util_GetWidthHeightAspect
        },
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
            { "DepthStencilTarget", nullptr },
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
            { "RenderTarget", "DepthStencilTarget", nullptr },
            { "renderTarget", "depthStencilTarget", nullptr },
            Render_SetRenderTarget2
        },
        {
            "SetRenderTargets",
            "int",
            { "int"               , "*"              , "DepthStencilTarget", nullptr },
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

////Utility methods//////////////////////////////////////////
void Util_GetWidthHeightAspect(FunCallbackContext& context)
{
    Application::RenderCollection* container = GetContainer(context.GetVmState());
    Wnd::Window* w = container->GetWindow();
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float) * 4);
    float* widthheightaspect = static_cast<float*>(context.GetRawOutputBuffer());
    widthheightaspect[0] = static_cast<float>(w->GetWidth());
    widthheightaspect[1] = static_cast<float>(w->GetHeight());
    widthheightaspect[2] = w->GetRatio();
    widthheightaspect[3] = w->GetRatioInv();
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
        PG_ASSERT(programId >= 0 && programId < container->GetProgramCount());
        PG_ASSERT(stageId   >= 0 && stageId < container->GetShaderCount());
        Shader::ShaderStageRef currShader = container->GetShader(stageId);
        container->GetProgram(programId)->SetShaderStage(currShader);
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
void MeshGenerator_SetGeneratorInput(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();    
    RenderCollection* collection = GetContainer(state);
    FunParamStream stream(context);
    
    RenderCollection::CollectionHandle& meshHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle& genHandle  = stream.NextArgument<RenderCollection::CollectionHandle>();
    
    if ( meshHandle != RenderCollection::INVALID_HANDLE && genHandle != RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshRef mesh = collection->GetMesh(meshHandle);
        Mesh::MeshGeneratorRef meshGeneratorRef = collection->GetMeshGenerator(genHandle);
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
void TextureGenerator_AddOperatorInput(FunCallbackContext& context)
{
}

void TextureGenerator_AddGeneratorInput(FunCallbackContext& context)
{
}

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

    const char* path = stream.NextBsStringArgument();

    Shader::ProgramLinkageRef program = container->GetAppContext()->GetShaderManager()->LoadProgram(path);
    if (program != nullptr)
    {
        //force shader compilation
        bool unused = false;
        program->GetUpdatedData(unused);

        stream.SubmitReturn( container->AddProgram(&(*program)) );
    }
    else
    {
        stream.SubmitReturn( Application::RenderCollection::INVALID_HANDLE ); //an invalid id
    }
}

void Node_CreateTexture(FunCallbackContext& context)
{
}

void Node_CreateTextureGenerator(FunCallbackContext& context)
{
}

void Node_CreateTextureOperator(FunCallbackContext& context)
{
}

void Node_CreateMesh(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    Core::IApplicationContext* appCtx = collection->GetAppContext();
    Mesh::MeshManager* meshManager = appCtx->GetMeshManager();
    Mesh::MeshRef newMesh = meshManager->CreateMeshNode();
    RenderCollection::CollectionHandle handle = collection->AddMesh(newMesh);
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
    RenderCollection::CollectionHandle handle = collection->AddMeshGenerator(meshGenerator);
    stream.SubmitReturn(handle);
    
}

/////////////////////////////////////////////////////////////
//!> Render functions
/////////////////////////////////////////////////////////////
void Render_CreateUniformBuffer(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();

    int& bufferSize = stream.NextArgument<int>();

    if ((bufferSize & 15) != 0)
    {
        PG_LOG('ERR_', "Error: cannot create buffer with unaligend size. Size must be 16 byte aligned.");
        stream.SubmitReturn( RenderCollection::INVALID_HANDLE ); 
    }
    else
    {
     
        Application::RenderCollection* renderCollection = GetContainer(state);
        Render::Buffer buffer;
        Render::CreateUniformBuffer(bufferSize, buffer);
        stream.SubmitReturn( renderCollection->AddBuffer(buffer));

    }
}

void Render_SetBuffer(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == (sizeof(int)/*buffer ref*/ + sizeof(int)/*raw ram ref*/));

    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    
    RenderCollection::CollectionHandle& bufferRef = stream.NextArgument<RenderCollection::CollectionHandle>();
    int& ramRef = stream.NextArgument<int>(); //since the second parameter is a *, we can just gets its pointer in memory
    char* bufferPointer = state->Ram() + ramRef;
 
    if (bufferRef != Application::RenderCollection::INVALID_HANDLE)
    {
        Application::RenderCollection* renderCollection = GetContainer(state);
        Render::Buffer* buff = renderCollection->GetBuffer(bufferRef);
        Render::SetBuffer(*buff, bufferPointer);
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
    RenderCollection::CollectionHandle& programId = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (programId != RenderCollection::INVALID_HANDLE)
    {
        Shader::ProgramLinkageRef program = renderCollection->GetProgram(programId);
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

    if (bufferHandle != Application::RenderCollection::INVALID_HANDLE)
    {
        Application::RenderCollection* renderCollection = GetContainer(state);
        Render::Buffer* buffer = renderCollection->GetBuffer(bufferHandle);
        bool res = Render::SetUniformBuffer(uniform, *buffer);
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

    if (texHandle != Application::RenderCollection::INVALID_HANDLE)
    {
        Application::RenderCollection* renderCollection = GetContainer(state);
        Texture::TextureRef texture = renderCollection->GetTexture(texHandle);
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

    if (renderTargetId != Application::RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTarget* renderTarget = renderCollection->GetRenderTarget(renderTargetId);
        Render::SetUniformTextureRenderTarget(uniform, *renderTarget);
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
    RenderCollection::CollectionHandle& programId = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (programId != Application::RenderCollection::INVALID_HANDLE)
    {
        Shader::ProgramLinkageRef program = renderCollection->GetProgram(programId);
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
    RenderCollection::CollectionHandle& meshId = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (meshId != Application::RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshRef mesh = renderCollection->GetMesh(meshId);
        Render::SetMesh(mesh);
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid mesh");
    }
}

void Render_SetViewport(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    Render::Viewport& viewport = stream.NextArgument<Render::Viewport>();
    Render::SetViewport(viewport);
}

void Render_SetViewport2(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTarget* rt = collection->GetRenderTarget(handle);
        Pegasus::Render::SetViewport(*rt);
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
    RenderCollection::CollectionHandle& rtHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (rtHandle != RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTarget* rt = renderCollection->GetRenderTarget(rtHandle);
        Render::SetRenderTarget(*rt);
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
    Pegasus::Render::RenderTarget* targets[Pegasus::Render::Constants::MAX_RENDER_TARGETS];
    for (int i = 0; i < targetCounts; ++i)
    {
        if (handles[i] != RenderCollection::INVALID_HANDLE)
        {
            PG_LOG('ERR_', "Trying to set incorrect handle in SetRenderTargets!");
            return;
        }
        else
        {
            targets[i] = renderCollection->GetRenderTarget(handles[i]);
        }
    }

    Pegasus::Render::SetRenderTargets(targetCounts, targets);
}

void Render_SetRenderTargets2(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* renderCollection = GetContainer(state);
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
    Pegasus::Render::DispatchDefaultRenderTarget();
}

void Render_Clear(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == 3 * sizeof(int));
    FunParamStream stream(context);
    bool col = stream.NextArgument<int>() != 0;
    bool depth = stream.NextArgument<int>() != 0;
    bool stencil = stream.NextArgument<int>() != 0;
    Render::Clear(col, depth, stencil);
}

void Render_SetClearColorValue(FunCallbackContext& context)
{
    FunParamStream stream(context);
    Math::ColorRGBA& color = stream.NextArgument<Math::ColorRGBA>();
    Render::SetClearColorValue(color);
}

void Render_SetRasterizerState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Render::RasterizerState* rasterState = collection->GetRasterizerState(handle);
        Render::SetRasterizerState(*rasterState);
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
    RenderCollection::CollectionHandle& handle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (handle != RenderCollection::INVALID_HANDLE)
    {
        Render::BlendingState* blendState = collection->GetBlendingState(handle);
        Render::SetBlendingState(*blendState);
    }
    else
    {
        PG_LOG('ERR_', "Attempting to set Invalid rasterizer state");
    }
}

void Render_SetDepthClearValue(FunCallbackContext& context)
{
    FunParamStream stream(context);
    float& depthClearVal = stream.NextArgument<float>();
    Render::SetDepthClearValue(depthClearVal);
}

void Render_Draw(FunCallbackContext& context)
{
    Render::Draw();
}

void Render_CreateRenderTarget(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* vmState = context.GetVmState();
    Application::RenderCollection* renderCollection = GetContainer(vmState);
    Render::RenderTargetConfig& config = stream.NextArgument<Render::RenderTargetConfig>();
    Render::RenderTarget rt;
    Render::CreateRenderTarget(config, rt);
    stream.SubmitReturn( renderCollection->AddRenderTarget(rt));
    
}

void Render_CreateRasterizerState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
   
    Render::RasterizerConfig& rasterConfig = stream.NextArgument<Render::RasterizerConfig>();
    Render::RasterizerState rasterState;
    Render::CreateRasterizerState(rasterConfig, rasterState);

    stream.SubmitReturn( collection->AddRasterizerState(rasterState) );
}

void Render_CreateBlendingState(FunCallbackContext& context)
{
    FunParamStream stream(context);
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = GetContainer(state);
    
    Render::BlendingConfig* blendConfig = static_cast<Render::BlendingConfig*>(context.GetRawOutputBuffer());
    Render::BlendingState blendState;
    Render::CreateBlendingState(*blendConfig, blendState);
    
    stream.SubmitReturn( collection->AddBlendingState(blendState) );
}

bool MeshOperatorPropertyCallback    (const Pegasus::BlockScript::PropertyCallbackContext& context)
{
    return false;
}

bool MeshGeneratorPropertyCallback   (const Pegasus::BlockScript::PropertyCallbackContext& context)
{
    RenderCollection* collection = GetContainer(context.state);
    const PropertyGrid::PropertyAccessor* accessor = collection->GetMeshGeneratorAccessor(context.objectHandle, context.propertyDesc->mGuid);
    if (accessor != nullptr)
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
    return false;
}

bool TextureOperatorPropertyCallback (const Pegasus::BlockScript::PropertyCallbackContext& context)
{
    //TODO: implement this
    return false;
}

bool TextureGeneratorPropertyCallback(const Pegasus::BlockScript::PropertyCallbackContext& context)
{
    //TODO: implement this
    return false;
}
