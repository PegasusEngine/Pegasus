/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraLineBlock.cpp
//! \author	Kevin Boulanger
//! \date	31st August 2014
//! \brief	Timeline block moving the camera along a straight line

#include "Pegasus/Camera/Camera.h"
#include "Pegasus/Camera/CameraManager.h"
#include "Pegasus/TimelineBlock/Camera/CameraLineBlock.h"

namespace Pegasus {
namespace TimelineBlock {


CameraLineBlock::CameraLineBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext)
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
    Camera::Camera * camera = GetCameraManager()->GetCamera(Camera::CameraManager::CAMERATYPE_MAIN);
}

//----------------------------------------------------------------------------------------

void CameraLineBlock::Render(float beat, Wnd::Window * window)
{
    // Does not do anything, Update() takes care of the camera position
}


}   // namespace TimelineBlock
}   // namespace Pegasus
