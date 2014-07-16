/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCube2Block.h
//! \author	Kevin Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube2 effect (colored fractal with shadows)

#ifndef TESTAPP1_FRACTALCUBE2_BLOCK_H
#define TESTAPP1_FRACTALCUBE2_BLOCK_H

#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"

//! Timeline block for the FractalCube2 effect (colored fractal with shadows)
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class FractalCube2Block : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(FractalCube2Block, "FractalCube2");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    FractalCube2Block(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~FractalCube2Block();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Render(float beat, Pegasus::Wnd::Window * window);

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(FractalCube2Block)

    Pegasus::Shader::ProgramLinkageRef mProgram;
    Pegasus::Mesh::MeshRef mQuad;
    Pegasus::Render::Uniform mTimeUniform;
    Pegasus::Render::Uniform mScreenRatioUniform;
    Pegasus::Render::BlendingState mCurrentBlockBlendingState;
    Pegasus::Render::BlendingState mDefaultBlendingState;
};


#endif  // TESTAPP1_FRACTALCUBE2_BLOCK_H
