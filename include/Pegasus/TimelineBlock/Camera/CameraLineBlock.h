/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraLineBlock.h
//! \author	Karolyn Boulanger
//! \date	31st August 2014
//! \brief	Timeline block moving the camera along a straight line

#ifndef PEGASUS_TIMELINEBLOCK_CAMERALINEBLOCK_H
#define PEGASUS_TIMELINEBLOCK_CAMERALINEBLOCK_H

#include "Pegasus/Timeline/Block.h"

namespace Pegasus {
namespace TimelineBlock {


//! Timeline block moving the camera along a straight line
class CameraLineBlock : public Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(CameraLineBlock, "CameraLine");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    CameraLineBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~CameraLineBlock();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Update the content of the block, called once at the beginning of each rendered frame
    //! \note Updates the position of the camera from the current beat
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    virtual void Update(float beat, Wnd::Window * window);

    //! Render the content of the block
    //! \note Does not do anything, Update() takes care of the camera position
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    virtual void Render(float beat, Wnd::Window * window);

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(CameraLineBlock)
};


}   // namespace TimelineBlock
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINEBLOCK_CAMERALINEBLOCK_H
