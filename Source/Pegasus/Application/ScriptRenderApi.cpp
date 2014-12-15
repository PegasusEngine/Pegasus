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
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Application/ScriptRenderApi.h"
#include "Pegasus/Window/IWindowContext.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Window/IWindowContext.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

static void RegisterTypes        (BlockLib* lib);
static void RegisterFunctions    (BlockLib* lib);

///////////////////////////////////////////////////////////////////////////////////
//! Forward declaration of API function wrappers
///////////////////////////////////////////////////////////////////////////////////

////Program Methods//////////////////////////////////////////
void Program_SetShaderStage(FunCallbackContext& context);

////Mesh Methods/////////////////////////////////////////////
void MeshGenerator_SetGeneratorInput(FunCallbackContext& context);

/////Texture Methods/////////////////////////////////////////
void TextureGenerator_AddOperatorInput(FunCallbackContext& context);
void TextureGenerator_AddGeneratorInput(FunCallbackContext& context);
void TextureOperator_AddOperatorInput(FunCallbackContext& context);
void TextureOperator_AddGeneratorInput(FunCallbackContext& context);
void Texture_AddOperatorInput(FunCallbackContext& context);
void Texture_AddGeneratorInput(FunCallbackContext& context);

/////Node Manager Methods////////////////////////////////////
void Node_CreateProgramLinkage(FunCallbackContext& context);
void Node_LoadShaderStage(FunCallbackContext& context);
void Node_CreateTexture(FunCallbackContext& context);
void Node_CreateTextureGenerator(FunCallbackContext& context);
void Node_CreateTextureOperator(FunCallbackContext& context);
void Node_CreateMesh(FunCallbackContext& context);
void Node_CreateMeshGenerator(FunCallbackContext& context);



///////////////////////////////////////////////////////////////////////////////////
//! Node / Render API registration functions. These are pure metadata attachments to
//! the blockscript runtime lib
///////////////////////////////////////////////////////////////////////////////////

void Pegasus::Application::RegisterRenderApi(BlockScript::BlockLib* rtLib)
{
    RegisterTypes(rtLib);
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
    TypeDesc* ptrType = lib->GetSymbolTable()->CreateType(
        TypeDesc::M_SCALAR,
        "void_ptr"
    );
    ptrType->SetByteSize(sizeof(void*));

    const StructDeclarationDesc structDefs[] = {
        { 
            "Uniform", 
            {"string", "int"           , "int"           , "int"             , nullptr},
            {"Name"  , "_internalIndex", "_internalOwner", "_internalVersion", nullptr}
        },
        {
            "Buffer",
            {"int"  ,  "void_ptr"    , nullptr},
            {"Size" , "_internalData", nullptr}
        },
        {
            "RenderTargetConfig",
            {"int"  , "int"   , nullptr },
            {"Width", "Height", nullptr }
        },
        {
            "RenderTarget",
            {"RenderTargetConfig", "void_ptr"     , nullptr},
            {"Config"            , "_internalData", nullptr } 
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
            "RasterizerState",
            {"RasterizerConfig", "void_ptr"     , "void_ptr"         ,  nullptr },
            {"Config"          , "_internalData", "_internalDataAux" ,  nullptr }
        },
        {
            "BlendingConfig",
            {"BlendOperator"   , "Multiplicator", "Multiplicator", "void_ptr"     , nullptr },
            {"BlendingOperator", "Source"       , "Dest"         , "_internalData", nullptr }
        },
        {
            "BlendingState",
            {"BlendingConfig", "void_ptr"     , nullptr},
            {"Config"        , "_internalData", nullptr }
        }
    };

    const int structDefSize = sizeof(structDefs) / sizeof(structDefs[0]);
    lib->CreateStructTypes(structDefs, structDefSize);
}

static void RegisterNodes(BlockLib* lib)
{
    const ClassTypeDesc nodeDefs[] = {
        {
            "ShaderStage",
            {}, 0 //no methods
        },
        {
            "ProgramLinkage",
            { //method list
                { "SetShaderStage", "int", {"ProgramLinkage", "ShaderStage", nullptr}, {"this", "stage", nullptr}, Program_SetShaderStage }
            },
            1
        },
        {
            "MeshGenerator",
            {},0
        },
        {
            "Mesh",
            {
                { "SetGeneratorInput", "int", {"Mesh", "MeshGenerator", nullptr}, {"this", "meshGenerator", nullptr}, MeshGenerator_SetGeneratorInput }
            },
            1
        },
        {
            "TextureGenerator",
            {}, 0
        },
        {
            "TextureOperator",
            {
                { "AddGeneratorInput", "int", { "TextureOperator", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, TextureOperator_AddGeneratorInput },
                { "AddOperatorInput",  "int", { "TextureOperator", "TextureOperator", nullptr },  { "this", "texOperator", nullptr },  TextureOperator_AddOperatorInput  }
            },
            2
        },
        {
            "Texture",
            {
                { "AddGeneratorInput", "int", { "Texture", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, Texture_AddGeneratorInput },
                { "AddOperatorInput",  "int", { "Texture", "TextureOperator", nullptr },  { "this", "texOperator", nullptr }, Texture_AddOperatorInput }
            },
            2
        }
    };
    lib->CreateClassTypes(nodeDefs, sizeof(nodeDefs)/sizeof(nodeDefs[0]));
}

static void RegisterTypes(BlockLib* lib)
{
    RegisterRenderEnums(lib);
    RegisterRenderStructs(lib);
    RegisterNodes(lib);
}

static void RegisterFunctions(BlockLib* lib)
{
    const FunctionDeclarationDesc funDeclarations[] = {
        {
            "CreateProgramLinkage",
            "ProgramLinkage",
            {"string",nullptr},
            {"name"  ,nullptr},
            Node_CreateProgramLinkage
        },
        {
            "LoadShader",
            "ShaderStage",
            { "string", nullptr },
            { "path", nullptr },
            Node_LoadShaderStage
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
            Node_CreateMesh
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

/////////////////////////////////////////////////////////////
//!> Program Node functions
/////////////////////////////////////////////////////////////
void Program_SetShaderStage(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* container = GetContainer(state);    

    
    int* output = static_cast<int*>(context.GetRawOutputBuffer());
    Application::RenderCollection::CollectionHandle programId = *static_cast<int*>(context.GetRawInputBuffer());
    Application::RenderCollection::CollectionHandle stageId   = *(static_cast<int*>(context.GetRawInputBuffer()) + 1);
    if (programId != Application::RenderCollection::INVALID_HANDLE && stageId != Application::RenderCollection::INVALID_HANDLE)
    {
        PG_ASSERT(programId >= 0 && programId < container->GetProgramCount());
        PG_ASSERT(stageId   >= 0 && stageId < container->GetShaderCount());
        Shader::ShaderStageRef currShader = container->GetShader(stageId);
        container->GetProgram(programId)->SetShaderStage(currShader);
        *output = 1;
    }
    else
    {
        *output = 0;
    }
}

/////////////////////////////////////////////////////////////
//!> Mesh Node functions
/////////////////////////////////////////////////////////////
void MeshGenerator_SetGeneratorInput(FunCallbackContext& context)
{
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

void Texture_AddOperatorInput(FunCallbackContext& context)
{
}

void Texture_AddGeneratorInput(FunCallbackContext& context)
{
}

/////////////////////////////////////////////////////////////
//!> Node Manager functions
/////////////////////////////////////////////////////////////
void Node_CreateProgramLinkage(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* container = GetContainer(state);

    int* inptStr = static_cast<int*>(context.GetRawInputBuffer());
    const char* name = static_cast<const char*>(state->GetHeapElement(*inptStr).mObject);
    Shader::ProgramLinkageRef prog = container->GetAppContext()->GetShaderManager()->CreateProgram(name);
    Application::RenderCollection::CollectionHandle* output = static_cast<int*>(context.GetRawOutputBuffer());
    *output = container->AddProgram(&(*prog));
}

void Node_LoadShaderStage(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* container = GetContainer(state);

    int* inptStr = static_cast<int*>(context.GetRawInputBuffer());
    const char* path = static_cast<const char*>(state->GetHeapElement(*inptStr).mObject);

    Shader::ShaderStageFileProperties fileProps;
    fileProps.mPath = path;
    fileProps.mLoader = container->GetAppContext()->GetIOManager();
    fileProps.mUserData = nullptr;

    Shader::ShaderStageRef shaderStage = container->GetAppContext()->GetShaderManager()->LoadShaderStageFromFile(fileProps);
    Application::RenderCollection::CollectionHandle* output = static_cast<int*>(context.GetRawOutputBuffer());
    if (shaderStage != nullptr)
    {
        *output = container->AddShader(&(*shaderStage));
    }
    else
    {
        *output = Application::RenderCollection::INVALID_HANDLE; //an invalid id
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
}

void Node_CreateMeshGenerator(FunCallbackContext& context)
{
}

