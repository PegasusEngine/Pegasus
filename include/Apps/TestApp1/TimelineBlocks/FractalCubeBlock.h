/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCubeBlock.h
//! \author	Karolyn Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube effect (grayscale fractal with no shadow)

#ifndef TESTAPP1_FRACTALCUBE_BLOCK_H
#define TESTAPP1_FRACTALCUBE_BLOCK_H

#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"

namespace Pegasus {
    namespace Timeline {
        struct UpdateInfo;
        struct RenderInfo;
    }
}

#if PEGASUS_ENABLE_RENDER_API

//! Timeline block for the FractalCube effect (grayscale fractal with no shadow)
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class FractalCubeBlock : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(FractalCubeBlock, "FractalCube");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    FractalCubeBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~FractalCubeBlock();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    virtual void Update(const Pegasus::Timeline::UpdateInfo& updateInfo) override;
    virtual void Render(const Pegasus::Timeline::RenderInfo& renderInfo) override;

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(FractalCubeBlock)

    Pegasus::Shader::ProgramLinkageRef mProgram;
    Pegasus::Mesh::MeshRef mQuad;

    Pegasus::Render::BufferRef mStateBuffer;
    Pegasus::Render::Uniform mStateBufferUniform;
    PEGASUS_ALIGN_BEGIN(16)
    struct UniformState
    {
        float ratio;
        float time;
    } mState;
    PEGASUS_ALIGN_END(16);
};

#endif
#endif  // TESTAPP1_FRACTALCUBE_BLOCK_H
