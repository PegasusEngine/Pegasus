/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Camera.cpp
//! \author	Karolyn Boulanger
//! \date	01st September 2014
//! \brief	Camera object, storing the settings of one camera in the scene

#include "Pegasus/RenderSystems/Camera/Camera.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA

#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Math/Quaternion.h"

namespace Pegasus {
namespace Camera {

#if PEGASUS_ENABLE_PROXIES
Utils::Vector<Camera*> Camera::sCameraInstances;
#endif

BEGIN_IMPLEMENT_ENUM(CameraContext)
    IMPLEMENT_ENUM(CameraContext, CAM_WORLD_CONTEXT)
    IMPLEMENT_ENUM(CameraContext, CAM_OFFSCREEN_CONTEXT)
    IMPLEMENT_ENUM(CameraContext, CAM_SHADOW_CONTEXT)
    IMPLEMENT_ENUM(CameraContext, CAM_CONTEXT_COUNT)
END_IMPLEMENT_ENUM()

BEGIN_IMPLEMENT_PROPERTIES(Camera)
    IMPLEMENT_PROPERTY(Camera, Position)
    IMPLEMENT_PROPERTY(Camera, Dir)
    IMPLEMENT_PROPERTY(Camera, TargetViewRotation)
    IMPLEMENT_PROPERTY(Camera, Target)
    IMPLEMENT_PROPERTY(Camera, TargetDistance)
    IMPLEMENT_PROPERTY(Camera, Near)
    IMPLEMENT_PROPERTY(Camera, Far)
    IMPLEMENT_PROPERTY(Camera, Fov)
    IMPLEMENT_PROPERTY(Camera, Aspect)
    IMPLEMENT_PROPERTY(Camera, EnableTarget)
    IMPLEMENT_PROPERTY(Camera, EnableOrtho)
END_IMPLEMENT_PROPERTIES(Camera)

//----------------------------------------------------------------------------------------

Camera::Camera(Alloc::IAllocator* allocator)
    : Application::GenericResource(allocator), mProjDirty(true), mViewportAspect(-1.0), mCachedWidth(0), mCachedHeight(0)
{
    BEGIN_INIT_PROPERTIES(Camera)
        INIT_PROPERTY(Position)
        INIT_PROPERTY(Dir)
        INIT_PROPERTY(TargetViewRotation)
        INIT_PROPERTY(Target)
        INIT_PROPERTY(TargetDistance)
        INIT_PROPERTY(Near)
        INIT_PROPERTY(Far)
        INIT_PROPERTY(Fov)
        INIT_PROPERTY(Aspect)
        INIT_PROPERTY(EnableTarget)
        INIT_PROPERTY(EnableOrtho)
    END_INIT_PROPERTIES()
#if PEGASUS_ENABLE_PROXIES
    sCameraInstances.PushEmpty() = this;
    mShowCamOnDebugMode = true;
#endif
}

Camera::~Camera()
{
#if PEGASUS_ENABLE_PROXIES
    for (unsigned int i = 0; i < sCameraInstances.GetSize(); ++i)
    {
        if (sCameraInstances[i] == this)
        {
            sCameraInstances.Delete(i);
            break;
        }
    }
#endif
}

#if PEGASUS_ENABLE_PROXIES
    Camera* Camera::GetCameraInstance(unsigned int instanceId)
    {
        if (instanceId < sCameraInstances.GetSize())
        {
            return sCameraInstances[instanceId];
        }
        return nullptr;
    }

    unsigned int Camera::GetCameraCount()
    {
        return sCameraInstances.GetSize();
    }
#endif

static void SetViewMat(Math::Mat44& mat, Math::Vec3& p, Math::Vec3& up, Math::Vec3& z)
{    
    float compare = Math::Dot(z,up);
    if (Math::Abs(compare) > 0.99f)
    {
        up = Math::Vec3(0.0f,0.0f, compare > 0.0f ? -1.0f : 1.0f);
    }
    z = -z; //flip z axis
    Math::Vec3 x = Math::Cross(up,z);    
    Math::Normalize(x);
    Math::Vec3 y = Math::Cross(z,x);
    Math::Normalize(y);

    mat.m11 = x.x;     mat.m12 = x.y;     mat.m13 = x.z;     mat.m14 = 0.0f;
    mat.m21 = y.x;     mat.m22 = y.y;     mat.m23 = y.z;     mat.m24 = 0.0f;
    mat.m31 = z.x;     mat.m32 = z.y;     mat.m33 = z.z;     mat.m34 = 0.0f;
    mat.m41 = 0.0f;    mat.m42 = 0.0f;    mat.m43 = 0.0f;    mat.m44 = 1.0f;

    Math::Vec4 camSpacePos, p4 = Math::Vec4(p,0.0f);
    Math::Mult44_41(camSpacePos, mat, p4);

    mat.m14 = -camSpacePos.x;
    mat.m24 = -camSpacePos.y;
    mat.m34 = -camSpacePos.z;
}


void Camera::Update()
{
    if (IsPropertyGridDirty())
    {
        Math::Vec3 pos = GetPosition();
        Math::Vec3 camDir = GetDir();
        Math::Vec3 up = Math::Vec3(0.0f,1.0f,0.0f);

        if (GetEnableTarget())
        {
            const Math::Vec3& rot = GetTargetViewRotation();
            Math::Quaternion xQ;
            Math::AxisAngleToQuaternion(xQ, Math::Vec3(0.0f, 1.0f, 0.0f), rot.x);
            Math::Quaternion yQ;
            Math::AxisAngleToQuaternion(yQ, Math::Vec3(1.0f, 0.0f, 0.0f), rot.y);
            camDir = Math::Vec3(0.0f,0.0f,-1.0f);
            Math::Quaternion finalQ = xQ;
            Math::RightMult(finalQ, yQ);
            RotateVector(camDir, finalQ);
            RotateVector(up, finalQ);
            pos = GetTarget() - GetTargetDistance()*camDir;
            SetDir(camDir);
            SetPosition(pos);
        }

        mGpuData.camPos = Math::Vec4(pos, 1.0f);

        Math::Normalize(camDir);
        mGpuData.camDir = Math::Vec4(camDir, 0.0f);

        SetViewMat(mGpuData.view, pos, up, camDir);
        Math::InverseHomogeneous(mGpuData.invView, mGpuData.view);

        mProjDirty = true;
        ValidatePropertyGrid();
    }
}

bool Camera::WindowUpdate(unsigned int width, unsigned int height)
{
    float requestedAspect = (float)height/(float)width;
    mCachedWidth = width;
    mCachedHeight = height;
    if (GetAspect() > 0.0)
    {
        requestedAspect = GetAspect();
    }
    if (Math::Abs(requestedAspect - mViewportAspect) > 0.001f || mProjDirty)
    {
        mViewportAspect = requestedAspect;
        if (!GetEnableOrtho())
        {
            Math::SetProjection(mGpuData.proj, GetFov(), mViewportAspect, GetNear(), GetFar());
        }
        else
        {
            float aspectHeight = mViewportAspect * (float)width;
            float hHeight = 0.5f * (float)aspectHeight;
            float hWidth = 0.5f*(float)height;
            Math::SetProjection(mGpuData.proj, -hWidth, hWidth, hHeight, -hHeight, GetNear(), GetFar(), /*Ortho*/true);
        }

        Math::Mult44_44(mGpuData.viewProj, mGpuData.proj, mGpuData.view);
        Math::Inverse(mGpuData.invViewProj, mGpuData.viewProj);

        //create frustum world points. Must match PointId
        static const Math::Vec4 sHomogeneousPoints[MAX_POINTS] = {
            Math::Vec4(-1.0f, 1.0f,0.0f,1.0f), //N_L_T
            Math::Vec4( 1.0f, 1.0f,0.0f,1.0f), //N_R_T
            Math::Vec4(-1.0f,-1.0f,0.0f,1.0f), //N_L_B
            Math::Vec4( 1.0f,-1.0f,0.0f,1.0f), //N_R_B
            Math::Vec4(-1.0f, 1.0f,1.0f,1.0f), //F_L_T
            Math::Vec4( 1.0f, 1.0f,1.0f,1.0f), //F_R_T
            Math::Vec4(-1.0f,-1.0f,1.0f,1.0f), //F_L_B
            Math::Vec4( 1.0f,-1.0f,1.0f,1.0f)  //F_R_B
        };

        for (unsigned int i = 0; i < MAX_POINTS; ++i)
        {
            Math::Vec4 pointOut;
            Math::Mult44_41(pointOut, mGpuData.invViewProj, sHomogeneousPoints[i]);
            pointOut /= pointOut.w;
            Math::Vec3& p = mFrustum.points[i];
            p.x = pointOut.x;
            p.y = pointOut.y;
            p.z = pointOut.z;
        }
        //plane normal computation, using triangles in correct order
        static const PointId sNormalOrder[MAX_PLANE_COUNT][3] = {
            {N_L_B, N_R_B, N_R_T}, //NEAR             
            {F_R_T, F_R_B, F_L_B}, //FAR
            {F_L_B, N_L_B, N_L_T}, //LEFT
            {N_R_B, F_R_B, F_R_T}, //RIGHT
            {N_L_T, N_R_T, F_R_T}, //TOP
            {F_L_B, F_R_B, N_R_B}  //BOTTOM
        };

        for (unsigned int p = 0; p < MAX_PLANE_COUNT; ++p)
        {
            Math::Vec3 iVec = mFrustum.points[sNormalOrder[p][1]] - mFrustum.points[sNormalOrder[p][0]];
            Math::Vec3 jVec = mFrustum.points[sNormalOrder[p][2]] - mFrustum.points[sNormalOrder[p][0]];
            Math::Vec3 normVec = Math::Cross(iVec,jVec);
            Math::Plane& plane = mFrustum.planes[p];
            plane.SetNormal(normVec);
            plane.SetOriginDistanceWithPoint(mFrustum.points[sNormalOrder[p][0]]);
        }
        mProjDirty = false;

        return true;
    }
    return false;
}

void Camera::ForceUpdate()
{
    Update();
    WindowUpdate(mCachedWidth,mCachedHeight);
}

Camera::CollisionState Camera::Frustum::GetCollisionState(const Math::Vec3& aabbMin, const Math::Vec3& aabbMax) const
{
    static const Math::Vec3 sCubeMasks[8] = {
        Math::Vec3(0.0f,0.0f,0.0f),
        Math::Vec3(1.0f,0.0f,0.0f),
        Math::Vec3(0.0f,1.0f,0.0f),
        Math::Vec3(1.0f,1.0f,0.0f),
        Math::Vec3(0.0f,0.0f,1.0f),
        Math::Vec3(1.0f,0.0f,1.0f),
        Math::Vec3(0.0f,1.0f,1.0f),
        Math::Vec3(1.0f,1.0f,1.0f)
    };

    Math::Vec3 cubePoints[8];
    for (int i = 0; i < 8; ++i)
    {
        cubePoints[i] = (Math::Vec3(1.0f,1.0f,1.0f) - sCubeMasks[i])*aabbMin + sCubeMasks[i]*aabbMax;
    }

  
    Camera::CollisionState cs = Camera::INSIDE;
    for (unsigned int pi = 0; pi < Camera::MAX_PLANE_COUNT; ++pi)
    {
        int in =0, out =0;
        const Math::Plane& p = planes[pi];
        for (unsigned int i = 0; i < Camera::MAX_POINTS && (in == 0 || out == 0); ++i)
        {
            if (p.IsInBack(cubePoints[i]))
            {
                ++in;
            }
            else
            {
                ++out;
            }
        }

        if (in == 0)
        {
            return Camera::OUTSIDE;
        }
        else if (out > 0)
        {
            cs = Camera::INTERSECT;
        }
    }

    return cs;
}


}   // namespace Camera
}   // namespace Pegasus

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
