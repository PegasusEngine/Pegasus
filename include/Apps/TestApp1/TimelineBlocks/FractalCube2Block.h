/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCube2Block.h
//! \author	Karolyn Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube2 effect (colored fractal with shadows)

#ifndef TESTAPP1_FRACTALCUBE2_BLOCK_H
#define TESTAPP1_FRACTALCUBE2_BLOCK_H

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

//! Timeline block for the FractalCube2 effect (colored fractal with shadows)
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class FractalCube2Block : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(FractalCube2Block, "FractalCube2");

public:

    
    FractalCube2Block(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext * appContext);

    
    virtual ~FractalCube2Block();
    virtual void Initialize() override;
    virtual void Shutdown()  override;
    virtual void Update(const Pegasus::Timeline::UpdateInfo& updateInfo) override;
    virtual void Render(const Pegasus::Timeline::RenderInfo& renderInfo) override;

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(FractalCube2Block)

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
    Pegasus::Render::BlendingStateRef mCurrentBlockBlendingState;
    Pegasus::Render::BlendingStateRef mDefaultBlendingState;
};


#endif  // TESTAPP1_FRACTALCUBE2_BLOCK_H
