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

    //this mesh only contains position UVs and normals
    mConfiguration.GetInputLayout()->GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_UV | MeshInputLayout::USE_NORMAL);
}

CombineTransformOperator::~CombineTransformOperator()
{
}

void CombineTransformOperator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr); 

    Math::Mat44 matrices[MaxCombineTransformInputs];
    GenerateMatrices(matrices);

    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}

void CombineTransformOperator::GenerateMatrices(Math::Mat44* matrices) const
{
    PropertyGrid::PropertyReadAccessor transAccessors[MaxCombineTransformInputs];
    PropertyGrid::PropertyReadAccessor scaleAccessors[MaxCombineTransformInputs];
    PropertyGrid::PropertyReadAccessor rotAccessors[MaxCombineTransformInputs];

    int accessorIdx = 0;

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        PG_ASSERT(GetClassPropertyRecord(accessorIdx).type == PropertyGrid::PROPERTYTYPE_VEC3);
        transAccessors[i] = GetClassReadPropertyAccessor(accessorIdx++);
    }

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        PG_ASSERT(GetClassPropertyRecord(accessorIdx).type == PropertyGrid::PROPERTYTYPE_VEC3);
        scaleAccessors[i] = GetClassReadPropertyAccessor(accessorIdx++);
    }

    for (int i = 0; i < MaxCombineTransformInputs; ++i)
    {
        PG_ASSERT(GetClassPropertyRecord(accessorIdx).type == PropertyGrid::PROPERTYTYPE_VEC4);
        rotAccessors[i] = GetClassReadPropertyAccessor(accessorIdx++);
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

        targetMat.m11 *= s.x;
        targetMat.m22 *= s.y;
        targetMat.m33 *= s.z;
    }

}

}
}
