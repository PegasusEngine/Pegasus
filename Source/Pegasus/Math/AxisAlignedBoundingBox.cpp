/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AxisAlignedBoundingBox.cpp
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Axis-aligned bounding box type and operations

#include "Pegasus/Math/AxisAlignedBoundingBox.h"
#include "Pegasus/Math/Matrix.h"
#include "Pegasus/Core/Assertion.h"

namespace Pegasus {
namespace Math {

    
const AxisAlignedBoundingBox N3DAABB_EMPTY = AxisAlignedBoundingBox();

//----------------------------------------------------------------------------------------

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
:   mMin(0.0f, 0.0f, 0.0f),
    mMax(0.0f, 0.0f, 0.0f),
    mCenter(0.0f, 0.0f, 0.0f),
    mSize(0.0f, 0.0f, 0.0f),
    areMinMaxDefined(true),
    areCenterSizeDefined(true)
{
}

//----------------------------------------------------------------------------------------

AxisAlignedBoundingBox::AxisAlignedBoundingBox(Point3In min, Point3In max)
:   mMin(min),
    mMax(max),
    mCenter(0.0f, 0.0f, 0.0f),
    mSize(0.0f, 0.0f, 0.0f),
    areMinMaxDefined(true),
    areCenterSizeDefined(false)
{
}

//----------------------------------------------------------------------------------------

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const AxisAlignedBoundingBox & box)
:   mMin(box.mMin),
    mMax(box.mMax),
    mCenter(box.mCenter),
    mSize(box.mSize),
    areMinMaxDefined(box.areMinMaxDefined),
    areCenterSizeDefined(box.areCenterSizeDefined)
{
}

//----------------------------------------------------------------------------------------

void AxisAlignedBoundingBox::SetZero()
{
    Math::SetZero(mMin);
    Math::SetZero(mMax);
    Math::SetZero(mCenter);
    Math::SetZero(mSize);
    areMinMaxDefined = true;
    areCenterSizeDefined = true;
}

//----------------------------------------------------------------------------------------

bool AxisAlignedBoundingBox::HasZeroSize() const
{
    const Vec3 & size = GetSize();
    return (size.x == 0.0f) || (size.y == 0.0f) || (size.z == 0.0f);
}

//----------------------------------------------------------------------------------------

void AxisAlignedBoundingBox::SetMinMax(Point3In min, Point3In max)
{
    PG_ASSERT(min.x <= max.x);
    PG_ASSERT(min.y <= max.y);
    PG_ASSERT(min.z <= max.z);

    mMin = Min(min, max);
    mMax = Max(min, max);
    areMinMaxDefined = true;
    areCenterSizeDefined = false;
}

//----------------------------------------------------------------------------------------

void AxisAlignedBoundingBox::SetCenterSize(Point3In center, Vec3In size)
{
    PG_ASSERT(size.x >= 0.0f);
    PG_ASSERT(size.y >= 0.0f);
    PG_ASSERT(size.z >= 0.0f);

    mCenter = center;
    mSize = Max(size, VEC3_ZERO);
    areMinMaxDefined = false;
    areCenterSizeDefined = true;
}

//----------------------------------------------------------------------------------------

void AxisAlignedBoundingBox::Expand(PFloat32 additionalSpace)
{
    if (areMinMaxDefined)
    {
        mMin -= Vec3(additionalSpace);
        mMax += Vec3(additionalSpace);
    }
    else
    {
        mSize += Vec3(additionalSpace * 2.0f);
    }
}

//----------------------------------------------------------------------------------------

const Point3 & AxisAlignedBoundingBox::GetMin() const
{
    if (!areMinMaxDefined)
    {
        PG_ASSERT(areCenterSizeDefined);
        mMin = mCenter - mSize * 0.5f;
        mMax = mMin + mSize;
        areMinMaxDefined = true;
    }

    return mMin;
}

//----------------------------------------------------------------------------------------

const Point3 & AxisAlignedBoundingBox::GetMax() const
{
    if (!areMinMaxDefined)
    {
        PG_ASSERT(areCenterSizeDefined);
        mMin = mCenter - mSize * 0.5f;
        mMax = mMin + mSize;
        areMinMaxDefined = true;
    }

    return mMax;
}

//----------------------------------------------------------------------------------------

const Point3 & AxisAlignedBoundingBox::GetCenter() const
{
    if (!areCenterSizeDefined)
    {
        PG_ASSERT(areMinMaxDefined);
        mCenter = (mMin + mMax) * 0.5f;
        mSize = mMax - mMin;
        areCenterSizeDefined = true;
    }

    return mCenter;
}

//----------------------------------------------------------------------------------------

const Vec3 & AxisAlignedBoundingBox::GetSize() const
{
    if (!areCenterSizeDefined)
    {
        PG_ASSERT(areMinMaxDefined);
        mCenter = (mMin + mMax) * 0.5f;
        mSize = mMax - mMin;
        areCenterSizeDefined = true;
    }

    return mSize;
}

//----------------------------------------------------------------------------------------

void AxisAlignedBoundingBox::ComputeCornerCoordinates(Point3 corners[8]) const
{
    const Point3 & min = GetMin();
    const Point3 & max = GetMax();
    corners[0].x = min.x;   corners[0].y = min.y;   corners[0].z = min.z;
    corners[1].x = max.x;   corners[1].y = min.y;   corners[1].z = min.z;
    corners[2].x = min.x;   corners[2].y = max.y;   corners[2].z = min.z;
    corners[3].x = max.x;   corners[3].y = max.y;   corners[3].z = min.z;
    corners[4].x = min.x;   corners[4].y = min.y;   corners[4].z = max.z;
    corners[5].x = max.x;   corners[5].y = min.y;   corners[5].z = max.z;
    corners[6].x = min.x;   corners[6].y = max.y;   corners[6].z = max.z;
    corners[7].x = max.x;   corners[7].y = max.y;   corners[7].z = max.z;
}

//----------------------------------------------------------------------------------------

void AxisAlignedBoundingBox::Union(const AxisAlignedBoundingBox & box)
{
    // If the other bounding box has a zero size, keep the bounding box unchanged
    if (box.HasZeroSize())
    {
        return;
    }

    // If the current bounding box has a zero size, copy the other bounding box
    if (HasZeroSize())
    {
        *this = box;
        return;
    }

    // Union between the two boxes
    const Point3 & min = GetMin();
    const Point3 & max = GetMax();
    const Point3 & oMin = box.GetMin();
    const Point3 & oMax = box.GetMax();
    mMin = Min(min, oMin);
    mMax = Max(max, oMax);
    areMinMaxDefined = true;
    areCenterSizeDefined = false;
}

//----------------------------------------------------------------------------------------

bool AxisAlignedBoundingBox::IsInside(Point3In point) const
{
    if (!areMinMaxDefined)
    {
        PG_ASSERT(areCenterSizeDefined);
        mMin = mCenter - mSize * 0.5f;
        mMax = mMin + mSize;
        areMinMaxDefined = true;
    }

    return (   (point.x >= mMin.x) && (point.x <= mMax.x)
            && (point.y >= mMin.y) && (point.y <= mMax.y)
            && (point.z >= mMin.z) && (point.z <= mMax.z) );
}

//----------------------------------------------------------------------------------------

bool AxisAlignedBoundingBox::IsOutside(Point3In point) const
{
    if (!areMinMaxDefined)
    {
        PG_ASSERT(areCenterSizeDefined);
        mMin = mCenter - mSize * 0.5f;
        mMax = mMin + mSize;
        areMinMaxDefined = true;
    }

    return (   (point.x < mMin.x) || (point.x > mMax.x)
            || (point.y < mMin.y) || (point.y > mMax.y)
            || (point.z < mMin.z) || (point.z > mMax.z) );
}

//----------------------------------------------------------------------------------------

bool AxisAlignedBoundingBox::IsBehind(const Plane & plane) const
{
    if (!areMinMaxDefined)
    {
        PG_ASSERT(areCenterSizeDefined);
        mMin = mCenter - mSize * 0.5f;
        mMax = mMin + mSize;
        areMinMaxDefined = true;
    }

    return (   (plane.IsInBack(mMin.x, mMin.y, mMin.z))
            && (plane.IsInBack(mMax.x, mMin.y, mMin.z))
            && (plane.IsInBack(mMin.x, mMax.y, mMin.z))
            && (plane.IsInBack(mMax.x, mMax.y, mMin.z))
            && (plane.IsInBack(mMin.x, mMin.y, mMax.z))
            && (plane.IsInBack(mMax.x, mMin.y, mMax.z))
            && (plane.IsInBack(mMin.x, mMax.y, mMax.z))
            && (plane.IsInBack(mMax.x, mMax.y, mMax.z)) );
}

//----------------------------------------------------------------------------------------

//! \todo Implement camera support
//bool AxisAlignedBoundingBox::IsInsideViewRegion(const Camera & camera) const
//{
//    return !(   IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_NEAR  ))
//             || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_FAR   ))
//             || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_LEFT  ))
//             || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_RIGHT ))
//             || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_BOTTOM))
//             || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_TOP   )) );
//}

//----------------------------------------------------------------------------------------

//! \todo Implement camera support
//bool AxisAlignedBoundingBox::IsInsideViewRegionAfterTransform(
//                                const Camera & camera, Mat44In mat) const
//{
//    // Compute the position of the 8 corners defined in homogeneous coordinates
//    const Point3 & min = GetMin();
//    const Point3 & max = GetMax();
//    Point4 corners[8];
//    corners[0].x = min.x;    corners[0].y = min.y;
//    corners[0].z = min.z;    corners[0].w = 1.0f;
//    corners[1].x = max.x;    corners[1].y = min.y;
//    corners[1].z = min.z;    corners[1].w = 1.0f;
//    corners[2].x = min.x;    corners[2].y = max.y;
//    corners[2].z = min.z;    corners[2].w = 1.0f;
//    corners[3].x = max.x;    corners[3].y = max.y;
//    corners[3].z = min.z;    corners[3].w = 1.0f;
//    corners[4].x = min.x;    corners[4].y = min.y;
//    corners[4].z = max.z;    corners[4].w = 1.0f;
//    corners[5].x = max.x;    corners[5].y = min.y;
//    corners[5].z = max.z;    corners[5].w = 1.0f;
//    corners[6].x = min.x;    corners[6].y = max.y;
//    corners[6].z = max.z;    corners[6].w = 1.0f;
//    corners[7].x = max.x;    corners[7].y = max.y;
//    corners[7].z = max.z;    corners[7].w = 1.0f;
//
//    // Apply the transformation to the 8 corners
//    // (no need to divide by w since there is no perspective projection)
//    Mult44_41(corners[0], mat, corners[0]);
//    Mult44_41(corners[1], mat, corners[1]);
//    Mult44_41(corners[2], mat, corners[2]);
//    Mult44_41(corners[3], mat, corners[3]);
//    Mult44_41(corners[4], mat, corners[4]);
//    Mult44_41(corners[5], mat, corners[5]);
//    Mult44_41(corners[6], mat, corners[6]);
//    Mult44_41(corners[7], mat, corners[7]);
//
//    // For each camera plane, test if the 8 corners are behind. If it is the case,
//    // the bounding box is outside the view frustum
//    const Camera::CameraPlane cameraPlanes[6] =
//    {    
//        Camera::N3DCAMERAPLANE_NEAR,
//        Camera::N3DCAMERAPLANE_FAR,
//        Camera::N3DCAMERAPLANE_LEFT,
//        Camera::N3DCAMERAPLANE_RIGHT,
//        Camera::N3DCAMERAPLANE_BOTTOM,
//        Camera::N3DCAMERAPLANE_TOP
//    };
//
//    for (PUInt32 p = 0; p < 6; p++)
//    {
//        const Plane & plane = camera.GetPlane(cameraPlanes[p]);
//
//        if (   (plane.IsInBack(corners[0].x, corners[0].y, corners[0].z))
//            && (plane.IsInBack(corners[1].x, corners[1].y, corners[1].z))
//            && (plane.IsInBack(corners[2].x, corners[2].y, corners[2].z))
//            && (plane.IsInBack(corners[3].x, corners[3].y, corners[3].z))
//            && (plane.IsInBack(corners[4].x, corners[4].y, corners[4].z))
//            && (plane.IsInBack(corners[5].x, corners[5].y, corners[5].z))
//            && (plane.IsInBack(corners[6].x, corners[6].y, corners[6].z))
//            && (plane.IsInBack(corners[7].x, corners[7].y, corners[7].z)) )
//            return false;
//    }
//
//    return true;
//}

//----------------------------------------------------------------------------------------

//! \todo Implement conversion to string
//String AxisAlignedBoundingBox::ToString() const
//{
//    if (!areMinMaxDefined)
//    {
//        PG_ASSERT(areCenterSizeDefined);
//        mMin = mCenter - mSize * 0.5f;
//        mMax = mMin + mSize;
//        areMinMaxDefined = true;
//    }
//
//    String string("(");
//    string += String(mMin.x);
//    string += ", ";
//    string += String(mMin.y);
//    string += ", ";
//    string += String(mMin.z);
//    string += ") -> (";
//    string += String(mMax.x);
//    string += ", ";
//    string += String(mMax.y);
//    string += ", ";
//    string += String(mMax.z);
//    string += ")";
//
//    return string;
//}


}   // namespace Math
}   // namespace Pegasus
