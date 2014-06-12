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
    mIndexCount(0),
    mVertexCount(0)
{
    
    //fill in stream strides
    const MeshInputLayout * inputLayout = configuration.GetInputLayout();
    for (int i = 0; i < inputLayout->GetAttributeCount(); ++i)
    {
        const MeshInputLayout::AttrDesc& desc = inputLayout->GetAttributeDesc(i);
        int size = MeshInputLayout::AttrTypeSizes[desc.mType] * desc.mAttributeTypeCount;
        int prevStride = mVertexStreams[desc.mStreamIndex].GetStride();
        mVertexStreams[desc.mStreamIndex].SetStride(prevStride + size);
    }

    mIndexBuffer.SetStride(sizeof(unsigned short));

}


void MeshData::AllocateVertexes(int count)
{
    mVertexCount = count;
    
    //allocate streams
    for (int stream = 0; stream < MESH_MAX_STREAMS; ++stream)
    {
        mVertexStreams[stream].Grow(GetAllocator(), count);        
    }

}

void MeshData::AllocateIndexes(int count)
{
    if (mConfiguration.GetIsIndexed())
    {
        mIndexBuffer.Grow(GetAllocator(), count);
    }
}

//----------------------------------------------------------------------------------------

MeshData::~MeshData()
{
    for (int s = 0; s < MESH_MAX_STREAMS; ++s)
    { 
        if (mVertexStreams[s].GetBuffer() != nullptr)
        {
            mVertexStreams[s].Destroy(GetAllocator());
        }
    }
    
    if (mIndexBuffer.GetBuffer() != nullptr)
    {
        mIndexBuffer.Destroy(GetAllocator());
    }
}


MeshData::Stream::Stream()
    : mBuffer(nullptr), mStride(0), mByteSize(0)
{
}

void MeshData::Stream::Grow(Alloc::IAllocator * allocator, int count)
{
    int newByteSize = count * mStride;
    if (newByteSize > mByteSize || newByteSize < (mByteSize / 2))
    {
         if (mByteSize > 0)
         {
            Destroy(allocator);
         }
         mBuffer = PG_NEW_ARRAY(allocator, -1, "MeshData::Stream[i].mBuffer", Alloc::PG_MEM_TEMP, char, newByteSize);
         mByteSize = newByteSize;
    }
}

void MeshData::Stream::Destroy(Alloc::IAllocator * allocator)
{
    PG_ASSERT(mBuffer != nullptr);
    PG_DELETE_ARRAY(allocator, mBuffer); 
    mBuffer = nullptr;
    mByteSize = 0;
}

MeshData::Stream::~Stream()
{
    PG_ASSERTSTR(mBuffer == nullptr, "Destroy must be called explicitely on this stream");
}

}   // namespace Mesh
}   // namespace Pegasus

