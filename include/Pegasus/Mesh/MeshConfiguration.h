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

namespace Pegasus {
namespace Mesh {


//! Mesh configuration used by all mesh nodes, which must be compatible
//! between nodes to link them
class MeshConfiguration
{
public:

    //! Default constructor
    MeshConfiguration();

    //! Copy constructor
    //! \param other Other configuration to copy from
    explicit MeshConfiguration(const MeshConfiguration & other);

    //! Assignment operator
    //! \param other Other configuration to copy from
    MeshConfiguration & operator=(const MeshConfiguration & other);

    //! Test if an input mesh configuration is considered as compatible with the current one
    //! \warning This is important to test when linking Mesh, MeshGenerator and MeshOperator together
    //! \param configuration Configuration to test with
    //! \return True if the configurations are compatible
    bool IsCompatible(const MeshConfiguration & configuration) const;

    //------------------------------------------------------------------------------------
    
private:
};


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHCONFIGURATION_H
