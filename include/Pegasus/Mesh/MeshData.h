/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshData.h
//! \author	Kleber Garcia
//! \date	May 11th 2014
//! \brief	Mesh node data, used by all mesh nodes, including generators and operators

#ifndef PEGASUS_MESH_MESHDATA_H
#define PEGASUS_MESH_MESHDATA_H

#include "Pegasus/Core/Ref.h"
#include "Pegasus/Graph/NodeData.h"
#include "Pegasus/Graph/Node.h"
#include "Pegasus/Mesh/MeshConfiguration.h"

namespace Pegasus {
namespace Mesh {


//! Mesh node data, used by all mesh nodes, including generators and operators
class MeshData : public Graph::NodeData
{
public:

    //! Default constructor
    //! \param configuration Configuration of the mesh
    //! \param mode the mode of this mesh data. Most of functions only work under STANDARD mode.
    //! \param allocator Allocator used for the node data
    //! \note Sets the dirty flag
    MeshData(const MeshConfiguration & configuration, Graph::Node::Mode mode, Alloc::IAllocator* allocator);

    //! Get the configuration of the mesh data
    //! \return Configuration of the mesh data
    inline const MeshConfiguration & GetConfiguration() const { return mConfiguration; }

    Graph::Node::Mode GetMode() const { return mMode; }

    //! Gets the stream, casted properly
    //! \param streamId the id of the stream
    //! \return the actual stream memory
    template <class T> 
    T * GetStream(int streamId);    
    
    //! Gets the stream, as a void pointer
    //! \param streamId the id of the stream
    //! \return the actual stream memory
    template <> void * GetStream(int streamId);    

    //! Pushes (and does respective allocations) a vertex element
    //! \param vertex the vertex structure to push
    //! \param streamId the target stream to set this vertex element to
    //! \return the new index
    template<class T>
    unsigned short PushVertex(const T * vertex, int streamId);

    //! Pushes (and does respective allocations) an index element
    //! \param index the index to push
    void PushIndex(unsigned short index);

    //! Gets the stride size count of the stream
    //! \param i the stream index
    int GetStreamStride(int i) const { return mVertexStreams[i].GetStride(); };

    //! Convenience function that returns the total byte size of a stream
    //! \param i the identifier for the stream to get the size from
    //! \return the byte size
    int GetStreamByteSize(int i) const { return mVertexStreams[i].GetByteSize(); }

    //! Gets the index buffer reference
    //! \return  the index buffer pointer
    unsigned short * GetIndexBuffer() { PG_ASSERTSTR(mMode == Graph::Node::STANDARD, "Function only available in mesh STANDARD mode."); return static_cast<unsigned short*>(mIndexBuffer.GetBuffer()); }

    //! Gets the vertex count
    //! \return the count of vertex elements
    int GetVertexCount() const {return mVertexCount; }

    //! Gets the index buffer count
    //! \return the count of index buffers elements
    int GetIndexCount() const { return mIndexCount; }

    //! Allocates vertex buffer elements.
    //! \param count the number of vertices to allocate
    void AllocateVertexes(int count);

    //! Allocates index buffer elements
    //! \param count the number of indices to allocate
    void AllocateIndexes(int count);

    //! Destroys all internal data and initializes this mesh data as completely new
    void Clear();
    
protected:

    //! Destructor
    virtual ~MeshData();

    //! Destroys the vertex streams
    void DestroyVertexStreams();

    //! Destroys the index buffer
    void DestroyIndexBuffer();

    //------------------------------------------------------------------------------------
    
private:
    //! Pushes (and does respective allocations) a vertex element
    //! \param vertex the vertex structure to push
    //! \param streamId the target stream to set this vertex element to
    //! \return the new index
    unsigned short InternalPushVertex(const void * vertex, int streamId);

    //! internally allocates vertices if necessary
    //! \param count new count of elements
    //! \param preserveElements if true, the old elements are copied (truncated if necessary) to
    //!        the new buffer
    void InternalAllocateVertexes(int count, bool preserveElements);

    //! internally allocates indices if necessary
    //! \param count new count of elements
    //! \param preserveElements if true, the old elements are copied (truncated if necessary) to
    //!        the new buffer
    void InternalAllocateIndexes(int count, bool preserveElements);

    //!helper class, encoding a stream buffer of bytes
    class Stream
    {
    public:
        Stream();
        ~Stream();

        //! returns the stride
        int GetStride() const { return mStride; }

        //! returns the byte size of this stream
        int GetByteSize() const { return mByteSize; }
        
        //! returns the actual buffer of this stream
        void* GetBuffer() { return mBuffer; }

        //! sets the stride of this stream
        void SetStride(int stride) { mStride = stride; }

        //! attempts to grow the stream. If the space is half as big then it is deallocated.
        //! if the requested size is different the stream size is grown. 
        //! \param  allocator the allocator for memory management
        //! \param  the vertex size to attempt to grow to. count * mStride is the total byte size
        //! \param  preserveElements  copy previous elements or allocate new ones if needed
        //! \note after calling Grow, the memory returned from GetBuffer is uninitialized.
        void Grow(Pegasus::Alloc::IAllocator * allocator, int count, bool preserveElements);

        //! Destroys whatever dynamic memory is inside this stream.
        //! call this before deleting the class. Failing to do so will cause an assert and a leak
        //! on the streams destructor
        void Destroy(Pegasus::Alloc::IAllocator * allocator);
        
    private:
        char * mBuffer;
        int    mStride;
        int    mByteSize;
    };

    // Node data cannot be copied, only references to them
    PG_DISABLE_COPY(MeshData)

    //! Configuration of the mesh data
    MeshConfiguration mConfiguration;

    //! Stream list of this mesh
    Stream  mVertexStreams[MESH_MAX_STREAMS];

    //! the index buffer of this mesh
    Stream  mIndexBuffer;
    
    //! total count of vertices
    int mVertexCount;

    //! total count of indices
    int mIndexCount;

    // mode of mesh data.
    Graph::Node::Mode mMode;
};


template<class T>
T * MeshData::GetStream(int streamId)
{
    PG_ASSERTSTR(sizeof(T) == mVertexStreams[streamId].GetStride(), "stream strides must match!");
    return static_cast<T*>(mVertexStreams[streamId].GetBuffer());
}

template<>
void * MeshData::GetStream(int stream)
{
    return (mVertexStreams[stream].GetBuffer());
}

template<class T>
unsigned short MeshData::PushVertex(const T * vertex, int streamId)
{
    PG_ASSERTSTR(sizeof(T) == mVertexStreams[streamId].GetStride(), "stream strides must match!");
    return InternalPushVertex(static_cast<const void *>(vertex), streamId);
}

//----------------------------------------------------------------------------------------

//! Reference to a MeshData, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<MeshData>   MeshDataRef;

//! Const reference to a reference to a MeshData, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<MeshData> & MeshDataIn;

//! Reference to a reference to a MeshData, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<MeshData> & MeshDataInOut;

//! Reference to a MeshData, typically used as the return value of a function
typedef       Pegasus::Core::Ref<MeshData>   MeshDataReturn;


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHDATA_H
