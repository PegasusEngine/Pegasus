/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyTypes.h
//! \author Kleber Garcia	
//! \date	November 1st, 2015
//! \brief  All the property types that are not native.

#ifndef PEGASUS_PROPERTY_TYPES_H
#define PEGASUS_PROPERTY_TYPES_H

struct Vec2Property
{
    float value[2];

    Vec2Property()
        {   value[0] = value[1] = 0.0f; }

    Vec2Property(const float* vals)
        { value[0] = vals[0]; value[1] = vals[1]; }

    bool operator==(const Vec2Property & v) const
        {   return (value[0] == v.value[0]) && (value[1] == v.value[1]);    }
};

struct Vec3Property
{
    float value[3];

    Vec3Property()
        {   value[0] = value[1] = value[2] = 0.0f;  }

    Vec3Property(const float* vals)
        { value[0] = vals[0]; value[1] = vals[1]; value[2] = vals[2];}
    

    bool operator==(const Vec3Property & v) const
        {   return (value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2]);    }
};

struct Vec4Property
{
    float value[4];

    Vec4Property()
        {   value[0] = value[1] = value[2] = value[3] = 0.0f; }

    Vec4Property(const float* vals)
        { value[0] = vals[0]; value[1] = vals[1]; value[2] = vals[2]; value[3] = vals[3];}

    bool operator==(const Vec4Property & v) const
        {   return (value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2]) && (value[3] == v.value[3]);    }
};

#endif
