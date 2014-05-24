/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	KleberTriangleBlock.h
//! \author	Kevin Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the KleberHomogayTriangle effect

#ifndef TESTAPP1_KLEBERTRIANGLE_BLOCK_H
#define TESTAPP1_KLEBERTRIANGLE_BLOCK_H

#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"

//! Timeline block for the KleberHomogayTriangle effect
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class KleberTriangleBlock : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(KleberTriangleBlock, "BlobTriangle");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    KleberTriangleBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~KleberTriangleBlock();


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
    PG_DISABLE_COPY(KleberTriangleBlock)

    Pegasus::Shader::ProgramLinkageRef mProgram;
    Pegasus::Shader::ProgramLinkageRef mProgram2;
    Pegasus::Mesh::MeshRef mQuadMesh;
    Pegasus::Render::Uniform mTimeUniform;
};


#endif  // TESTAPP1_KLEBERTRIANGLE_BLOCK_H
