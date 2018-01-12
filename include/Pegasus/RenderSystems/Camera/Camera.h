/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Camera.h
//! \author	Karolyn Boulanger
//! \date	01st September 2014
//! \brief	Camera object, storing the settings of one camera in the scene

#ifndef PEGASUS_CAMERA_CAMERA_H
#define PEGASUS_CAMERA_CAMERA_H

#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/PropertyGrid/PropertyGridEnumType.h"
#include "Pegasus/RenderSystems/Config.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Math/Matrix.h"
#include "Pegasus/Math/Plane.h"
#include "Pegasus/Core/Ref.h"
#include "Pegasus/Utils/Vector.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA

namespace Pegasus {


namespace Alloc
{
    class IAllocator;
}

namespace Camera {

BEGIN_DECLARE_ENUM(CameraContext)
    DECLARE_ENUM(CameraContext, CAM_WORLD_CONTEXT)  //!> world context, used to render into the world, can get overriden by freecam.
    DECLARE_ENUM(CameraContext, CAM_OFFSCREEN_CONTEXT) //!> offscreen context, use it for reflections / offscreen passes.
    DECLARE_ENUM(CameraContext, CAM_SHADOW_CONTEXT)  //!> use for setting shadow context. 
    DECLARE_ENUM(CameraContext, CAM_CONTEXT_COUNT) //!> do not use to set things.
END_DECLARE_ENUM()

//! Camera object, storing the settings of one camera in the scene
class Camera : public Application::GenericResource
{
    BEGIN_DECLARE_PROPERTIES(Camera, GenericResource)
        //! Position of the camera in world space.
        DECLARE_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 0.0f, 0.0f))
        //! Direction of the camera in world space. Value is set automatically if Target is enabled.
        DECLARE_PROPERTY(Math::Vec3, Dir, Math::Vec3(0.0f, 0.0f, -1.0f))
        //! For target mode only, rotation, x y and z, and w as a quaternion.
        DECLARE_PROPERTY(Math::Vec3, TargetViewRotation, Math::Vec3(0.0f, 0.0f, 1.0f))
        //! Target value for camera to follow
        DECLARE_PROPERTY(Math::Vec3, Target, Math::Vec3(0.0f, 0.0f, 0.0f))
        //! Distance to target. Must be positive.
        DECLARE_PROPERTY(float, TargetDistance, 1.0f)
        //! Near plane distance in view space
        DECLARE_PROPERTY(float, Near, 0.05f)
        //! far plane distance in view space
        DECLARE_PROPERTY(float, Far, 200.0f)
        //! field of view, in radiance (half angle)
        DECLARE_PROPERTY(float, Fov, 0.6f)
        //! aspect ratio (y / x), use -1 to set it to automatic (current viewport's)
        DECLARE_PROPERTY(float, Aspect, -1.0f)
        //! If true, Position and Dir are calculated from the Target values.
        DECLARE_PROPERTY(int, EnableTarget, 0)
        //! If true, this becomes an orthographic matrix.
        DECLARE_PROPERTY(int, EnableOrtho, 0)
    END_DECLARE_PROPERTIES()

    //------------------------------------------------------------------------------------
        
public:

    //! Constructor
    Camera(Alloc::IAllocator* allocator);

    //! Destructor
    ~Camera();

    //! Structure explaining the gpu data of a camera
    struct GpuCamData
    {
        Math::Mat44 view;
        Math::Mat44 invView;
        Math::Mat44 proj;
        Math::Mat44 viewProj;
        Math::Mat44 invViewProj;
        Math::Vec4  camPos;
        Math::Vec4  camDir;
    };

    enum PlaneId
    {
        NEAR,
        FAR,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        MAX_PLANE_COUNT
    };

    enum PointId
    {
        N_L_T,
        N_R_T,
        N_L_B,
        N_R_B,
        F_L_T,
        F_R_T,
        F_L_B,
        F_R_B,
        MAX_POINTS
    };

    enum CollisionState
    {
        INTERSECT,
        INSIDE,
        OUTSIDE
    };

    struct Frustum
    {
        Math::Plane planes[MAX_PLANE_COUNT];
        Math::Vec3 points[MAX_POINTS];
        CollisionState GetCollisionState(const Math::Vec3& aabbMin, const Math::Vec3& aabbMax) const;
    };

    //! Gets the camera world space frustum
    const Frustum& GetWorldFrustum() const { return mFrustum; }

    //! function that updates the corresponding camera data.
    virtual void Update();

    //! Force reupdate of internal matrices of this camera. Use this to compute the GpuCamData if there is no viewport at hand.
    //! This function is normally not used, the system uses Update and WindowUpdate respectively
    void ForceUpdate();

    //! window update.
    //! \param width, the width of the window t
    //! \param heigth, the height of the window to update. Ignored if aspect is > 0.0f
    //! \return true if there was a flush of gpu data. False otherwise.
    bool WindowUpdate(unsigned int width, unsigned int height);

    //! Returns a reference to the camera's gpu data.
    GpuCamData& GetGpuData() { return mGpuData; }

#if PEGASUS_ENABLE_PROXIES
    //! Set if this cam is displayed on debug mode (camera frustum lines)
    void SetShowCameraOnDebugMode(bool showCamOnDebugMode) { mShowCamOnDebugMode = showCamOnDebugMode; }

    //! Get if this cam is displayed on debug mode (camera frustum lines)
    bool GetShowCameraOnDebugMode() const { return mShowCamOnDebugMode; }

    static Camera* GetCameraInstance(unsigned int instanceId);
    static unsigned int GetCameraCount();
#endif

private:
    GpuCamData mGpuData;
    float mViewportAspect; 
    bool mProjDirty;
    unsigned int mCachedWidth;
    unsigned int mCachedHeight;
#if PEGASUS_ENABLE_PROXIES
    static Utils::Vector<Camera*> sCameraInstances;
    bool mShowCamOnDebugMode;
#endif

    Frustum mFrustum;

};

typedef Pegasus::Core::Ref<Camera> CameraRef;


}   // namespace Camera
}   // namespace Pegasus

#endif // RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
#endif  // PEGASUS_CAMERA_CAMERA_H
