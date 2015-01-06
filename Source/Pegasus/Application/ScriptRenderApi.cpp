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
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Color.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::BlockScript;
using namespace Pegasus::Application;
using namespace Pegasus::BlockScript::Ast;
using namespace Pegasus::Math;

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
void Texture_SetOperatorInput(FunCallbackContext& context);
void Texture_SetGeneratorInput(FunCallbackContext& context);

/////Node Manager Methods////////////////////////////////////
void Node_CreateProgramLinkage(FunCallbackContext& context);
void Node_LoadShaderStage(FunCallbackContext& context);
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
void Render_SetRenderTarget(FunCallbackContext& context);
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
        "void_ptr",
        nullptr, // no child
        0, //no modifier property
        TypeDesc::E_INT //int ALU engine
    );
    ptrType->SetByteSize(sizeof(void*));


    const StructDeclarationDesc structDefs[] = {
       {  
            "Uniform",  // the size of this struct will be patched bellow, since pegasus api still
                        // does not support registration of static array members
            {"int"   , nullptr},
            {"unused", nullptr}
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
            {"BlendOperator"   , "Multiplicator", "Multiplicator", "void_ptr"     , nullptr },
            {"BlendingOperator", "Source"       , "Dest"         , "_internalData", nullptr }
        }
    };

    const int structDefSize = sizeof(structDefs) / sizeof(structDefs[0]);
    lib->CreateStructTypes(structDefs, structDefSize);

    //patch uniform type size
    TypeDesc* uniformType = lib->GetSymbolTable()->GetTypeForPatching("Uniform");
    PG_ASSERT(uniformType != nullptr);
    uniformType->SetByteSize(sizeof(Render::Uniform));
}

static void RegisterNodes(BlockLib* lib)
{
    const ClassTypeDesc nodeDefs[] = {
        {
            "Buffer",
            {}, 0 // no methods for buffer
        },
        {
            "RenderTarget",
            {}, 0
        },
        {
            "BlendingState",
            {}, 0
        },
        {
            "RasterizerState",
            {}, 0
        },
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
                { "SetGeneratorInput", "int", { "Texture", "TextureGenerator", nullptr }, { "this", "texGenerator", nullptr }, Texture_SetGeneratorInput },
                { "SetOperatorInput",  "int", { "Texture", "TextureOperator", nullptr },  { "this", "texOperator", nullptr }, Texture_SetOperatorInput }
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
        // Node constructors
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
            "SetRenderTarget",
            "int",
            { "RenderTarget", nullptr },
            { "renderTarget", nullptr },
            Render_SetRenderTarget
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
            { nullptr },
            { nullptr },
            Render_CreateRasterizerState
        },
        {
            "CreateBlendingState",
            "BlendingState",
            { nullptr },
            { nullptr },
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
    PG_ASSERT(context.GetInputBufferSize() == 2*sizeof(RenderCollection::CollectionHandle));
    BsVmState* state = context.GetVmState();    
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    
    RenderCollection::CollectionHandle* handles = static_cast<RenderCollection::CollectionHandle*>(context.GetRawInputBuffer());
    
    if (handles[0] != RenderCollection::INVALID_HANDLE && handles[1] != RenderCollection::INVALID_HANDLE)
    {
        Mesh::MeshRef mesh = collection->GetMesh(handles[0]);
        Mesh::MeshGeneratorRef meshGeneratorRef = collection->GetMeshGenerator(handles[1]);
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
        //force shader compilation
        bool unused = false;
        shaderStage->GetUpdatedData(unused);

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
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    Wnd::IWindowContext* appCtx = collection->GetAppContext();
    Mesh::MeshManager* meshManager = appCtx->GetMeshManager();
    Mesh::MeshRef newMesh = meshManager->CreateMeshNode();
    RenderCollection::CollectionHandle handle = collection->AddMesh(newMesh);
    *static_cast<RenderCollection::CollectionHandle*>(context.GetRawOutputBuffer()) = handle;
}

void Node_CreateMeshGenerator(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(int)); //int is a memory ref to the string
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    Wnd::IWindowContext* appCtx = collection->GetAppContext();
    Mesh::MeshManager* meshManager = appCtx->GetMeshManager();

    //get the input
    int stringRef = *static_cast<int*>(context.GetRawInputBuffer());
    BsVmState::HeapElement& heapEl = state->GetHeapElement(stringRef);
    char* name = static_cast<char*>(heapEl.mObject);

    //create new mesh generator
    Mesh::MeshGeneratorRef meshGenerator = meshManager->CreateMeshGeneratorNode(name);
    RenderCollection::CollectionHandle handle = collection->AddMeshGenerator(meshGenerator);
    *static_cast<RenderCollection::CollectionHandle*>(context.GetRawOutputBuffer()) = handle;
    
}

/////////////////////////////////////////////////////////////
//!> Render functions
/////////////////////////////////////////////////////////////
void Render_CreateUniformBuffer(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();

    PG_ASSERT(context.GetInputBufferSize() == sizeof(int));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(int));

    int* inputBuffer = static_cast<int*>(context.GetRawInputBuffer()); 
    int* outputBuffer = static_cast<int*>(context.GetRawOutputBuffer());
    int bufferSize = inputBuffer[0];

    if ((bufferSize & 15) != 0)
    {
        PG_LOG('ERR_', "Error: cannot create buffer with unaligend size. Size must be 16 byte aligned.");
        outputBuffer[0] = Application::RenderCollection::INVALID_HANDLE; 
    }
    else
    {
     
        Render::Buffer buffer;
        Render::CreateUniformBuffer(bufferSize, buffer);
        Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
        outputBuffer[0] = renderCollection->AddBuffer(buffer);

    }
}

void Render_SetBuffer(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == (sizeof(int)/*buffer ref*/ + sizeof(int)/*raw ram ref*/));

    BsVmState* state = context.GetVmState();
    
    int* inputBuffers = static_cast<int*>(context.GetRawInputBuffer());
    int bufferRef = inputBuffers[0];
    int ramRef = inputBuffers[1];
    char* bufferPointer = state->Ram() + ramRef;

   
 
    if (bufferRef != Application::RenderCollection::INVALID_HANDLE)
    {
        Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
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
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
    int programId = static_cast<int*>(context.GetRawInputBuffer())[0];
    if (programId != -1)
    {
        Shader::ProgramLinkageRef program = renderCollection->GetProgram(programId);
        int uniformNameRef = static_cast<int*>(context.GetRawInputBuffer())[1];
        const char* uniformNameStr = static_cast<const char*>(state->GetHeapElement(uniformNameRef).mObject);
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

    BsVmState * state = context.GetVmState();
    Render::Uniform* uniform  = static_cast<Render::Uniform*>(context.GetRawInputBuffer()); 
    int bufferHandle = reinterpret_cast<int*>(static_cast<char*>(context.GetRawInputBuffer()) + sizeof(Render::Uniform))[0];

    if (bufferHandle != Application::RenderCollection::INVALID_HANDLE)
    {
        Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
        Render::Buffer* buffer = renderCollection->GetBuffer(bufferHandle);
        bool res = Render::SetUniformBuffer(*uniform, *buffer);
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
    PG_ASSERT(context.GetInputBufferSize() == (sizeof(Render::Uniform) + sizeof(int)));

    BsVmState * state = context.GetVmState();
    Render::Uniform* uniform  = static_cast<Render::Uniform*>(context.GetRawInputBuffer()); 
    int texHandle = reinterpret_cast<int*>(static_cast<char*>(context.GetRawInputBuffer()) + sizeof(Render::Uniform))[0];

    if (texHandle != Application::RenderCollection::INVALID_HANDLE)
    {
        Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
        Texture::TextureRef texture = renderCollection->GetTexture(texHandle);
        bool res = Render::SetUniformTexture(*uniform, texture);
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
    BsVmState * state = context.GetVmState();
    Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
    PG_ASSERT(context.GetInputBufferSize() == (sizeof(Render::Uniform) + sizeof(int)));
    Render::Uniform* uniform = static_cast<Render::Uniform*>(context.GetRawInputBuffer());
    int renderTargetId = reinterpret_cast<int>(static_cast<char*>(context.GetRawInputBuffer()) + sizeof(Render::Uniform));

    if (renderTargetId != Application::RenderCollection::INVALID_HANDLE)
    {
        Render::RenderTarget* renderTarget = renderCollection->GetRenderTarget(renderTargetId);
        Render::SetUniformTextureRenderTarget(*uniform, *renderTarget);
    }
    else
    {
        PG_LOG('ERR_', "Can't set an invalid render target");
    }
    
}

void Render_SetProgram(FunCallbackContext& context)
{
    BsVmState * state = context.GetVmState();
    Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(int));
    int programId = (static_cast<int*>(context.GetRawInputBuffer()))[0];
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
    PG_ASSERT(context.GetInputBufferSize() == sizeof(int));
    BsVmState* state = context.GetVmState();
    Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(state->GetUserContext());
    int meshId = (static_cast<int*>(context.GetRawInputBuffer()))[0];
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

void Render_SetRenderTarget(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    RenderCollection* renderCollection = static_cast<RenderCollection*>(state->GetUserContext());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    RenderCollection::CollectionHandle rtHandle = *static_cast<RenderCollection::CollectionHandle*>(context.GetRawInputBuffer());
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

void Render_SetDefaultRenderTarget(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    RenderCollection* renderCollection = static_cast<RenderCollection*>(context.GetRawInputBuffer());
    
}

void Render_Clear(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == 3 * sizeof(int));
    int* input = static_cast<int*>(context.GetRawInputBuffer());
    Render::Clear(input[0] == 1, input[1] == 1, input[2] == 1);
}

void Render_SetClearColorValue(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(Math::Vec4));
    Math::ColorRGBA* color = static_cast<Math::ColorRGBA*>(context.GetRawInputBuffer());
    Render::SetClearColorValue(*color);
}

void Render_SetRasterizerState(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    RenderCollection::CollectionHandle handle = *static_cast<RenderCollection::CollectionHandle*>(context.GetRawInputBuffer());
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
    PG_ASSERT(context.GetInputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    RenderCollection::CollectionHandle handle = *static_cast<RenderCollection::CollectionHandle*>(context.GetRawInputBuffer());
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
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    Render::SetDepthClearValue((static_cast<float*>(context.GetRawInputBuffer()))[0]);
}

void Render_Draw(FunCallbackContext& context)
{
    Render::Draw();
}

void Render_CreateRenderTarget(FunCallbackContext& context)
{
    BsVmState* vmState = context.GetVmState();
    Application::RenderCollection* renderCollection = static_cast<Application::RenderCollection*>(vmState->GetUserContext());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(Render::RenderTargetConfig));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(int));
    Render::RenderTargetConfig* config = static_cast<Render::RenderTargetConfig*>(context.GetRawInputBuffer());
    Render::RenderTarget rt;
    Render::CreateRenderTarget(*config, rt);
    Application::RenderCollection::CollectionHandle rtHandle = renderCollection->AddRenderTarget(rt);
    *static_cast<Application::RenderCollection::CollectionHandle*>(context.GetRawOutputBuffer()) = rtHandle;
    
}

void Render_CreateRasterizerState(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(Render::RasterizerConfig));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    
    Render::RasterizerConfig* rasterConfig = static_cast<Render::RasterizerConfig*>(context.GetRawInputBuffer());
    Render::RasterizerState rasterState;
    Render::CreateRasterizerState(*rasterConfig, rasterState);

    RenderCollection::CollectionHandle handle = collection->AddRasterizerState(rasterState);
    *static_cast<RenderCollection::CollectionHandle*>(context.GetRawOutputBuffer()) = handle;
}

void Render_CreateBlendingState(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    RenderCollection* collection = static_cast<RenderCollection*>(state->GetUserContext());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(Render::BlendingConfig));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(RenderCollection::CollectionHandle));
    
    Render::BlendingConfig* blendConfig = static_cast<Render::BlendingConfig*>(context.GetRawOutputBuffer());
    Render::BlendingState blendState;
    Render::CreateBlendingState(*blendConfig, blendState);
    
    RenderCollection::CollectionHandle handle = collection->AddBlendingState(blendState);
    *static_cast<RenderCollection::CollectionHandle*>(context.GetRawOutputBuffer()) = handle;
}

