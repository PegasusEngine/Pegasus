/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PsyBeadsBlock.h
//! \author	Kevin Boulanger
//! \date	10th January 2014
//! \brief	Timeline block for the PsyBeads effect (vertical columns with regular beads)

#ifndef TESTAPP1_PSYBEADS_BLOCK_H
#define TESTAPP1_PSYBEADS_BLOCK_H

#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"

//! Timeline block for the PsyBeads effect (vertical columns with regular beads)
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class PsyBeadsBlock : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(PsyBeadsBlock, "PsyBeads");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    PsyBeadsBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~PsyBeadsBlock();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Update the content of the block, called once at the beginning of each rendered frame
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Update(float beat, Pegasus::Wnd::Window * window);

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Render(float beat, Pegasus::Wnd::Window * window);

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(PsyBeadsBlock)

    Pegasus::Shader::ProgramLinkageRef mProgram;
    Pegasus::Mesh::MeshRef mQuad;

    Pegasus::Render::Buffer mStateBuffer;
    Pegasus::Render::Uniform mStateBufferUniform;
    PEGASUS_ALIGN_BEGIN(16)
    struct UniformState
    {
        float ratio;
        float time;
    } mState;
    PEGASUS_ALIGN_END(16);

    
};


#endif  // TESTAPP1_PSYBEADS_BLOCK_H
