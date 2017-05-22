/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MultiCopyOperator.h
//! \author	Kleber Garcia
//! \date	May 21th 2017
//! \brief	MultiCopyOperator
#include "Pegasus/Mesh/Operator/MultiCopyOperator.h"
#include "Pegasus/Math/Quaternion.h"

namespace Pegasus {
namespace Mesh {


//! Property implementations
BEGIN_IMPLEMENT_PROPERTIES(MultiCopyOperator)
    IMPLEMENT_PROPERTY(MultiCopyOperator, MultiCopyFactor)
    IMPLEMENT_PROPERTY(MultiCopyOperator, TranslationOffset)
    IMPLEMENT_PROPERTY(MultiCopyOperator, ScaleOffset)
    IMPLEMENT_PROPERTY(MultiCopyOperator, QuaternionRotOffset)
END_IMPLEMENT_PROPERTIES(MultiCopyOperator)


MultiCopyOperator::MultiCopyOperator(Pegasus::Alloc::IAllocator* nodeAllocator, 
              Pegasus::Alloc::IAllocator* nodeDataAllocator) 
: MeshOperator(nodeAllocator, nodeDataAllocator)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(MultiCopyOperator)
        INIT_PROPERTY(MultiCopyFactor)
        INIT_PROPERTY(TranslationOffset)
        INIT_PROPERTY(ScaleOffset)
        INIT_PROPERTY(QuaternionRotOffset)
    END_INIT_PROPERTIES()
}

MultiCopyOperator::~MultiCopyOperator()
{
}

void MultiCopyOperator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);

    //generate matrices
    Math::Mat44 mat = Math::MAT44_IDENTITY;
    Math::Mat33 normMat = Math::MAT33_IDENTITY;

    int iterCount = GetMultiCopyFactor();
    if (iterCount <= 1 || iterCount > 64)
    {
        PG_LOG('ERR_', "MultiCopyFactor not allowed to be below 1 or above 64.");
        iterCount = 1;
    }
    bool updated = false;
    MeshDataRef inputMesh = static_cast<MeshData *>(&(*GetInput(0)->GetUpdatedData(updated)));
    const StdVertex* inputVertex = inputMesh->GetStream<StdVertex>(0);
    const unsigned short* inputIndexes = inputMesh->GetIndexBuffer();

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr); 
    meshData->AllocateVertexes(inputMesh->GetVertexCount() * iterCount);
    meshData->AllocateIndexes(inputMesh->GetIndexCount() * iterCount);
    StdVertex* outputVertex = meshData->GetStream<StdVertex>(0);
    unsigned short* outputIndexes = meshData->GetIndexBuffer();

    for (int i = 0; i < iterCount; ++i)
    {
        //transform current mesh
        for (int v = 0; v < inputMesh->GetVertexCount(); ++v)
        {
            int vIdx = i*inputMesh->GetVertexCount() + v;
            Math::Mult44_41(outputVertex[vIdx].position, mat, inputVertex[v].position);
            Math::Mult33_31(outputVertex[vIdx].normal, normMat, inputVertex[v].normal);
            outputVertex[vIdx].uv = inputVertex[v].uv;
        }

        for (int k = 0; k < inputMesh->GetIndexCount(); ++k)
        {
           int kIdx =  i*inputMesh->GetIndexCount() + k; 
           outputIndexes[kIdx] = inputIndexes[k] + i*inputMesh->GetVertexCount();
        }

        //prepare transforms for next iteration
        Math::Mat44 newTransform;
        Math::Mat33 newNormTransform;

        Math::Vec3 s = GetScaleOffset();
        Math::Mat44 scaleMat4 = Math::MAT44_IDENTITY;
        Math::Mat33 scaleMat3 = Math::MAT33_IDENTITY;
        scaleMat4.m11 = s.x;
        scaleMat4.m22 = s.y;
        scaleMat4.m33 = s.z;
        scaleMat3.m11 = 1.0f/s.x;
        scaleMat3.m12 = 1.0f/s.y;
        scaleMat3.m13 = 1.0f/s.z;
        

        Math::Vec3 t = GetTranslationOffset();

        Math::Vec4 axisAngle = GetQuaternionRotOffset();
        Math::Vec3 axisRot(axisAngle.x,axisAngle.y,axisAngle.z);
        Math::Quaternion q(axisRot, axisAngle.w);
        Math::QuaternionToMat44(newTransform, q);
        Math::QuaternionToMat33(newNormTransform, q);

        Math::Mult44_44(newTransform,newTransform,scaleMat4);
        Math::Mult33_33(newNormTransform,newNormTransform,scaleMat3);

        newTransform.m14 = t.x;
        newTransform.m24 = t.y;
        newTransform.m34 = t.z;
        newTransform.m44 = 1.0f;

        Math::Mult44_44(mat, mat, newTransform);
        Math::Mult33_33(normMat, normMat, newNormTransform);
    }


    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}


}
}
