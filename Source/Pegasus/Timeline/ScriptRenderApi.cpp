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

#include "Pegasus/Timeline/ScriptRenderApi.h"
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

/////////////////////////////////////////////////////////////////////////////////////////
//! Node container definitions
/////////////////////////////////////////////////////////////////////////////////////////
NodeContainer::NodeContainer(Alloc::IAllocator* allocator, Wnd::IWindowContext* context)
    : mAppContext(context),
      mPrograms(allocator),
      mShaders(allocator),
      mTextures(allocator),
      mTexturesGenerators(allocator),
      mTexturesOperators(allocator),
      mMeshes(allocator),
      mMeshesGenerators(allocator)
{
}

void NodeContainer::Clean()
{
    for (int i = 0; i < mPrograms.GetSize(); ++i)           { mPrograms[i] = nullptr; }
    for (int i = 0; i < mShaders.GetSize(); ++i)            { mShaders[i] = nullptr; }
    for (int i = 0; i < mTextures.GetSize(); ++i)           { mTextures[i] = nullptr; }
    for (int i = 0; i < mTexturesGenerators.GetSize(); ++i) { mTexturesGenerators[i] = nullptr; }
    for (int i = 0; i < mTexturesOperators.GetSize(); ++i)  { mTexturesOperators[i] = nullptr; }
    for (int i = 0; i < mMeshes.GetSize(); ++i)             { mMeshes[i] = nullptr; }
    for (int i = 0; i < mMeshesGenerators.GetSize(); ++i)   { mMeshesGenerators[i] = nullptr; }
    mPrograms.Clear();
    mShaders.Clear();
    mTextures.Clear();
    mTexturesGenerators.Clear();
    mTexturesOperators.Clear();
    mMeshes.Clear();
    mMeshesGenerators.Clear();
}

NodeContainer::~NodeContainer()
{
    Clean();
}


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

char* CopyStr(BlockLib* lib, const char* str)
{
    char* newStr = lib->GetBuilder()->GetStringPool().AllocateString();
    newStr[0] = '\0';
    PG_ASSERT(Utils::Strlen(str) + 1 < IddStrPool::sCharsPerString );
    return Utils::Strcat(newStr, str);
}

void Pegasus::Timeline::RegisterRenderApi(Pegasus::Wnd::IWindowContext* appContext)
{
    BlockScriptManager* bsMgr = appContext->GetBlockScriptManager();
    BlockLib* rtLib = bsMgr->GetRuntimeLib();
 
    RegisterTypes(rtLib);
    RegisterFunctions(rtLib);
}

static void RegisterRenderEnums(BlockLib* lib)
{
    SymbolTable* st = lib->GetSymbolTable();

    using namespace Pegasus::Render;

    const struct EnumDef {
        const char* typeName;
        const struct EnumData{
            const char* enumName;
            int enumVal;         
        } enumList[50];
        int   count;
    } enumDefs[] = {
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

    int enumDefsCount = sizeof(enumDefs) / sizeof(enumDefs[0]);
    for (int i = 0; i < enumDefsCount; ++i)
    {
        const EnumDef& def = enumDefs[i];
        TypeDesc::EnumNode* enumNode = st->NewEnumNode();
        TypeDesc::EnumNode* lastEl = enumNode;

        for (int enVal = 0; enVal < def.count; ++enVal)
        {
            lastEl->mIdd = CopyStr(lib, def.enumList[enVal].enumName);
            lastEl->mGuid = def.enumList[enVal].enumVal;
            if (enVal != def.count - 1)
            {
                lastEl->mNext = st->NewEnumNode();
                lastEl = lastEl->mNext;
            }
        }

        st->CreateType(
            TypeDesc::M_ENUM,
            CopyStr(lib, def.typeName),
            nullptr, //no child
            0, //no modifier property
            TypeDesc::E_NONE, //no ALU engine
            nullptr, //no struct def
            enumNode
        );
    }
}

static void RegisterStruct(BlockLib* lib, const char* structName, const char* const* types, const char* const* typeNames)
{
    BlockScriptBuilder* builder = lib->GetBuilder();
    SymbolTable* st = lib->GetSymbolTable();
    builder->StartNewFrame();
    
    int c = 0;
    const char* currType = types[c];
    ArgList* argList = builder->CreateArgList();
    ArgList* currArgList = argList;
    while (currType != nullptr)
    {
        const TypeDesc* typeDesc = st->GetTypeByName(currType);
        PG_ASSERTSTR(typeDesc != nullptr, "Type cannot be null! check the registration of the enums and stuff");
        ArgDec* argDec = builder->BuildArgDec(CopyStr(lib, typeNames[c]), typeDesc);

        currArgList->SetArgDec(argDec);
        currType = types[++c];

        if (currType != nullptr)
        {
            currArgList->SetTail(builder->CreateArgList());
            currArgList = currArgList->GetTail();
        }
    }

    StmtStructDef* def = builder->BuildStmtStructDef(CopyStr(lib, structName), argList);
    PG_ASSERTSTR(def != nullptr, "FATAL! could not register the structure definition %s.", structName);
}

static void RegisterRenderStructs(BlockLib* lib)
{
    TypeDesc* ptrType = lib->GetSymbolTable()->CreateType(
        TypeDesc::M_SCALAR,
        "void_ptr"
    );
    ptrType->SetByteSize(sizeof(void*));

    const struct StructDef
    {
        const char* name;
        const char* types[20];
        const char* memberNames[20];
    } structDefs[] = {
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
    for (int i = 0; i < structDefSize; ++i)
    {
        const StructDef& def = structDefs[i];
        RegisterStruct(lib, def.name, def.types, def.memberNames);
        
    }
}

static void RegisterNodes(BlockLib* lib)
{
    struct NodeDef {
        const char* name;
        struct NodeMethods {
            const char* methodName;
            const char* methodReturnType;
            FunCallback callback;
            const char* types[20];
            const char* parameterNames[20];
        } mMethods[50];
        int methodsCount;
    } nodeDefs[] = {
        {
            "ShaderStage",
            {}, 0
        },
        {
            "ProgramLinkage",
            {
                {
                    "SetShaderStage", "int", Program_SetShaderStage,
                    {"ProgramLinkage", "ShaderStage", nullptr},
                    {"this", "stage", nullptr}
                }
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
                {
                    "SetGeneratorInput", "int", MeshGenerator_SetGeneratorInput,
                    {"Mesh", "MeshGenerator", nullptr},
                    {"this", "meshGenerator", nullptr}
                },
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
                {
                    "AddGeneratorInput", "int", TextureOperator_AddGeneratorInput,
                    { "TextureOperator", "TextureGenerator", nullptr },
                    { "this", "texGenerator", nullptr }
                },
                {
                    "AddOperatorInput", "int", TextureOperator_AddOperatorInput,
                    { "TextureOperator", "TextureOperator", nullptr },
                    { "this", "texOperator", nullptr }
                },
            },
            2
        },
        {
            "Texture",
            {
                {
                    "AddGeneratorInput", "int", Texture_AddGeneratorInput,
                    { "Texture", "TextureGenerator", nullptr },
                    { "this", "texGenerator", nullptr }
                },
                {
                    "AddOperatorInput", "int", Texture_AddOperatorInput,
                    { "Texture", "TextureOperator", nullptr },
                    { "this", "texOperator", nullptr }
                },
            },
            2
        }
    };
    
    SymbolTable* st = lib->GetSymbolTable();
    int nodeDefCount = sizeof(nodeDefs) / sizeof(nodeDefs[0]);
    for (int i = 0; i < nodeDefCount; ++i)
    {
        NodeDef& def = nodeDefs[i];    
        //register the type first
        st->CreateType(
            TypeDesc::M_REFERECE,
            def.name,
            nullptr, //no child
            0, //no modifier property yet...
            TypeDesc::E_NONE, //no engine
            nullptr, //no struct definition
            nullptr // no enumeration mode
        );

        for (int m = 0; m < def.methodsCount; ++m)
        {
            int paramCount = -1;
            while(def.mMethods[m].types[++paramCount] != nullptr);
            PG_ASSERT(paramCount > 0);
            bool result = CreateIntrinsicFunction(
                lib->GetBuilder(),
                def.mMethods[m].methodName,
                def.mMethods[m].types,
                def.mMethods[m].parameterNames,
                paramCount,
                def.mMethods[m].methodReturnType,
                def.mMethods[m].callback,
                true //these are in the -> notation
            );
            PG_ASSERTSTR(result, "failed registering api");
        }
    }
}

static void RegisterTypes(BlockLib* lib)
{
    RegisterRenderEnums(lib);
    RegisterRenderStructs(lib);
    RegisterNodes(lib);
}

static void RegisterFunctions(BlockLib* lib)
{
    struct FunDeclaration
    {
        const char* name;
        const char*  returnType;
        const char* types[20];
        const char* arguments[20];
        FunCallback callback;
    } funDeclarations[] = {
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

    int apiFunCount = sizeof(funDeclarations) / sizeof(funDeclarations[0]);
    for (int i = 0; i < apiFunCount; ++i)
    {
        FunDeclaration& dec = funDeclarations[i];
        int argCount = -1;
        while(dec.types[++argCount] != nullptr);
        bool result = CreateIntrinsicFunction(
            lib->GetBuilder(), 
            dec.name,
            dec.types,
            dec.arguments,
            argCount,
            dec.returnType,
            dec.callback
        );
        PG_ASSERTSTR(result, "failed registering API");
    }
}


/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//! Implementation of API wrappers / manipulation of a node container
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static NodeContainer* GetContainer(BsVmState* state)
{
    NodeContainer* container = static_cast<NodeContainer*>(state->GetUserContext());
    PG_ASSERT(container != nullptr);
    return container;
}

/////////////////////////////////////////////////////////////
//!> Program Node functions
/////////////////////////////////////////////////////////////
void Program_SetShaderStage(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    NodeContainer* container = GetContainer(state);    

    
    int* output = static_cast<int*>(context.GetRawOutputBuffer());
    int programId = *static_cast<int*>(context.GetRawInputBuffer());
    int stageId   = *(static_cast<int*>(context.GetRawInputBuffer()) + 1);
    if (programId != -1 && stageId != -1)
    {
        PG_ASSERT(programId >= 0 && programId < container->mPrograms.GetSize());
        PG_ASSERT(stageId   >= 0 && stageId < container->mShaders.GetSize());
        container->mPrograms[programId]->SetShaderStage(container->mShaders[stageId]);
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
    NodeContainer* container = GetContainer(state);

    int* inptStr = static_cast<int*>(context.GetRawInputBuffer());
    const char* name = static_cast<const char*>(state->GetHeapElement(*inptStr).mObject);
    Shader::ProgramLinkageRef prog = container->mAppContext->GetShaderManager()->CreateProgram(name);
    int* output = static_cast<int*>(context.GetRawOutputBuffer());
    Shader::ProgramLinkageRef& newProg = container->mPrograms.PushEmpty();
    newProg = prog;
    *output = container->mPrograms.GetSize() - 1;
}

void Node_LoadShaderStage(FunCallbackContext& context)
{
    BsVmState* state = context.GetVmState();
    NodeContainer* container = GetContainer(state);

    int* inptStr = static_cast<int*>(context.GetRawInputBuffer());
    const char* path = static_cast<const char*>(state->GetHeapElement(*inptStr).mObject);

    Shader::ShaderStageFileProperties fileProps;
    fileProps.mPath = path;
    fileProps.mLoader = container->mAppContext->GetIOManager();
    fileProps.mUserData = nullptr;

    Shader::ShaderStageRef shaderStage = container->mAppContext->GetShaderManager()->LoadShaderStageFromFile(fileProps);
    int* output = static_cast<int*>(context.GetRawOutputBuffer());
    if (shaderStage != nullptr)
    {
        Shader::ShaderStageRef& newStage = container->mShaders.PushEmpty();
        newStage = shaderStage;
        *output = container->mShaders.GetSize() - 1;
    }
    else
    {
        *output = -1; //an invalid id
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

