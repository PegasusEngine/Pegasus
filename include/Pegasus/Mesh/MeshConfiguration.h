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
        LINE,
        LINE_STRIP,
        POINT,
        PRIMITIVE_COUNT
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
    
    //! Gets wether this mesh is static or dynamic
    bool    GetIsDynamic() const { return mIsDynamic; }

    //! Gets wether this mesh supports draw indirect.
    bool    GetIsDrawIndirect() const { return mIsDrawIndirect; }

    //! Gets the primitive type for this mesh
    MeshPrim GetMeshPrimitiveType() const { return mPrimitiveType; }

    //! Sets wether this mesh is indexed or not
    void    SetIsIndexed(bool isIndexed) { mIsIndexed = isIndexed; }

    //! Sets wether this mesh is dynamic or static
    void    SetIsDynamic(bool isDynamic) { mIsDynamic = isDynamic; }

    //! Draw indirect is for meshes that expect their draw parameters be filled by a compute shader.
    //! To access the indirect buffer created, use the Render API
    void    SetIsDrawIndirect(bool isDrawIndirect) { mIsDrawIndirect = isDrawIndirect; }

    //! Sets the primitive type for this mesh
    void    SetMeshPrimitiveType(MeshPrim primitiveType) { mPrimitiveType = primitiveType; }

private:
    //! boolean that determines if this mesh is indexed or not
    bool     mIsIndexed;

    //! boolean that determines if the buffer is dynamic (for draw)
    bool     mIsDynamic;

    //! Does this mesh has support for draw indirect.
    bool     mIsDrawIndirect;

    //! the primitive type
    MeshPrim mPrimitiveType;

    //! the input layout
    MeshInputLayout mInputLayout;
    
};


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHCONFIGURATION_H
