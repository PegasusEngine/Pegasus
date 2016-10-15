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
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace Mesh {


MeshData::MeshData(const MeshConfiguration & configuration, Graph::Node::Mode mode, Alloc::IAllocator* allocator)
:   Graph::NodeData(allocator),
    mConfiguration(configuration),
    mIndexCount(0),
    mVertexCount(0),
    mMode(mode)
{
    
    //fill in stream strides
    const MeshInputLayout * inputLayout = configuration.GetInputLayout();
    for (int i = 0; i < inputLayout->GetAttributeCount(); ++i)
    {
        const MeshInputLayout::AttrDesc& desc = inputLayout->GetAttributeDesc(i);
        int size = desc.mByteSize;
        int prevStride = mVertexStreams[desc.mStreamIndex].GetStride();
        mVertexStreams[desc.mStreamIndex].SetStride(prevStride + size);
    }

    mIndexBuffer.SetStride(sizeof(unsigned short));

}

unsigned short MeshData::InternalPushVertex(const void * vertex, int streamId)
{   
    PG_ASSERTSTR(mMode == Graph::Node::STANDARD, "Function only available in mesh STANDARD mode.");
    PG_ASSERT(streamId < MESH_MAX_STREAMS);

    int newElementIndex = GetVertexCount();
    int stride = mVertexStreams[streamId].GetStride();
    int byteOffset = newElementIndex * stride;

    InternalAllocateVertexes(GetVertexCount() + 1, true);

    PG_ASSERT(mVertexStreams[streamId].GetByteSize() >= GetVertexCount() * stride);

    char * s = static_cast<char * >(GetStream<void>(streamId)) + byteOffset;

    Pegasus::Utils::Memcpy(s, vertex, stride);
    return static_cast<unsigned short>(newElementIndex);
    
}

void MeshData::PushIndex(unsigned short index)
{
    PG_ASSERTSTR(mMode == Graph::Node::STANDARD, "Function only available in mesh STANDARD mode.");
    int idxOffset = GetIndexCount();
    InternalAllocateIndexes(GetIndexCount() + 1, true);
    PG_ASSERT(mIndexBuffer.GetByteSize() >= GetIndexCount() * mIndexBuffer.GetStride());
    unsigned short * idxBuffer = GetIndexBuffer();
    idxBuffer[idxOffset] = index;
}

void MeshData::AllocateVertexes(int count)
{
    InternalAllocateVertexes(count, false);
}

void MeshData::AllocateIndexes(int count)
{
    InternalAllocateIndexes(count, false);
}

void MeshData::InternalAllocateVertexes(int count, bool preserveElements)
{
    mVertexCount = count;
    
    if (mMode == Graph::Node::STANDARD)
    {
        for (int stream = 0; stream < MESH_MAX_STREAMS; ++stream)
        {
            mVertexStreams[stream].Grow(GetAllocator(), count, preserveElements);        
        }
    }
}

void MeshData::InternalAllocateIndexes(int count, bool preserveElements)
{
    if (mConfiguration.GetIsIndexed())
    {
        mIndexCount = count;
        if (mMode == Graph::Node::STANDARD)
        {
            mIndexBuffer.Grow(GetAllocator(), count, preserveElements);
        }
    }
}

void MeshData::Clear()
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
    
    mVertexCount = 0;
    mIndexCount = 0;
}

MeshData::~MeshData()
{
    Clear();
}


MeshData::Stream::Stream()
    : mBuffer(nullptr), mStride(0), mByteSize(0)
{
}

void MeshData::Stream::Grow(Alloc::IAllocator * allocator, int count, bool preserveElements)
{
    if (mStride > 0)
    {
        const int MINIMUM_BYTE_GROWTH = 32 * mStride; //grow on 

        int newByteSize = ((count * mStride) / MINIMUM_BYTE_GROWTH + 1) * MINIMUM_BYTE_GROWTH;

        if (newByteSize > mByteSize || newByteSize < (mByteSize / 2))
        {
            char * newList = PG_NEW_ARRAY(allocator, -1, "MeshData::Stream[i].mBuffer", Alloc::PG_MEM_TEMP, char, newByteSize);
            if (mByteSize > 0)
            {
                if (preserveElements)
                {
                    int preserveSize = newByteSize < mByteSize ? newByteSize : mByteSize;
                    Pegasus::Utils::Memcpy(newList, mBuffer, preserveSize);
                }   
                Destroy(allocator);
            }
            mBuffer =  newList;
            mByteSize = newByteSize;
        }
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

