/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraLineBlock.cpp
//! \author	Kevin Boulanger
//! \date	31st August 2014
//! \brief	Timeline block moving the camera along a straight line

#include "Pegasus/TimelineBlock/Camera/CameraLineBlock.h"

namespace Pegasus {
namespace TimelineBlock {


CameraLineBlock::CameraLineBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
:   Timeline::Block(allocator, appContext)
{
}    

//----------------------------------------------------------------------------------------

CameraLineBlock::~CameraLineBlock()
{
}

//----------------------------------------------------------------------------------------

void CameraLineBlock::Initialize()
{
}

//----------------------------------------------------------------------------------------

void CameraLineBlock::Shutdown()
{
}

//----------------------------------------------------------------------------------------

void CameraLineBlock::Update(float beat, Wnd::Window * window)
{
    //! \todo Implement
    /******/
}

//----------------------------------------------------------------------------------------

void CameraLineBlock::Render(float beat, Wnd::Window * window)
{
    // Does not do anything, Update() takes care of the camera position
}


}   // namespace TimelineBlock
}   // namespace Pegasus
