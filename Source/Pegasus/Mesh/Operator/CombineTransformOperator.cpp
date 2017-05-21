/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CombineTransformOperator.cpp
//! \author	Kleber Garcia
//! \date	May 17th 2017
//! \brief	CombineTransformOperator


#include "Pegasus/Mesh/Operator/CombineTransformOperator.h"
#include "Pegasus/Math/Quaternion.h"

namespace Pegasus {
namespace Mesh {

//TODO: put this on its own header file.
struct Vertex {
    Math::Vec4 position; 
    Math::Vec3 normal;        
    Math::Vec2 uv; 
};

//! Property implementations
BEGIN_IMPLEMENT_PROPERTIES(CombineTransformOperator)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Translation0)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Translation1)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Translation2)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Translation3)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Scale0)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Scale1)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Scale2)
    IMPLEMENT_PROPERTY(CombineTransformOperator, Scale3)
    IMPLEMENT_PROPERTY(CombineTransformOperator, QuaternionRot0)
    IMPLEMENT_PROPERTY(CombineTransformOperator, QuaternionRot1)
    IMPLEMENT_PROPERTY(CombineTransformOperator, QuaternionRot2)
    IMPLEMENT_PROPERTY(CombineTransformOperator, QuaternionRot3)
END_IMPLEMENT_PROPERTIES(CombineTransformOperator)


CombineTransformOperator::CombineTransformOperator(Pegasus::Alloc::IAllocator* nodeAllocator, 
              Pegasus::Alloc::IAllocator* nodeDataAllocator) 
: MeshOperator(nodeAllocator, nodeDataAllocator)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(CombineTransformOperator)
        INIT_PROPERTY(Translation0)
        INIT_PROPERTY(Translation1)
        INIT_PROPERTY(Translation2)
        INIT_PROPERTY(Translation3)
        INIT_PROPERTY(Scale0)
        INIT_PROPERTY(Scale1)
        INIT_PROPERTY(Scale2)
        INIT_PROPERTY(Scale3)
        INIT_PROPERTY(QuaternionRot0)
        INIT_PROPERTY(QuaternionRot1)
        INIT_PROPERTY(QuaternionRot2)
        INIT_PROPERTY(QuaternionRot3)
    END_INIT_PROPERTIES()
}

CombineTransformOperator::~CombineTransformOperator()
{
}

static void TransformAppendMesh(const Vertex* input, Vertex* outputDest, int count, const Math::Mat44& targetTransform, const Math::Mat33& targetNormalTransform)
{
    for (int v = 0; v < count; ++v)
    {
        Math::Mult44_41(outputDest[v].position, targetTransform, input[v].position);
        Math::Mult33_31(outputDest[v].normal, targetNormalTransform, input[v].normal);
        float len = Math::Length(outputDest[v].normal);
        if (Math::Abs(len) > PFLOAT_EPSILON)
        {
            outputDest[v].normal /= len;
        }
        
        outputDest[v].uv = input[v].uv;
    }
}

static void AppendIndices(const unsigned short* inputIndices, unsigned short* outputIndices, int count, short newOffset)
{
    for (int i = 0; i < count; ++i)
    {
        outputIndices[i] = inputIndices[i] + newOffset;
    }
}

void CombineTransformOperator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr); 

    Math::Mat44 matrices[MaxCombineTransformInputs];
    Math::Mat33 normalMatrices[MaxCombineTransformInputs];
    GenerateMatrices(matrices, normalMatrices);

    unsigned int indexSummedCounts[MaxCombineTransformInputs];//holds counts of everything before
    unsigned int vertexSummedCounts[MaxCombineTransformInputs]; //holds counts of everything before
    unsigned int currentIndexCount = 0;
    unsigned int currentVertexCount = 0;

    //go for every single active child mesh and get all the counts.
    for (unsigned i = 0; i < GetNumInputs(); ++i)
    {
        bool updated = false;
        MeshData* inputData = static_cast<MeshData *>(&(*GetInput(i)->GetUpdatedData(updated)));
        //TODO: Work on checking compatible format when combining.
        if (inputData != nullptr)
        {
            indexSummedCounts[i] = currentIndexCount;
            vertexSummedCounts[i] = currentVertexCount;
            currentIndexCount += inputData->GetIndexCount();
            currentVertexCount += inputData->GetVertexCount();
        }
    }

    meshData->AllocateIndexes(currentIndexCount);
    meshData->AllocateVertexes(currentVertexCount);

    Vertex* outputVertData = meshData->GetStream<Vertex>(0);
    unsigned short* outputIndices = meshData->GetIndexBuffer();

    //go for every single active child mesh and get all the counts.
    for (unsigned i = 0; i < GetNumInputs(); ++i)
    {
        bool updated = false;
        MeshData* inputData = static_cast<MeshData *>(&(*GetInput(i)->GetUpdatedData(updated)));
        Math::Mat44& targetTransform = matrices[i];
        Math::Mat33& targetNormalTransform = normalMatrices[i];
        if (inputData != nullptr)
        {
            const Vertex* inputVertData = inputData->GetStream<Vertex>(0);
            Vertex* currentMeshOutput = outputVertData + vertexSummedCounts[i];
            TransformAppendMesh(inputVertData, currentMeshOutput, inputData->GetVertexCount(), targetTransform, targetNormalTransform);
            
            const unsigned short* inputIndices = inputData->GetIndexBuffer();
            unsigned short* currentIndexOutput = outputIndices + indexSummedCounts[i];
            AppendIndices(inputIndices, currentIndexOutput, inputData->GetIndexCount(), vertexSummedCounts[i]);
        }
    }

    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}

void CombineTransformOperator::GenerateMatrices(Math::Mat44* matrices, Math::Mat33* normalMatrices) const
{
    PropertyGrid::PropertyReadAccessor transAccessors[MaxCombineTransformInputs];
    PropertyGrid::PropertyReadAccessor scaleAccessors[MaxCombineTransformInputs];
    PropertyGrid::PropertyReadAccessor rotAccessors[MaxCombineTransformInputs];

    int accessorIdx = 0;

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        PG_ASSERT(GetDerivedClassPropertyRecord(accessorIdx).type == PropertyGrid::PROPERTYTYPE_VEC3);
        transAccessors[i] = GetDerivedClassReadPropertyAccessor(accessorIdx++);
    }

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        PG_ASSERT(GetDerivedClassPropertyRecord(accessorIdx).type == PropertyGrid::PROPERTYTYPE_VEC3);
        scaleAccessors[i] = GetDerivedClassReadPropertyAccessor(accessorIdx++);
    }

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        PG_ASSERT(GetDerivedClassPropertyRecord(accessorIdx).type == PropertyGrid::PROPERTYTYPE_VEC4);
        rotAccessors[i] = GetDerivedClassReadPropertyAccessor(accessorIdx++);
    }

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        Math::Mat44& targetMat = matrices[i];
        Math::Vec4 axisAngle = rotAccessors[i].Get<Math::Vec4>();
        Math::Vec3 axisRot(axisAngle.x,axisAngle.y,axisAngle.z);
        Math::Vec3 t = transAccessors[i].Get<Math::Vec3>();
        Math::Vec3 s = scaleAccessors[i].Get<Math::Vec3>();

        Math::Quaternion q(axisRot, axisAngle.w);
        Math::QuaternionToMat44(targetMat, q);

        targetMat.m14 = t.x;
        targetMat.m24 = t.y;
        targetMat.m34 = t.z;
        targetMat.m44 = 1.0f;

        Math::Mat33& targetNormalmat = normalMatrices[i];
        targetNormalmat.m11 = targetMat.m11;
        targetNormalmat.m12 = targetMat.m12;
        targetNormalmat.m13 = targetMat.m13;
        targetNormalmat.m21 = targetMat.m11;
        targetNormalmat.m22 = targetMat.m12;
        targetNormalmat.m23 = targetMat.m13;
        targetNormalmat.m31 = targetMat.m11;
        targetNormalmat.m32 = targetMat.m12;
        targetNormalmat.m33 = targetMat.m13;

        //apply all scales
        targetMat.m11 *= s.x;
        targetMat.m22 *= s.y;
        targetMat.m33 *= s.z;
        targetNormalmat.m11 /= s.x;
        targetNormalmat.m22 /= s.y;
        targetNormalmat.m33 /= s.z;
    }

}

}
}
