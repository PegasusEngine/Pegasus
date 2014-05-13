/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshData.cpp
//! \author	Kleber Garcia
//! \date	01st November 2013
//! \brief	Mesh node data, used by all mesh nodes, including generators and operators

#include "Pegasus/Mesh/MeshData.h"

namespace Pegasus {
namespace Mesh {


MeshData::MeshData(const MeshConfiguration & configuration, Alloc::IAllocator* allocator)
:   Graph::NodeData(allocator),
    mConfiguration(configuration),
    mIndexBuffer(nullptr)
{
    //initialize stream pool
    for (int i = 0; i < MESH_MAX_STREAMS; ++i)
    {
        mVertexStreams[i] = nullptr; 
        mVertexStreamStrides[i] = 0;
    }
    
    //fill in stream strides
    const MeshInputLayout * inputLayout = configuration.GetInputLayout();
    for (int i = 0; i < inputLayout->GetAttributeCount(); ++i)
    {
        const MeshInputLayout::AttrDesc& desc = inputLayout->GetAttributeDesc(i);
        int size = MeshInputLayout::AttrTypeSizes[desc.mType];
        mVertexStreamStrides[desc.mStreamIndex] += size;
    }

    //allocate streams
    for (int stream = 0; stream < MESH_MAX_STREAMS; ++stream)
    {
        int streamSize = mVertexStreamStrides[stream] * configuration.GetVertexCount();
        if (streamSize > 0)
        {
            mVertexStreams[stream] = PG_NEW_ARRAY(GetAllocator(), -1, "MeshData::mVertexStream[i]", Alloc::PG_MEM_TEMP, char, streamSize);
        }
    }

    if (configuration.GetIsIndexed())
    {
        mIndexBuffer = PG_NEW_ARRAY(GetAllocator(), -1, "MeshData::mIndexBuffer", Alloc::PG_MEM_TEMP, short, configuration.GetIndexCount());
    }
}

//----------------------------------------------------------------------------------------

MeshData::~MeshData()
{
    for (int s = 0; s < MESH_MAX_STREAMS; ++s)
    { 
        if (mVertexStreams[s] != nullptr)
        {
            PG_DELETE_ARRAY(GetAllocator(), (char*)mVertexStreams[s]);
        }
    }
    
    if (mIndexBuffer != nullptr)
    {
        PG_DELETE_ARRAY(GetAllocator(), mIndexBuffer);
    }
}


}   // namespace Mesh
}   // namespace Pegasus
