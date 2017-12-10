/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AtmosLookUpGenerator.h
//! \author Arthur Homs
//! \date   Septenber 2nd, 2017
//! \brief  Atmosphere look up generator.

#ifndef PEGASUS_ATMOS_LOOK_UP_GENERATOR
#define PEGASUS_ATMOS_LOOK_UP_GENERATOR

#include "Pegasus/RenderSystems/Config.h"
#if 0 //RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/RenderSystems/Atmos/AtmosSystem.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace RenderSystems
{

//! Mesh cube generator. Generates a cube
class AtmosLookUpGenerator
{
    DECLARE_MESH_GENERATOR_NODE(AtmosLookUpGenerator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(AtmosLookUpGenerator, MeshGenerator)
        DECLARE_PROPERTY(float, GenerationSetting, 0.0f)
    END_DECLARE_PROPERTIES()

public:

    //! Cube generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
	AtmosLookUpGenerator(Pegasus::Alloc::IAllocator* nodeAllocator,
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~AtmosLookUpGenerator(){}

    //! Compute mode.
    virtual Graph::Node::Mode GetMode() const { return Graph::Node::COMPUTE; }

    //! Override of Update
    virtual bool Update();

    //! creates internally render resources if they have not been created.
    void CreateComputeResources();


protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

private:
    bool mAreComputeResourcesAllocated;

    Render::BufferRef mLookUpUniformBuffer;

    Render::BufferRef mIndexBuffer;
    Render::BufferRef mVertexBuffer;

    RenderSystems::AtmosSystem::AtmosResources mResources;
    int mProgramVersions[AtmosSystem::PROGRAM_COUNT];
};


}
}

#endif
#endif
