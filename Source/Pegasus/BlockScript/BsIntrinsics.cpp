/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BsIntrinsics.cpp
//! \author Kleber Garcia
//! \date   October 12th 2014
//! \brief  runtime library intrinsic functions

#include "Pegasus/BlockScript/BsIntrinsics.h"
#include "Pegasus/BlockScript/BlockScriptBuilder.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/FunDesc.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Core/Log.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

//Intrinsic functions for vectors
namespace Private_VectorConstructors
{

void ConstructFloat4_float_float_float_float(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float)*4);
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*4);

    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    Math::Vec4* v4 = static_cast<Math::Vec4*>(argout);
    v4->x = ((float*)(argin))[0];
    v4->y = ((float*)(argin))[1];
    v4->z = ((float*)(argin))[2];
    v4->w = ((float*)(argin))[3];
    
}

void ConstructFloat4_int_int_int_int(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float)*4);
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*4);

    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    Math::Vec4* v4 = static_cast<Math::Vec4*>(argout);
    v4->x = static_cast<float>(((int*)(argin))[0]);
    v4->y = static_cast<float>(((int*)(argin))[1]);
    v4->z = static_cast<float>(((int*)(argin))[2]);
    v4->w = static_cast<float>(((int*)(argin))[3]);
    
}

void ConstructFloat4_float(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*4);

    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    float v = *((float*)argin);

    Math::Vec4* v4 = static_cast<Math::Vec4*>(argout);
    v4->x = v; 
    v4->y = v;
    v4->z = v;
    v4->w = v;
    
}

void ConstructFloat4_int(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*4);

    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();
    float v = static_cast<float>(*((int*)argin));

    Math::Vec4* v4 = static_cast<Math::Vec4*>(argout);
    v4->x = v; 
    v4->y = v;
    v4->z = v;
    v4->w = v;
    
}

void ConstructFloat3_float_float_float(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float)*3);
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*3);

    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    Math::Vec3* v3 = static_cast<Math::Vec3*>(argout);
    v3->x = ((float*)(argin))[0];
    v3->y = ((float*)(argin))[1];
    v3->z = ((float*)(argin))[2];
    
}

void ConstructFloat3_int_int_int(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float)*3);
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*3);

    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    Math::Vec3* v3 = static_cast<Math::Vec3*>(argout);
    v3->x = static_cast<float>(((int*)(argin))[0]);
    v3->y = static_cast<float>(((int*)(argin))[1]);
    v3->z = static_cast<float>(((int*)(argin))[2]);
    
}

void ConstructFloat3_float(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*3);
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();
    float v = *((float*)argin);

    Math::Vec3* v3 = static_cast<Math::Vec3*>(argout);
    v3->x = v; 
    v3->y = v;
    v3->z = v;
    
}

void ConstructFloat3_int(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*3);
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();
    float v = static_cast<float>(*((int*)argin));

    Math::Vec3* v3 = static_cast<Math::Vec3*>(argout);
    v3->x = v; 
    v3->y = v;
    v3->z = v;
    
}

void ConstructFloat2_float_float(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float)*2);
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*2);
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    Math::Vec2* v2 = static_cast<Math::Vec2*>(argout);
    v2->x = ((float*)(argin))[0];
    v2->y = ((float*)(argin))[1];
    
}

void ConstructFloat2_int_int(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float)*2);
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*2);
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();

    Math::Vec2* v2 = static_cast<Math::Vec2*>(argout);
    v2->x = static_cast<float>(((int*)(argin))[0]);
    v2->y = static_cast<float>(((int*)(argin))[1]);
    
}

void ConstructFloat2_float(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*2);
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();
    float v = *((float*)argin);

    Math::Vec2* v2 = static_cast<Math::Vec2*>(argout);
    v2->x = v; 
    v2->y = v;
    
}

void ConstructFloat2_int(FunCallbackContext& context)
{
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));
    PG_ASSERT(context.GetOutputBufferSize() == sizeof(float)*2);
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();
    float v = static_cast<float>(*((int*)argin));

    Math::Vec2* v2 = static_cast<Math::Vec2*>(argout);
    v2->x = v; 
    v2->y = v;
    
}

}

// Intrinsic functions for misc utilities
namespace Private_Utilities
{
void Echo_String(FunCallbackContext& context)
{
    int* heapPtr = static_cast<int*>(context.GetRawInputBuffer());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(int));

    BsVmState::HeapElement& str = context.GetVmState()->GetHeapElement(*heapPtr);
    if (Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback != nullptr)
    {
        Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback(static_cast<char*>(str.mObject));
    }
}

void Echo_Int(FunCallbackContext& context)
{
    int* intPtr = static_cast<int*>(context.GetRawInputBuffer());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(int));

    int v = *intPtr;
    if (Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback != nullptr)
    {
        Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback(v);
    } 
}

void Echo_Float(FunCallbackContext& context)
{
    int* floatPtr = static_cast<int*>(context.GetRawInputBuffer());
    PG_ASSERT(context.GetInputBufferSize() == sizeof(float));

    float v = *reinterpret_cast<float*>(floatPtr);
    if (Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback != nullptr)
    {
        Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback(v);
    } 
}

}

static void RegisterIntrinsicTypes(BlockLib* lib)
{
    SymbolTable* symbolTable = lib->GetSymbolTable();
    PG_ASSERT(symbolTable != nullptr);

    //Register ints and scalars
    TypeDesc* floatT = symbolTable->CreateType(
        TypeDesc::M_SCALAR,
        "float"
    );

    floatT->SetAluEngine(TypeDesc::E_FLOAT);

    TypeDesc* intT   = symbolTable->CreateType(
        TypeDesc::M_SCALAR,
        "int"
    );
    intT->SetAluEngine(TypeDesc::E_INT);

    char fStr[25];
    fStr[0] = '\0';
    char iStr[25];
    iStr[0] = '\0';
    Utils::Strcat(fStr, "float");
    Utils::Strcat(iStr, "int");
    int fStrLen = Utils::Strlen(fStr);
    int iStrLen = Utils::Strlen(iStr);
    PG_ASSERT(BlockScript::Ast::gMaxAluDimensions < 10); //only 1 digit numbers
    for (int i = 2; i <= BlockScript::Ast::gMaxAluDimensions; ++i)
    {
        fStr[fStrLen] = i + '0';
        iStr[iStrLen] = i + '0';
        fStr[fStrLen+1] = '\0';
        iStr[iStrLen+1] = '\0';
        TypeDesc* t1 = symbolTable->CreateType(
            TypeDesc::M_VECTOR,
            fStr,
            floatT,
            i
        );
        t1->SetAluEngine(static_cast<TypeDesc::AluEngine>(TypeDesc::E_FLOAT + i - 1));

    }

    symbolTable->CreateType(TypeDesc::M_REFERECE, "string");
}

void Pegasus::BlockScript::RegisterIntrinsics(BlockLib* lib)
{
    RegisterIntrinsicTypes(lib);

    const Pegasus::BlockScript::FunctionDeclarationDesc funConstructors[] =
    {
        //*funName | retType | argsTypes                                   |  argNames                    | callback
        ///////////////////////////////////////////float4///////////////////////////////////////////////////////////////
        { "float4", "float4", {"float", "float", "float", "float", nullptr}, {"x", "y", "z", "w", nullptr}, Private_VectorConstructors::ConstructFloat4_float_float_float_float },
        { "float4", "float4", {"float3", "float", nullptr},                  {"xyz", "w", nullptr},         Private_VectorConstructors::ConstructFloat4_float_float_float_float },
        { "float4", "float4", {"int", "int", "int", "int", nullptr},         {"x", "y", "z", "w", nullptr}, Private_VectorConstructors::ConstructFloat4_int_int_int_int },
        { "float4", "float4", {"float", nullptr},                            {"xyzw", nullptr},             Private_VectorConstructors::ConstructFloat4_float },
        { "float4", "float4", {"int", nullptr},                              {"xyzw", nullptr},             Private_VectorConstructors::ConstructFloat4_int },
        ///////////////////////////////////////////float3///////////////////////////////////////////////////////////////
        { "float3", "float3", {"float" , "float", "float", nullptr},         {"x", "y", "z", nullptr},      Private_VectorConstructors::ConstructFloat3_float_float_float },
        { "float3", "float3", {"float2", "float", nullptr},                  {"x", "y", "z", nullptr},      Private_VectorConstructors::ConstructFloat3_float_float_float },
        { "float3", "float3", {"int", "int", "int", nullptr},                {"x", "y", "z", nullptr},      Private_VectorConstructors::ConstructFloat3_int_int_int },
        { "float3", "float3", {"float", nullptr},                            {"xyz", nullptr},              Private_VectorConstructors::ConstructFloat3_float },
        { "float3", "float3", {"int", nullptr},                              {"xyz", nullptr},              Private_VectorConstructors::ConstructFloat3_int },
        ///////////////////////////////////////////float2///////////////////////////////////////////////////////////////
        {"float2", "float2",  {"float", "float", nullptr},                   {"x", "y", nullptr},           Private_VectorConstructors::ConstructFloat2_float_float },
        {"float2", "float2",  {"int", "int", nullptr},                       {"x", "y", nullptr},           Private_VectorConstructors::ConstructFloat2_int_int },
        {"float2", "float2",  {"float", nullptr},                            {"xy", nullptr},               Private_VectorConstructors::ConstructFloat2_float },
        {"float2", "float2",  {"int", nullptr},                              {"xy", nullptr},               Private_VectorConstructors::ConstructFloat2_int },
        ///////////////////////////////////////////echo///////////////////////////////////////////////////////////////
        {"echo",   "int",     {"string", nullptr},                           {"input", nullptr},            Private_Utilities::Echo_String },
        {"echo",   "int",     {"int", nullptr},                              {"input", nullptr},            Private_Utilities::Echo_Int },
        {"echo",   "int",     {"float", nullptr},                            {"input", nullptr},            Private_Utilities::Echo_Float },
    };

     lib->CreateIntrinsicFunctions(funConstructors, sizeof(funConstructors) / sizeof(funConstructors[0])); 
        
}

