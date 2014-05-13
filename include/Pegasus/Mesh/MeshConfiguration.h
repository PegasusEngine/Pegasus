/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshConfiguration.h
//! \author	Kleber Garcia
//! \date   11th May 2014	
//! \brief	Mesh configuration used by all mesh nodes, which must be compatible
//!         between nodes to link them

#ifndef PEGASUS_MESH_MESHCONFIGURATION_H
#define PEGASUS_MESH_MESHCONFIGURATION_H

#include "Pegasus/Mesh/MeshInputLayout.h"

//Increase this number if we are to use more than 32 attributes
#define MESH_MAX_ATTRIBUTES 32

namespace Pegasus {
namespace Mesh {


//! Mesh configuration used by all mesh nodes, which must be compatible
//! between nodes to link them
class MeshConfiguration
{
public:
    //! the primitive type of this mesh
    enum MeshPrim
    {
        TRIANGLE,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        QUAD,
        QUAD_STRIP,
        LINE,
        LINE_STRIP,
        POINT,
    };

    //! Default constructor
    //! Creates a default mesh configuration. A default mesh configuration is empty and requires arguments inserted to it.
    MeshConfiguration();

    //! Copy constructor
    //! \param other Other configuration to copy from
    explicit MeshConfiguration(const MeshConfiguration & other);

    //! Assignment operator
    //! \param other Other configuration to copy from
    MeshConfiguration & operator=(const MeshConfiguration & other);

    //! Get the input layout. Use this function also to alter the input layout
    //! \return the input layout to use or to edit
    MeshInputLayout * GetInputLayout() { return &mInputLayout; }
    
    //! Get the input layout. Use this function ato get a read reference of an input layout
    //! \return the input layout to use or to edit
    const MeshInputLayout * GetInputLayout() const { return &mInputLayout; }

    //! Gets wether this mesh is indexed
    bool    GetIsIndexed() const { return mIsIndexed; }
    
    //! Gets the vertex count for this mesh
    int     GetVertexCount() const { return mVertexCount; }

    //! Gets the index count for this mesh, if is indexed
    int     GetIndexCount() const { return mIndexCount; }

    //! Gets the primitive type for this mesh
    MeshPrim GetMeshPrimitiveType() const { return mPrimitiveType; }

    //! Sets wether this mesh is indexed or not
    void    SetIsIndexed(bool isIndexed) { mIsIndexed = isIndexed; }
    
    //! Sets the vertex count for this mesh config
    void    SetVertexCount(int vertexCount) { mVertexCount = vertexCount; }

    //! Sets the index count for this mesh config
    void    SetIndexCount(int indexCount) { mIndexCount = indexCount; }

    //! Sets the primitive type for this mesh
    void    SetMeshPrimitiveType(MeshPrim primitiveType) { mPrimitiveType = primitiveType; }

private:
    //! boolean that determines if this mesh is indexed or not
    bool     mIsIndexed;

    //! the vertex count
    int      mVertexCount;

    //! the index count
    int      mIndexCount;

    //! the primitive type
    MeshPrim mPrimitiveType;

    //! the input layout
    MeshInputLayout mInputLayout;
    
};


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHCONFIGURATION_H
