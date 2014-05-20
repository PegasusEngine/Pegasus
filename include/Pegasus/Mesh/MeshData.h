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
#include "Pegasus/Mesh/MeshConfiguration.h"

namespace Pegasus {
namespace Mesh {


//! Mesh node data, used by all mesh nodes, including generators and operators
class MeshData : public Graph::NodeData
{
public:

    //! Default constructor
    //! \param configuration Configuration of the mesh
    //! \param allocator Allocator used for the node data
    //! \note Sets the dirty flag
    MeshData(const MeshConfiguration & configuration, Alloc::IAllocator* allocator);

    //! Get the configuration of the mesh data
    //! \return Configuration of the mesh data
    inline const MeshConfiguration & GetConfiguration() const { return mConfiguration; }

    //! Gets the stream, casted properly
    template <class T> T * GetStream(int stream);    
    
    //! Gets the stream, as a void pointer
    template <> void * GetStream(int stream);    

    //! Gets the stride size count of the stream
    //! \param i the stream index
    int GetStreamStride(int i) const { return mVertexStreamStrides[i]; };

    //! Convinience function that returns the total byte size of a stream
    int GetStreamByteSize(int i) const { return mVertexStreamStrides[i] * mConfiguration.GetVertexCount(); }
    
protected:

    //! Destructor
    virtual ~MeshData();

    //------------------------------------------------------------------------------------
    
private:

    // Node data cannot be copied, only references to them
    PG_DISABLE_COPY(MeshData)

    //! Configuration of the mesh data
    MeshConfiguration mConfiguration;
    void  * mVertexStreams[MESH_MAX_STREAMS];
    short * mIndexBuffer;
    int     mVertexStreamStrides[MESH_MAX_STREAMS];
};


template<class T>
T * MeshData::GetStream(int stream)
{
    PG_ASSERTSTR(sizeof(T) == mVertexStreamStrides[stream], "stream strides must match!");
    return static_cast<T*>(mVertexStreams[stream]);
}
template<>
void * MeshData::GetStream(int stream)
{
    return (mVertexStreams[stream]);
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
