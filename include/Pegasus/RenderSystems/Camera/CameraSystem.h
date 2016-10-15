/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraSystem.h
//! \author	Karolyn Boulanger
//! \date	04th September 2014
//! \brief	Camera manager, storing the global cameras

#ifndef PEGASUS_CAMERA_CAMERAMANAGER_H
#define PEGASUS_CAMERA_CAMERAMANAGER_H

#include "Pegasus/RenderSystems/Config.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA

#include "Pegasus/RenderSystems/System/RenderSystem.h"
#include "Pegasus/RenderSystems/Camera/Camera.h"
#include "Pegasus/Render/Render.h"

namespace Pegasus {
namespace Camera {

//! Camera manager, storing the global cameras
class CameraSystem : public RenderSystems::RenderSystem
{
public:

    //! Constructor
    //! \param allocator Allocator used for all camera allocations
    CameraSystem(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~CameraSystem();

    //! \return true if we want to create a new blockscript api, false otherwise. If true, then OnRegisterBlockscriptApi gets created, and the new lib gets passed.
    virtual bool CanCreateBlockScriptApi() const { return true; }

    //! GetSystemNode
    //! \return the name of this system. Will be used to get included in the blockscript lib.
    virtual const char* GetSystemName() const { return "CameraSystem"; }

    //! Internal function, called to initialize and load any render resources required.
    virtual void Load(Core::IApplicationContext* appContext);

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);

    virtual void OnRegisterShaderGlobalConstants(Utils::Vector<RenderSystem::ShaderGlobalConstantDesc>& outConstants);

    virtual void WindowUpdate(Wnd::Window* window);

    //API functions
    
    //! Binds a camera to a camera context.
    //! \param cam the camera to bind. Contains all transform data.
    //! \param context the context that will relate to this camera.
    void BindCamera(Camera* cam, CameraContext context = CAM_WORLD_CONTEXT);

    //! Switches the source context to dispatch the transforms to.
    //! \param context the enum of the context to use to dispatch such camera.
    void UseCameraContext(const CameraContext& context);

    //! Overrides the camera in the world context with the free cam.
    //! \param SetFreeCam - then the free cam is used on the world context. Meaning any camera being set through BindCamera
    //!                     with semantic world won't be used. To undo this, pass a null camera.
    void SetFreeCam(CameraRef freeCam);

    //! Gets a reference to the free cam.
    //! \param reference to the free cam for modifications.
    CameraRef GetFreeCam() { return mFreeCam; }

private:

    //! Allocator used for all camera allocations
    Alloc::IAllocator * mAllocator;

    //container vector with cameras per context
    Utils::Vector<CameraRef> mCameras;

    CameraContext mCurrentContext;

    Render::BufferRef mCameraBuffer;

    CameraRef mFreeCam;

    bool mCamStateDirty;
};


}   // namespace Camera
}   // namespace Pegasus

#endif // RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
#endif  // PEGASUS_CAMERA_CAMERAMANAGER_H
