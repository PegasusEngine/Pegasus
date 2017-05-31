/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	WaveFieldOperator.h
//! \author	Kleber Garcia
//! \date	May 21th 2017
//! \brief	WaveFieldOperator
#include "Pegasus/Mesh/Operator/WaveFieldOperator.h"
#include "Pegasus/Math/Quaternion.h"

namespace Pegasus {
namespace Mesh {


//! Property implementations
BEGIN_IMPLEMENT_PROPERTIES(WaveFieldOperator)
    IMPLEMENT_PROPERTY(WaveFieldOperator, WFFreqFieldQuaternion)
    IMPLEMENT_PROPERTY(WaveFieldOperator, WFFreqAmpOffset0)
    IMPLEMENT_PROPERTY(WaveFieldOperator, WFFreqAmpOffset1)
    IMPLEMENT_PROPERTY(WaveFieldOperator, WFFreqAmpOffset2)
    IMPLEMENT_PROPERTY(WaveFieldOperator, WFFreqAmpOffset3)
    IMPLEMENT_PROPERTY(WaveFieldOperator, WFIsRadial)
END_IMPLEMENT_PROPERTIES(WaveFieldOperator)


WaveFieldOperator::WaveFieldOperator(Pegasus::Alloc::IAllocator* nodeAllocator, 
              Pegasus::Alloc::IAllocator* nodeDataAllocator) 
: MeshOperator(nodeAllocator, nodeDataAllocator)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(WaveFieldOperator)
        INIT_PROPERTY(WFFreqFieldQuaternion)
        INIT_PROPERTY(WFFreqAmpOffset0)
        INIT_PROPERTY(WFFreqAmpOffset1)
        INIT_PROPERTY(WFFreqAmpOffset2)
        INIT_PROPERTY(WFFreqAmpOffset3)
        INIT_PROPERTY(WFIsRadial)
    END_INIT_PROPERTIES()
}

WaveFieldOperator::~WaveFieldOperator()
{
}

void WaveFieldOperator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);
    
    Math::Vec4 axisRotAngle = GetWFFreqFieldQuaternion();
    Math::Vec3 axisRot(axisRotAngle.x, axisRotAngle.y, axisRotAngle.z);
    Math::Quaternion q(axisRot, axisRotAngle.w);

    Math::Vec3 displacementDir = Math::RotateVector(Math::Vec3(0.0f,1.0f,0.0f),q);
    Math::Vec3 timeDir = Math::RotateVector(Math::Vec3(0.0f, 0.0f, 1.0f), q);
    
    bool updated = false;
    MeshDataRef inputMesh = static_cast<MeshData *>(&(*GetInput(0)->GetUpdatedData(updated)));
    const StdVertex* inputVertex = inputMesh->GetStream<StdVertex>(0);
    const unsigned short* inputIndexes = inputMesh->GetIndexBuffer();

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr); 
    meshData->AllocateVertexes(inputMesh->GetVertexCount());
    meshData->AllocateIndexes(inputMesh->GetIndexCount());
    StdVertex* outputVertex = meshData->GetStream<StdVertex>(0);
    unsigned short* outputIndexes = meshData->GetIndexBuffer();

    //copy indexes, which are exact replicas.
    Utils::Memcpy(outputIndexes, inputIndexes, sizeof(unsigned short)*meshData->GetIndexCount());

    //setup FFT waves
    Math::Vec3 waveParams[NumOfWaves];
    waveParams[0] = GetWFFreqAmpOffset0();
    waveParams[1] = GetWFFreqAmpOffset1();
    waveParams[2] = GetWFFreqAmpOffset2();
    waveParams[3] = GetWFFreqAmpOffset3();

    bool isRadial = GetWFIsRadial() > 0;
    for (int v = 0; v < meshData->GetVertexCount(); ++v)
    {
        const Math::Vec4& p4 = inputVertex[v].position;
        Math::Vec3 p3(p4.x,p4.y,p4.z);

        float t = Math::Dot(p3, timeDir);
        float d = 0.0f;
            
        for (int w = 0; w < NumOfWaves; ++w)
        {
            Math::Vec3& wave = waveParams[w];
            d += wave.y*(Math::Sin(t*wave.x + wave.z));
        }

        Math::Vec3 waveDispDir = displacementDir;
        if (isRadial)
        { 
            Math::Vec3 radialDisp = p3-t*timeDir;
            float radialDispLen = Math::Length(radialDisp);
            if (radialDispLen > PFLOAT_EPSILON)
            {
                radialDisp /= radialDispLen;
            }
            else
            {
                radialDisp = Math::Vec3(0.0f,0.0f,0.0f);
            }
            waveDispDir = radialDisp;
        }
        outputVertex[v].position = inputVertex[v].position + Math::Vec4(d*waveDispDir,0.0f);

        //todo, reconstruct normals
        outputVertex[v].normal = inputVertex[v].normal;
        outputVertex[v].uv = inputVertex[v].uv;
    }

    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}


}
}
