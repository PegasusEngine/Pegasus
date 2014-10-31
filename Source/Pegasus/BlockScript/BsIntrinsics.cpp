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
#include "Pegasus/BlockScript/TypeTable.h"
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


// Conversion API
void Pegasus::BlockScript::RegisterIntrinsics(BlockScriptBuilder* builder)
{
    static struct ConstructorDesc
    {
        const char* funName;
        const char* returnType;
        struct funDefinition
        {
            int argCounts;
            const char* argTypes[50];
            const char* argNames[50];
            FunCallback callback;
        } mDefinitions[10];
        int overrideCounts;
    } FunConstructors[] =
    {
        {
            "float4", //fun name
            "float4",  // return type
            {
                { 4, {"float", "float", "float", "float", nullptr}, {"x", "y", "z", "w", nullptr}, Private_VectorConstructors::ConstructFloat4_float_float_float_float },
                { 2, {"float3", "float", nullptr}, {"xyz", "w", nullptr}, Private_VectorConstructors::ConstructFloat4_float_float_float_float },
                { 4, {"int", "int", "int", "int", nullptr}, {"x", "y", "z", "w", nullptr}, Private_VectorConstructors::ConstructFloat4_int_int_int_int },
                { 1, {"float", nullptr}, {"xyzw", nullptr}, Private_VectorConstructors::ConstructFloat4_float },
                { 1, {"int", nullptr}, {"xyzw", nullptr}, Private_VectorConstructors::ConstructFloat4_int }
            },
            5
        },
        {
            "float3", //fun name
            "float3",  // return type
            {
                { 3,{"float" , "float", "float", nullptr}, {"x", "y", "z", nullptr}, Private_VectorConstructors::ConstructFloat3_float_float_float },
                { 2,{"float2", "float", nullptr}, {"x", "y", "z", nullptr}, Private_VectorConstructors::ConstructFloat3_float_float_float },
                { 3,{"int", "int", "int", nullptr}, {"x", "y", "z", nullptr}, Private_VectorConstructors::ConstructFloat3_int_int_int },
                { 1,{"float", nullptr}, {"xyz", nullptr}, Private_VectorConstructors::ConstructFloat3_float },
                { 1,{"int", nullptr}, {"xyz", nullptr}, Private_VectorConstructors::ConstructFloat3_int },
            },
            5
        },
        {
            "float2", //fun name
            "float2",  // return type
            {
                {2,{"float", "float", nullptr}, {"x", "y", nullptr}, Private_VectorConstructors::ConstructFloat2_float_float },
                {2,{"int", "int", nullptr}, {"x", "y", nullptr}, Private_VectorConstructors::ConstructFloat2_int_int },
                {1,{"float", nullptr}, {"xy", nullptr}, Private_VectorConstructors::ConstructFloat2_float },
                {1,{"int", nullptr}, {"xy", nullptr}, Private_VectorConstructors::ConstructFloat2_int },
            },
            4
        },
        {
            "echo", //fun name
            "int",  // return type
            {
                {1,{"string", nullptr}, {"input", nullptr}, Private_Utilities::Echo_String },
                {1,{"int", nullptr}, {"input", nullptr}, Private_Utilities::Echo_Int },
                {1,{"float", nullptr}, {"input", nullptr}, Private_Utilities::Echo_Float },
            },
            3
        }
    };

    static const int ConstructorCount = sizeof(FunConstructors) / sizeof(ConstructorDesc); 
    
    for (int i = 0; i < ConstructorCount; ++i)
    {
        ConstructorDesc& constructorDesc = FunConstructors[i];
        for (int j = 0; j < constructorDesc.overrideCounts; ++j)
        {
            bool result = CreateIntrinsicFunction(
                builder,
                constructorDesc.funName,
                constructorDesc.mDefinitions[j].argTypes,
                constructorDesc.mDefinitions[j].argNames,
                constructorDesc.mDefinitions[j].argCounts,
                constructorDesc.returnType,
                constructorDesc.mDefinitions[j].callback
            );
            
            PG_ASSERT(result);
        }
    }
}

