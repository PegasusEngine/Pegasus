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
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Matrix.h"
#include "Pegasus/Math/Quaternion.h"
#include "Pegasus/Core/Log.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

//Intrinsic functions for vectors
namespace Private_VectorConstructors
{

template<int sz>
void ConstructMatrixN_by_N(FunCallbackContext& context)
{
    PG_ASSERTSTR(context.GetInputBufferSize() == sizeof(float)*sz, "Size of input buffer: %d", context.GetInputBufferSize());
    PG_ASSERTSTR(context.GetOutputBufferSize() == sizeof(float)*sz, "Size of output buffer: %d", context.GetOutputBufferSize());
    void* argin = context.GetRawInputBuffer();
    void* argout = context.GetRawOutputBuffer();
    Utils::Memcpy(argout, argin, context.GetInputBufferSize());
}

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

namespace Private_Math
{
    template<class T>
    void Lerp(FunCallbackContext& context)
    {
        FunParamStream stream(context);
        T& a = stream.NextArgument<T>();
        T& b = stream.NextArgument<T>();
        float t = stream.NextArgument<float>();
        stream.SubmitReturn<T>(Math::Lerp(a,b,t));
    }

    template<class V, class M, void MULF(V&, const M&, const V&)>
    void Mul(FunCallbackContext& context)   
    {
        FunParamStream stream(context);
        M& m = stream.NextArgument<M>();
        V& t = stream.NextArgument<V>();
        PG_ASSERT(sizeof(V) == context.GetOutputBufferSize());
        V* r = static_cast<V*>(context.GetRawOutputBuffer());
        MULF(*r, m, t);
    }

    template<class T>
    void Dot(FunCallbackContext& context)
    {
        FunParamStream stream(context);
        T& v1 = stream.NextArgument<T>();
        T& v2 = stream.NextArgument<T>();
        stream.SubmitReturn<float>(Math::Dot(v1,v2));
    }

    template<class T>
    void Cross(FunCallbackContext& context)
    {
        FunParamStream stream(context); 
        T& v1 = stream.NextArgument<T>();
        T& v2 = stream.NextArgument<T>(); 
        stream.SubmitReturn<T>(Math::Cross(v1, v2));
    }

    void Sin(FunCallbackContext& context)
    {
        FunParamStream stream(context); 
        float v1 = stream.NextArgument<float>();
        stream.SubmitReturn<float>(Math::Sin(v1));
    }

    void Cos(FunCallbackContext& context)
    {
        FunParamStream stream(context); 
        float v1 = stream.NextArgument<float>();
        stream.SubmitReturn<float>(Math::Cos(v1));
    }

    void Mat44_Rotation(FunCallbackContext& context)
    {
        FunParamStream stream(context); 
        Math::Vec3 axis = stream.NextArgument<Math::Vec3>();
        float amount = stream.NextArgument<float>();
        PG_ASSERT(context.GetOutputBufferSize() == sizeof(Math::Mat44));
        Math::Mat44& res = *static_cast<Math::Mat44*>(context.GetRawOutputBuffer());
        Math::SetRotation(res, axis, amount );
    }
}

// Intrinsic functions for math


static void RegisterIntrinsicTypes(BlockLib* lib)
{
    SymbolTable* symbolTable = lib->GetSymbolTable();
    PG_ASSERT(symbolTable != nullptr);

    //Register the start type, which is a pointer to anything
    symbolTable->CreateStarType();

    //Register ints and scalars
    TypeDesc* floatT = symbolTable->CreateScalarType("float", TypeDesc::E_FLOAT);

    TypeDesc* intT   = symbolTable->CreateScalarType("int", TypeDesc::E_INT);

    char fStr[25];
    fStr[0] = '\0';
    char mStr[25];
    mStr[0] = '\0';
    
    Utils::Strcat(fStr, "float");
    int fStrLen = Utils::Strlen(fStr);
    Utils::Strcat(mStr, "float");
    int mStrLen = Utils::Strlen(mStr);
    
    PG_ASSERT(BlockScript::Ast::gMaxAluDimensions < 10); //only 1 digit numbers
    for (int i = 2; i <= BlockScript::Ast::gMaxAluDimensions; ++i)
    {
        fStr[fStrLen] = i + '0';
        fStr[fStrLen+1] = '\0';
        TypeDesc* t1 = symbolTable->CreateVectorType(
            fStr,
            floatT,
            i,
            static_cast<TypeDesc::AluEngine>(TypeDesc::E_FLOAT + i - 1)
        );

        //float4x4
        mStr[mStrLen] = i + '0';
        mStr[mStrLen + 1] = 'x';
        mStr[mStrLen + 2] = i + '0';
        mStr[mStrLen + 3] = '\0';
        symbolTable->CreateVectorType(
            mStr,
            t1,
            i,
            static_cast<TypeDesc::AluEngine>(TypeDesc::E_MATRIX2x2 + i - 2)
        );
    }

    symbolTable->CreateObjectType("string",nullptr,nullptr);
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
        ///////////////////////////////////////////float4x4///////////////////////////////////////////////////////////////
        { "float4x4", "float4x4", {"float4", "float4", "float4", "float4", nullptr}, {"col_x", "col_y", "col_z", "col_w", nullptr}, Private_VectorConstructors::ConstructMatrixN_by_N<16>},
        { "float4x4", "float4x4", {"float", "float", "float", "float", 
                               "float", "float", "float", "float", 
                               "float", "float", "float", "float", 
                               "float", "float", "float", "float", nullptr}, 
                              {"m11", "m12", "m13", "m14",
                               "m21", "m22", "m23", "m24",
                               "m31", "m32", "m33", "m34",
                               "m41", "m42", "m43", "m44",  nullptr}, Private_VectorConstructors::ConstructMatrixN_by_N<16> },
        ///////////////////////////////////////////float3x3///////////////////////////////////////////////////////////////
        { "float3x3", "float3x3", {"float3", "float3", "float3", nullptr}, {"col_x", "col_y", "col_z", nullptr}, Private_VectorConstructors::ConstructMatrixN_by_N<9> },
        { "float3x3", "float3x3", {"float", "float", "float", 
                               "float", "float", "float", 
                               "float", "float", "float", nullptr}, 
                              {"m11", "m12", "m13",
                               "m21", "m22", "m23",
                               "m41", "m42", "m43", nullptr}, Private_VectorConstructors::ConstructMatrixN_by_N<9> },
        ///////////////////////////////////////////float2x2///////////////////////////////////////////////////////////////
        { "float2x2", "float2x2", {"float2", "float2", nullptr}, {"x", "y", nullptr}, Private_VectorConstructors::ConstructMatrixN_by_N<4> },
        { "float2x2", "float2x2", {"float", "float", 
                               "float", "float", nullptr}, 
                              {"m11", "m12",
                               "m41", "m42", nullptr}, Private_VectorConstructors::ConstructMatrixN_by_N<4> },
    };

    lib->CreateIntrinsicFunctions(funConstructors, sizeof(funConstructors) / sizeof(funConstructors[0])); 

    //Register Math intrinsics
    const Pegasus::BlockScript::FunctionDeclarationDesc mathFuncs[] =
    {
        //*funName | retType | argsTypes                                   |  argNames                    | callback
        ///////////////////////////////////////////DOT///////////////////////////////////////////////////////////////
        { "dot", "float",  { "float4",  "float4", nullptr}, {"x", "y", nullptr}, Private_Math::Dot<Math::Vec4>},
        { "dot", "float",  { "float3",  "float3", nullptr}, {"x", "y", nullptr}, Private_Math::Dot<Math::Vec3>},
        { "dot", "float",  { "float2",  "float2", nullptr}, {"x", "y", nullptr}, Private_Math::Dot<Math::Vec2>},
        ///////////////////////////////////////////LERP///////////////////////////////////////////////////////////////
        { "lerp", "float",  { "float",  "float",  "float",  nullptr}, {"x", "y", "t", nullptr}, Private_Math::Lerp<float>},
        { "lerp", "float4", { "float4", "float4", "float",  nullptr}, {"x", "y", "t", nullptr}, Private_Math::Lerp<Math::Vec4>},
        { "lerp", "float3", { "float3", "float3", "float",  nullptr}, {"x", "y", "t", nullptr}, Private_Math::Lerp<Math::Vec3>},
        { "lerp", "float2", { "float2", "float2", "float",  nullptr}, {"x", "y", "t", nullptr}, Private_Math::Lerp<Math::Vec2>},
        ///////////////////////////////////////////MUL///////////////////////////////////////////////////////////////
        { "mul", "float4", { "float4x4", "float4", nullptr}, {"x", "y", nullptr}, Private_Math::Mul<Math::Vec4, Math::Mat44, Math::Mult44_41>},
        { "mul", "float3", { "float3x3", "float3", nullptr}, {"x", "y", nullptr}, Private_Math::Mul<Math::Vec3, Math::Mat33, Math::Mult33_31>},
        { "mul", "float2", { "float2x2", "float2", nullptr}, {"x", "y", nullptr}, Private_Math::Mul<Math::Vec2, Math::Mat22, Math::Mult22_21>},
        ///////////////////////////////////////////CROSS///////////////////////////////////////////////////////////////
        { "cross", "float3", { "float3", "float3", nullptr}, {"x", "y", nullptr}, Private_Math::Cross<Math::Vec3>},
        ///////////////////////////////////////////TRIG///////////////////////////////////////////////////////////////
        { "sin", "float", { "float", nullptr}, {"v", nullptr}, Private_Math::Sin},
        { "cos", "float", { "float", nullptr}, {"v", nullptr}, Private_Math::Cos},
        { "GetRotation", "float4x4", { "float3", "float", nullptr}, {"axis", "amount", nullptr}, Private_Math::Mat44_Rotation}
    };
        
    lib->CreateIntrinsicFunctions(mathFuncs, sizeof(mathFuncs) / sizeof(mathFuncs[0])); 
}

