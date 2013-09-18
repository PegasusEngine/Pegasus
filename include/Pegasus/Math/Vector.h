/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Vector.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Vector types and operations

#ifndef PEGASUS_MATH_VECTOR_H
#define PEGASUS_MATH_VECTOR_H

#include "Pegasus/Math/Scalar.h"

namespace Pegasus {
namespace Math {


//! 2-dimensional vector (for coordinates, texture coordinates)
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Vec2
{
    PFloat32 v[2];                  //!< Array version of the vector
    PFloat32 xy[2];                 //!< Array version of the vector
    PFloat32 xz[2];                 //!< Array version of the vector
    PFloat32 st[2];                 //!< Array version of the vector
    struct { PFloat32 x, y; };      //!< Space coordinates
    struct { PFloat32 x, z; };      //!< Space coordinates
    struct { PFloat32 s, t; };      //!< Texture coordinates

    Vec2()                          //!< Default constructor
        : x(0.0f), y(0.0f) { };

    Vec2(const Vec2 & v)            //!< Copy constructor
        : x(v.x), y(v.y) { };

    Vec2(const PFloat32 v[2])       //!< Constructor with an array
        : x(v[0]), y(v[1]) { };

    Vec2(PFloat32 x, PFloat32 y)    //!< Constructor with scalars
        : x(x), y(y) { };

    Vec2(PFloat32 s)                //!< Constructor with one scalar
        : x(s), y(s) { };
};

//----------------------------------------------------------------------------------------

//! Function input type for a 2D vector
typedef const Vec2 &     Vec2In;

//! Function input/output type for a 2D vector
typedef       Vec2 &     Vec2InOut;

//! Function return type for a 2D vector
typedef       Vec2       Vec2Return;

//! 2D point type
typedef       Vec2       Point2;

//! Function input type for a 2D point
typedef       Vec2In     Point2In;

//! Function input/output type for a 2D point
typedef       Vec2InOut  Point2InOut;

//! Function return type for a 2D point
typedef       Vec2Return Point2Return;

//----------------------------------------------------------------------------------------

//! 3-dimensional vector (for coordinates, texture coordinates, or colors)
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Vec3
{
    PFloat32 v[3];                            //!< Array version of the vector
    PFloat32 xyz[3];                          //!< Array version of the vector
    PFloat32 str[3];                          //!< Array version of the vector
    struct { PFloat32 x, y, z;          };    //!< Space coordinates
    struct { PFloat32 xy[2], z;         };    //!< Space coordinates
    struct { PFloat32 x, yz[2];         };    //!< Space coordinates
    struct { PFloat32 s, t, r;          };    //!< Texture coordinates
    struct { PFloat32 st[2], r;         };    //!< Texture coordinates
    struct { PFloat32 s, tr[2];         };    //!< Texture coordinates
    struct { PFloat32 red, green, blue; };    //!< Colors
    struct { PFloat32 rg[2], blue;      };    //!< Colors
    struct { PFloat32 red, gb[2];       };    //!< Colors

    //! \todo Find out why Vec2 couldn't be used (constructor issue? double initialization?)

    //struct { PFloat32 x, y, z;                };    //!< Space coordinates
    //struct { Vec2 xy; float z;            };    //!< Space coordinates
    //struct { PFloat32 x; Vec2 yz;        };    //!< Space coordinates
    //struct { PFloat32 s, t, r;                };    //!< Texture coordinates
    //struct { Vec2 st; PFloat32 r;        };    //!< Texture coordinates
    //struct { PFloat32 s; Vec2 tr;        };    //!< Texture coordinates
    //struct { PFloat32 red, green, blue;        };    //!< Colors
    //struct { Vec2 rg; PFloat32 blue;        };    //!< Colors
    //struct { PFloat32 red; Vec2 gb;        };    //!< Colors

    Vec3()                                    //!< Default constructor
        : x(0.0f), y(0.0f), z(0.0f) { };

    Vec3(const Vec3 & v)                        //!< Copy constructor
        : x(v.x), y(v.y), z(v.z) { };

    Vec3(const PFloat32 v[3])                   //!< Constructor with an array
        : x(v[0]), y(v[1]), z(v[2]) { };

    Vec3(PFloat32 x, PFloat32 y, PFloat32 z)    //!< Constructor with scalars
        : x(x), y(y), z(z) { };

    Vec3(const Vec2 & xy, PFloat32 z)           //!< Mixed constructor
        : x(xy.x), y(xy.y), z(z) { };

    Vec3(const PFloat32 xy[2], PFloat32 z)      //!< Mixed constructor
        : x(xy[0]), y(xy[1]), z(z) { };

    Vec3(PFloat32 x, const Vec2 & yz)           //!< Mixed constructor
        : x(x), y(yz.x), z(yz.y) { };

    Vec3(PFloat32 x, const PFloat32 yz[2])      //!< Mixed constructor
        : x(x), y(yz[0]), z(yz[1]) { };

    Vec3(PFloat32 s)                            //!< Constructor with one scalar
        : x(s), y(s), z(s) { };
};

//----------------------------------------------------------------------------------------

//! Function input type for a 3D vector
typedef const Vec3 &     Vec3In;

//! Function input/output type for a 3D vector
typedef       Vec3 &     Vec3InOut;

//! Function return type for a 3D vector
typedef       Vec3       Vec3Return;

//! 3D point type
typedef       Vec3       Point3;

//! Function input type for a 3D point
typedef       Vec3In     Point3In;

//! Function input/output type for a 3D point
typedef       Vec3InOut  Point3InOut;

//! Function return type for a 3D point
typedef       Vec3Return Point3Return;

//----------------------------------------------------------------------------------------

//! 4-dimensional vector (for coordinates, texture coordinates, or colors)
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Vec4
{
    PFloat32 v[4];                                   //!< Array version of the vector
    PFloat32 xyzw[4];                                //!< Array version of the vector
    PFloat32 strq[4];                                //!< Array version of the vector
    struct { PFloat32 x, y, z, w;              };    //!< Homogeneous space coordinates
    struct { PFloat32 xy[2], zw[2];            };    //!< Homogeneous space coordinates
    struct { PFloat32 x, yz[2], w;             };    //!< Homogeneous space coordinates
    struct { PFloat32 xyz[3], w;               };    //!< Homogeneous space coordinates
    struct { PFloat32 x, yzw[3];               };    //!< Homogeneous space coordinates
    struct { PFloat32 s, t, r, q;              };    //!< Texture coordinates
    struct { PFloat32 st[2], rq[2];            };    //!< Texture coordinates
    struct { PFloat32 s, tr[2], q;             };    //!< Texture coordinates
    struct { PFloat32 str[3], q;               };    //!< Texture coordinates
    struct { PFloat32 s, trq[3];               };    //!< Texture coordinates
    struct { PFloat32 red, green, blue, alpha; };    //!< Colors
    struct { PFloat32 rg[2], ba[2];            };    //!< Colors
    struct { PFloat32 red, gb[2], alpha;       };    //!< Colors
    struct { PFloat32 rgb[3], alpha;           };    //!< Colors
    struct { PFloat32 red, gba[3];             };    //!< Colors

    //! \todo Find out why Vec2 couldn't be used (constructor issue? double initialization?)

    //struct { PFloat32 x, y, z, w;            };    //!< Homogeneous space coordinates
    //struct { Vec2 xy, zw;            };    //!< Homogeneous space coordinates
    //struct { PFloat32 x; Vec2 yz; PFloat32 w;};    //!< Homogeneous space coordinates
    //struct { Vec3 xyz; PFloat32 w;    };    //!< Homogeneous space coordinates
    //struct { PFloat32 x; Vec3 yzw;    };    //!< Homogeneous space coordinates
    //struct { PFloat32 s, t, r, q;            };    //!< Texture coordinates
    //struct { Vec2 st, rq;            };    //!< Texture coordinates
    //struct { PFloat32 s; Vec2 tr; PFloat32 q;};    //!< Texture coordinates
    //struct { Vec3 str; PFloat32 q;    };    //!< Texture coordinates
    //struct { PFloat32 s; Vec3 trq;    };    //!< Texture coordinates
    //struct { PFloat32 red, green, blue, alpha;    };    //!< Colors
    //struct { Vec2 rg, ba;            };    //!< Colors
    //struct { PFloat32 red; Vec2 gb; PFloat32 alpha;    };    //!< Colors
    //struct { Vec3 rgb; PFloat32 alpha;    };    //!< Colors
    //struct { PFloat32 red; Vec3 gba;    };    //!< Colors

    Vec4()                                                  //!< Default constructor
        : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { };

    Vec4(const Vec4 & v)                                    //!< Copy constructor
        : x(v.x), y(v.y), z(v.z), w(v.w) { };

    Vec4(const PFloat32 v[4])                               //!< Constructor with an array
        : x(v[0]), y(v[1]), z(v[2]), w(v[3]) { };

    Vec4(PFloat32 x, PFloat32 y, PFloat32 z, PFloat32 w)    //!< Constructor with scalars
        : x(x), y(y), z(z), w(w) { };

    Vec4(const Vec2 & xy, const Vec2 & zw)                  //!< Mixed constructor
        : x(xy.x), y(xy.y), z(zw.x), w(zw.y) { };

    Vec4(const PFloat32 xy[2], const PFloat32 zw[2])        //!< Mixed constructor
        : x(xy[0]), y(xy[1]), z(zw[0]), w(zw[1]) { };

    Vec4(PFloat32 x, const Vec2 & yz, PFloat32 w)           //!< Mixed constructor
        : x(x), y(yz.x), z(yz.y), w(w) { };

    Vec4(PFloat32 x, const PFloat32 yz[2], PFloat32 w)      //!< Mixed constructor
        : x(x), y(yz[0]), z(yz[1]), w(w) { };

    Vec4(const Vec3 & xyz, PFloat32 w)                      //!< Mixed constructor
        : x(xyz.x), y(xyz.y), z(xyz.z), w(w) { };

    Vec4(const PFloat32 xyz[3], PFloat32 w)                 //!< Mixed constructor
        : x(xyz[0]), y(xyz[1]), z(xyz[2]), w(w) { };

    Vec4(PFloat32 x, const Vec3 & yzw)                      //!< Mixed constructor
        : x(x), y(yzw.x), z(yzw.y), w(yzw.z) { };

    Vec4(PFloat32 x, const PFloat32 yzw[3])                 //!< Mixed constructor
        : x(x), y(yzw[0]), z(yzw[1]), w(yzw[2]) { };

    Vec4(PFloat32 s)                                        //!< Constructor with one scalar
        : x(s), y(s), z(s), w(s) { };
};

//----------------------------------------------------------------------------------------

//! Function input type for a 4D vector
typedef const Vec4 &     Vec4In;

//! Function input/output type for a 4D vector
typedef       Vec4 &     Vec4InOut;

//! Function return type for a 4D vector
typedef       Vec4       Vec4Return;

//! 4D point type
typedef       Vec4       Point4;

//! Function input type for a 4D point
typedef       Vec4In     Point4In;

//! Function input/output type for a 4D point
typedef       Vec4InOut  Point4InOut;

//! Function return type for a 4D point
typedef       Vec4Return Point4Return;

//----------------------------------------------------------------------------------------

// Vector constants

//! \todo Write them as static, otherwise they are instanced in each .cpp file

//@{
//! Zero vector
const Vec2 VEC2_ZERO(0.0f, 0.0f);
const Vec3 VEC3_ZERO(0.0f, 0.0f, 0.0f);
const Vec4 VEC4_ZERO(0.0f, 0.0f, 0.0f, 0.0f);
//@}

//@{
//! X axis
const Vec2 VEC2_X(1.0f, 0.0f);
const Vec3 VEC3_X(1.0f, 0.0f, 0.0f);
const Vec4 VEC4_X(1.0f, 0.0f, 0.0f, 0.0f);
//@}

//@{
//! Y axis
const Vec2 VEC2_Y(0.0f, 1.0f);
const Vec3 VEC3_Y(0.0f, 1.0f, 0.0f);
const Vec4 VEC4_Y(0.0f, 1.0f, 0.0f, 0.0f);
//@}

//@{
//! Z axis
const Vec3 VEC3_Z(0.0f, 0.0f, 1.0f);
const Vec4 VEC4_Z(0.0f, 0.0f, 1.0f, 0.0f);
//@}

//! W axis
const Vec4 VEC4_W(0.0f, 0.0f, 0.0f, 1.0f);

//@{
//! X axis (synonyms of VECx_X)
const Vec2 VEC2_POSX(1.0f, 0.0f);
const Vec3 VEC3_POSX(1.0f, 0.0f, 0.0f);
const Vec4 VEC4_POSX(1.0f, 0.0f, 0.0f, 0.0f);
//@}

//@{
//! Y axis (synonyms of VECx_Y)
const Vec2 VEC2_POSY(0.0f, 1.0f);
const Vec3 VEC3_POSY(0.0f, 1.0f, 0.0f);
const Vec4 VEC4_POSY(0.0f, 1.0f, 0.0f, 0.0f);
//@}

//@{
//! Z axis (synonyms of VECx_Z)
const Vec3 VEC3_POSZ(0.0f, 0.0f, 1.0f);
const Vec4 VEC4_POSZ(0.0f, 0.0f, 1.0f, 0.0f);
//@}

//! W axis (synonym of VEC4_W)
const Vec4 VEC4_POSW(0.0f, 0.0f, 0.0f, 1.0f);

//@{
//! -X axis
const Vec2 VEC2_NEGX(-1.0f, 0.0f);
const Vec3 VEC3_NEGX(-1.0f, 0.0f, 0.0f);
const Vec4 VEC4_NEGX(-1.0f, 0.0f, 0.0f, 0.0f);
//@}

//@{
//! -Y axis
const Vec2 VEC2_NEGY(0.0f, -1.0f);
const Vec3 VEC3_NEGY(0.0f, -1.0f, 0.0f);
const Vec4 VEC4_NEGY(0.0f, -1.0f, 0.0f, 0.0f);
//@}

//@{
//! -Z axis
const Vec3 VEC3_NEGZ(0.0f, 0.0f, -1.0f);
const Vec4 VEC4_NEGZ(0.0f, 0.0f, -1.0f, 0.0f);
//@}

//@{
//! -W axis
const Vec4 VEC4_NEGW(0.0f, 0.0f, 0.0f, -1.0f);
//@}

//----------------------------------------------------------------------------------------

//@{
//! Origin point
const Point2 POINT2_ORIGIN(0.0f, 0.0f);
const Point3 POINT3_ORIGIN(0.0f, 0.0f, 0.0f);
const Point4 POINT4_ORIGIN(0.0f, 0.0f, 0.0f, 1.0f);
//@}

//----------------------------------------------------------------------------------------

//@{
//! Set the components of a vector to zero
//! \param vec Vector to initialize
inline void SetZero(Vec2InOut vec) { vec.x = 0.0f; vec.y = 0.0f;                             }
inline void SetZero(Vec3InOut vec) { vec.x = 0.0f; vec.y = 0.0f; vec.z = 0.0f;               }
inline void SetZero(Vec4InOut vec) { vec.x = 0.0f; vec.y = 0.0f; vec.z = 0.0f; vec.w = 0.0f; }
//@}

//@{
//! Test if a vector is exactly zero
//! \param vec Vector to test
//! \return true if all components of the input vector are equal to zero
inline bool IsZero(Vec2In vec) { return (vec.x == 0.0f) && (vec.y == 0.0f);                                       }
inline bool IsZero(Vec3In vec) { return (vec.x == 0.0f) && (vec.y == 0.0f) && (vec.z == 0.0f);                    }
inline bool IsZero(Vec4In vec) { return (vec.x == 0.0f) && (vec.y == 0.0f) && (vec.z == 0.0f) && (vec.w == 0.0f); }
//@}

//@{
//! Set the components of a vector to one
//! \param vec Vector to initialize
inline void SetOne(Vec2InOut vec) { vec.x = 1.0f; vec.y = 1.0f;                             }
inline void SetOne(Vec3InOut vec) { vec.x = 1.0f; vec.y = 1.0f; vec.z = 1.0f;               }
inline void SetOne(Vec4InOut vec) { vec.x = 1.0f; vec.y = 1.0f; vec.z = 1.0f; vec.w = 1.0f; }
//@}

//@{
//! Test if a vector is exactly one
//! \param vec Vector to test
//! \return true if all components of the input vector are equal to one
inline bool IsOne(Vec2In vec) { return (vec.x == 1.0f) && (vec.y == 1.0f);                                       }
inline bool IsOne(Vec3In vec) { return (vec.x == 1.0f) && (vec.y == 1.0f) && (vec.z == 1.0f);                    }
inline bool IsOne(Vec4In vec) { return (vec.x == 1.0f) && (vec.y == 1.0f) && (vec.z == 1.0f) && (vec.w == 1.0f); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Complementary of a vector (unary operator)
//! \param vec Vector to complement
//! \return Complemented vector
inline Vec2Return operator - (Vec2In vec) { return Vec2(-vec.x, -vec.y);                 }
inline Vec3Return operator - (Vec3In vec) { return Vec3(-vec.x, -vec.y, -vec.z);         }
inline Vec4Return operator - (Vec4In vec) { return Vec4(-vec.x, -vec.y, -vec.z, -vec.w); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Add a vector to another one
//! \param vect1 First vector (which contains the result)
//! \param vect2 Second vector
inline void operator += (Vec2InOut vec1, Vec2In vec2) { vec1.x += vec2.x; vec1.y += vec2.y;                                     }
inline void operator += (Vec3InOut vec1, Vec3In vec2) { vec1.x += vec2.x; vec1.y += vec2.y; vec1.z += vec2.z;                   }
inline void operator += (Vec4InOut vec1, Vec4In vec2) { vec1.x += vec2.x; vec1.y += vec2.y; vec1.z += vec2.z; vec1.w += vec2.w; }
//@}

//@{
//! Subtract a vector from another one
//! \param vect1 First vector (which contains the result)
//! \param vect2 Second vector
inline void operator -= (Vec2InOut vec1, Vec2In vec2) { vec1.x -= vec2.x; vec1.y -= vec2.y;                                     }
inline void operator -= (Vec3InOut vec1, Vec3In vec2) { vec1.x -= vec2.x; vec1.y -= vec2.y; vec1.z -= vec2.z;                   }
inline void operator -= (Vec4InOut vec1, Vec4In vec2) { vec1.x -= vec2.x; vec1.y -= vec2.y; vec1.z -= vec2.z; vec1.w -= vec2.w; }
//@}

//@{
//! Multiply a vector by a constant
//! \param vec The vector to multiply (which contains the result)
//! \param cst The constant
inline void operator *= (Vec2InOut vec, PFloat32 cst) { vec.x *= cst; vec.y *= cst;                             }
inline void operator *= (Vec3InOut vec, PFloat32 cst) { vec.x *= cst; vec.y *= cst; vec.z *= cst;               }
inline void operator *= (Vec4InOut vec, PFloat32 cst) { vec.x *= cst; vec.y *= cst; vec.z *= cst; vec.w *= cst; }
//@}

//@{
//! Multiply a vector by another one, component-wise
//! \param vect1 The vector to multiply (which contains the result)
//! \param vect2 Second vector
inline void operator *= (Vec2InOut vec1, Vec2In vec2) { vec1.x *= vec2.x; vec1.y *= vec2.y;                                     }
inline void operator *= (Vec3InOut vec1, Vec3In vec2) { vec1.x *= vec2.x; vec1.y *= vec2.y; vec1.z *= vec2.z;                   }
inline void operator *= (Vec4InOut vec1, Vec4In vec2) { vec1.x *= vec2.x; vec1.y *= vec2.y; vec1.z *= vec2.z; vec1.w *= vec2.w; }
//@}

//@{
//! Divide a vector by a constant
//! \warning It is faster to use *= with the reciprocal constant if available
//! \param vec The vector to divide (which contains the result)
//! \param cst The constant
inline void operator /= (Vec2InOut vec, PFloat32 cst) { vec.x /= cst; vec.y /= cst;                             }
inline void operator /= (Vec3InOut vec, PFloat32 cst) { vec.x /= cst; vec.y /= cst; vec.z /= cst;               }
inline void operator /= (Vec4InOut vec, PFloat32 cst) { vec.x /= cst; vec.y /= cst; vec.z /= cst; vec.w /= cst; }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Add a vector to another one
//! \warning This operator implies a vector copy, so it is better to use
//!          the preceding unary operators
//! \param vect1 First vector (which contains the result)
//! \param vect2 Second vector
//! \return The result vector
inline Vec2Return operator + (Vec2In vec1, Vec2In vec2) { return Vec2(vec1.x + vec2.x, vec1.y + vec2.y);                                   }
inline Vec3Return operator + (Vec3In vec1, Vec3In vec2) { return Vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);                  }
inline Vec4Return operator + (Vec4In vec1, Vec4In vec2) { return Vec4(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w); }
//@}

//@{
//! Subtract a vector from another one
//! \warning This operator implies a vector copy, so it is better to use
//!          the preceding unary operators
//! \param vect1 First vector (which contains the result)
//! \param vect2 Second vector
//! \return The result vector
inline Vec2Return operator - (Vec2In vec1, Vec2In vec2) { return Vec2(vec1.x - vec2.x, vec1.y - vec2.y);                                   }
inline Vec3Return operator - (Vec3In vec1, Vec3In vec2) { return Vec3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);                  }
inline Vec4Return operator - (Vec4In vec1, Vec4In vec2) { return Vec4(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w); }
//@}

//@{
//! Multiply a vector by a constant
//! \warning This operator implies a vector copy, so it is better to use
//!          the preceding unary operators
//! \param vec The vector to multiply (which contains the result)
//! \param cst The constant
//! \return The result vector
inline Vec2Return operator * (Vec2In vec, PFloat32 cst) { return Vec2(vec.x * cst, vec.y * cst);                           }
inline Vec3Return operator * (Vec3In vec, PFloat32 cst) { return Vec3(vec.x * cst, vec.y * cst, vec.z * cst);              }
inline Vec4Return operator * (Vec4In vec, PFloat32 cst) { return Vec4(vec.x * cst, vec.y * cst, vec.z * cst, vec.w * cst); }
inline Vec2Return operator * (PFloat32 cst, Vec2In vec) { return Vec2(vec.x * cst, vec.y * cst);                           }
inline Vec3Return operator * (PFloat32 cst, Vec3In vec) { return Vec3(vec.x * cst, vec.y * cst, vec.z * cst);              }
inline Vec4Return operator * (PFloat32 cst, Vec4In vec) { return Vec4(vec.x * cst, vec.y * cst, vec.z * cst, vec.w * cst); }
//@}

//@{
//! Multiply a vector by another one component-wise
//! \warning This operator implies a vector copy, so it is better to use
//!          the preceding unary operators
//! \param vec1 The vector to multiply (which contains the result)
//! \param vec2 Second vector
//! \return The result vector
inline Vec2Return operator * (Vec2In vec1, Vec2In vec2) { return Vec2(vec1.x * vec2.x, vec1.y * vec2.y);                                   }
inline Vec3Return operator * (Vec3In vec1, Vec3In vec2) { return Vec3(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);                  }
inline Vec4Return operator * (Vec4In vec1, Vec4In vec2) { return Vec4(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z, vec1.w * vec2.w); }
//@}

//@{
//! Divide a vector by a constant
//! \warning This operator implies a vector copy, so it is better to use
//!          the preceding unary operators
//! \param vec The vector to divide (which contains the result)
//! \param cst The constant
//! \return The result vector
inline Vec2Return operator / (Vec2In vec, PFloat32 cst)
    {
        const PFloat32 recCst = 1.0f / cst;
        return Vec2(vec.x * recCst, vec.y * recCst);
    }

inline Vec3Return operator / (Vec3In vec, PFloat32 cst)
    {
        const PFloat32 recCst = 1.0f / cst;
        return Vec3(vec.x * recCst, vec.y * recCst, vec.z * recCst);
    }

inline Vec4Return operator / (Vec4In vec, PFloat32 cst)
    {
        const PFloat32 recCst = 1.0f / cst;
        return Vec4(vec.x * recCst, vec.y * recCst, vec.z * recCst, vec.w * recCst);
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Test the equality of two vectors
//! \param vec1 First vector to compare
//! \param vec2 Second vector to compare
//! \return true if the vectors are equal component-wise
inline bool operator == (Vec2In vec1, Vec2In vec2) { return (vec1.x == vec2.x) && (vec1.y == vec2.y);                                             }
inline bool operator == (Vec3In vec1, Vec3In vec2) { return (vec1.x == vec2.x) && (vec1.y == vec2.y) && (vec1.z == vec2.z);                       }
inline bool operator == (Vec4In vec1, Vec4In vec2) { return (vec1.x == vec2.x) && (vec1.y == vec2.y) && (vec1.z == vec2.z) && (vec1.w == vec2.w); }
//@}

//@{
//! Test the inequality of two vectors
//! \param vec1 First vector to compare
//! \param vec2 Second vector to compare
//! \return true if the vectors are not equal component-wise
inline bool operator != (Vec2In vec1, Vec2In vec2) { return (vec1.x != vec2.x) && (vec1.y != vec2.y);                                             }
inline bool operator != (Vec3In vec1, Vec3In vec2) { return (vec1.x != vec2.x) && (vec1.y != vec2.y) && (vec1.z != vec2.z);                       }
inline bool operator != (Vec4In vec1, Vec4In vec2) { return (vec1.x != vec2.x) && (vec1.y != vec2.y) && (vec1.z != vec2.z) && (vec1.w != vec2.w); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Length of a vector
//! \param vec Argument vector
//! \return Length of the vector
inline PFloat32 Length(Vec2In vec) { return Sqrt(vec.x * vec.x + vec.y * vec.y);                                 }
inline PFloat32 Length(Vec3In vec) { return Sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);                 }
inline PFloat32 Length(Vec4In vec) { return Sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w); }
//@}

//@{
//! Squared length of a vector (faster to compute than standard length)
//! \param vec Argument vector
//! \return Squared length of the vector
inline PFloat32 SqLength(Vec2In vec) { return vec.x * vec.x + vec.y * vec.y;                                 }
inline PFloat32 SqLength(Vec3In vec) { return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;                 }
inline PFloat32 SqLength(Vec4In vec) { return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w; }
//@}

//@{
//! Reciprocal of the length of a vector
//! \param vec Argument vector
//! \return 1.0 / length of the vector
inline PFloat32 RcpLength(Vec2In vec) { return RcpSqrt(vec.x * vec.x + vec.y * vec.y);                                 }
inline PFloat32 RcpLength(Vec3In vec) { return RcpSqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);                 }
inline PFloat32 RcpLength(Vec4In vec) { return RcpSqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w); }
//@}

//@{
//! Normalize a vector
//! \param vec The vector to normalize (also the result of the calculation)
inline void Normalize(Vec2InOut vec) { vec *= RcpLength(vec); }
inline void Normalize(Vec3InOut vec) { vec *= RcpLength(vec); }
inline void Normalize(Vec4InOut vec) { vec *= RcpLength(vec); }
//@}

//@{
//! Normalize a vector
//! \param dst The resulting normalized vector
//! \param vec The vector to normalize (unmodified)
inline void Normalize(Vec2InOut dst, Vec2In vec) { dst = vec * RcpLength(vec); }
inline void Normalize(Vec3InOut dst, Vec3In vec) { dst = vec * RcpLength(vec); }
inline void Normalize(Vec4InOut dst, Vec4In vec) { dst = vec * RcpLength(vec); }
//@}

//@{
//! Normalize a vector
//! \param vec The vector to normalize
//! \return Normalized vector
inline Vec2Return Normalize(Vec2In vec) { return vec * RcpLength(vec); }
inline Vec3Return Normalize(Vec3In vec) { return vec * RcpLength(vec); }
inline Vec4Return Normalize(Vec4In vec) { return vec * RcpLength(vec); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Dot product of 2 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \return The scalar product of vec1 and vec2
inline PFloat32 Dot(Vec2In vec1, Vec2In vec2) { return vec1.x * vec2.x + vec1.y * vec2.y;                                     }
inline PFloat32 Dot(Vec3In vec1, Vec3In vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;                   }
inline PFloat32 Dot(Vec4In vec1, Vec4In vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w; }
//@}

//! Cross product of 2 vectors
//! \param dst The result of the cross product
//! \param vec1 The first vector
//! \param vec2 The second vector
inline void Cross(Vec3InOut dst, Vec3In vec1, Vec3In vec2)
    {
        const PFloat32 t0 = vec1.y * vec2.z - vec1.z * vec2.y;
        const PFloat32 t1 = vec1.z * vec2.x - vec1.x * vec2.z;
        dst.v[2] = vec1.x * vec2.y - vec1.y * vec2.x;
        dst.v[0] = t0;
        dst.v[1] = t1;
    }

//! Cross product of 2 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \return The result of the cross product
inline Vec3Return Cross(Vec3In vec1, Vec3In vec2)
    {
        return Vec3(vec1.y * vec2.z - vec1.z * vec2.y,
                    vec1.z * vec2.x - vec1.x * vec2.z,
                    vec1.x * vec2.y - vec1.y * vec2.x);
    }

//! Projection of a 2D vector into another coordinate frame
//! \param dst The projected vector, expressed in the new coordinate frame
//! \param vec The vector to project
//! \param v1 The first vector of the new coordinate frame
//! \param v2 The second vector of the new coordinate frame
inline void Project(Vec2InOut dst, Vec2In vec, Vec2In v1, Vec2In v2)
    {
        dst.x = Dot(vec, v1);
        dst.y = Dot(vec, v2);
    }

//! Projection of a 2D vector into another coordinate frame
//! \param vec The vector to project
//! \param v1 The first vector of the new coordinate frame
//! \param v2 The second vector of the new coordinate frame
//! \return The projected vector, expressed in the new coordinate frame
inline Vec2Return Project(Vec2In vec, Vec2In v1, Vec2In v2)
    {
        return Vec2(Dot(vec, v1), Dot(vec, v2));
    }

//! Projection of a 3D vector into another coordinate frame
//! \param dst The projected vector, expressed in the new coordinate frame
//! \param vec The vector to project
//! \param v1 The first vector of the new coordinate frame
//! \param v2 The second vector of the new coordinate frame
//! \param v3 The third vector of the new coordinate frame
inline void Project(Vec3InOut dst, Vec3In vec, Vec3In v1, Vec3In v2, Vec3In v3)
    {
        dst.x = Dot(vec, v1);
        dst.y = Dot(vec, v2);
        dst.z = Dot(vec, v3);
    }

//! Projection of a 3D vector into another coordinate frame
//! \param vec The vector to project
//! \param v1 The first vector of the new coordinate frame
//! \param v2 The second vector of the new coordinate frame
//! \param v3 The third vector of the new coordinate frame
//! \return The projected vector, expressed in the new coordinate frame
inline Vec3Return Project(Vec3In vec, Vec3In v1, Vec3In v2, Vec3In v3)
    {
        return Vec3(Dot(vec, v1), Dot(vec, v2), Dot(vec, v3));
    }

//! Projection of a 4D vector into another coordinate frame
//! \param dst The projected vector, expressed in the new coordinate frame
//! \param vec The vector to project
//! \param v1 The first vector of the new coordinate frame
//! \param v2 The second vector of the new coordinate frame
//! \param v3 The third vector of the new coordinate frame
//! \param v4 The fourth vector of the new coordinate frame
inline void Project(Vec4InOut dst, Vec4In vec, Vec4In v1, Vec4In v2, Vec4In v3, Vec4In v4)
    {
        dst.x = Dot(vec, v1);
        dst.y = Dot(vec, v2);
        dst.z = Dot(vec, v3);
        dst.w = Dot(vec, v4);
    }

//! Projection of a 4D vector into another coordinate frame
//! \param vec The vector to project
//! \param v1 The first vector of the new coordinate frame
//! \param v2 The second vector of the new coordinate frame
//! \param v3 The third vector of the new coordinate frame
//! \param v4 The fourth vector of the new coordinate frame
//! \return The projected vector, expressed in the new coordinate frame
inline Vec4Return Project(Vec4In vec, Vec4In v1, Vec4In v2, Vec4In v3, Vec4In v4)
    {
        return Vec4(Dot(vec, v1), Dot(vec, v2), Dot(vec, v3), Dot(vec, v4));
    }

//----------------------------------------------------------------------------------------

//@{
//! Distance between two points
//! \param point1 The first point
//! \param point2 The second point
//! \return Distance between the two points
inline PFloat32 Distance(Point2In point1, Point2In point2) { return Length(point2 - point1); }
inline PFloat32 Distance(Point3In point1, Point3In point2) { return Length(point2 - point1); }
inline PFloat32 Distance(Point4In point1, Point4In point2) { return Length(point2 - point1); }
//@}

//@{
//! Squared distance between two points
//! \param point1 The first point
//! \param point2 The second point
//! \return Squared distance between the two points
inline PFloat32 SqDistance(Point2In point1, Point2In point2) { return SqLength(point2 - point1); }
inline PFloat32 SqDistance(Point3In point1, Point3In point2) { return SqLength(point2 - point1); }
inline PFloat32 SqDistance(Point4In point1, Point4In point2) { return SqLength(point2 - point1); }
//@}

//@{
//! Reciprocal of the distance between two points
//! \param point1 The first point
//! \param point2 The second point
//! \return Reciprocal of the distance between the two points
inline PFloat32 RcpDistance(Point2In point1, Point2In point2) { return RcpLength(point2 - point1); }
inline PFloat32 RcpDistance(Point3In point1, Point3In point2) { return RcpLength(point2 - point1); }
inline PFloat32 RcpDistance(Point4In point1, Point4In point2) { return RcpLength(point2 - point1); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Clamp a vector
//! \param vec The vector to clamp (also the result of the clamping)
//! \param min Minimum value of the vector fields
//! \param max Maximum value of the vector fields
inline void Clamp(Vec2InOut vec, PFloat32 min, PFloat32 max) { vec.x = Clamp(vec.x, min, max); vec.y = Clamp(vec.y, min, max);                                                                 }
inline void Clamp(Vec3InOut vec, PFloat32 min, PFloat32 max) { vec.x = Clamp(vec.x, min, max); vec.y = Clamp(vec.y, min, max); vec.z = Clamp(vec.z, min, max);                                 }
inline void Clamp(Vec4InOut vec, PFloat32 min, PFloat32 max) { vec.x = Clamp(vec.x, min, max); vec.y = Clamp(vec.y, min, max); vec.z = Clamp(vec.z, min, max); vec.w = Clamp(vec.w, min, max); }
//@}

//@{
//! Clamp a vector
//! \param vec The vector to clamp
//! \param min Minimum value of the vector fields
//! \param max Maximum value of the vector fields
//! \return Resulting clamped vector
inline Vec2Return Clamp(Vec2In vec, PFloat32 min, PFloat32 max) { return Vec2(Clamp(vec.x, min, max), Clamp(vec.y, min, max));                                                 }
inline Vec3Return Clamp(Vec3In vec, PFloat32 min, PFloat32 max) { return Vec3(Clamp(vec.x, min, max), Clamp(vec.y, min, max), Clamp(vec.z, min, max));                         }
inline Vec4Return Clamp(Vec4In vec, PFloat32 min, PFloat32 max) { return Vec4(Clamp(vec.x, min, max), Clamp(vec.y, min, max), Clamp(vec.z, min, max), Clamp(vec.w, min, max)); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Component-wise minimum of 2 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \return Vector containing the minimum components of vec1 and vec2
inline Vec2Return Min(Vec2In vec1, Vec2In vec2) { return Vec2(Min(vec1.x, vec2.x), Min(vec1.y, vec2.y));                                           }
inline Vec3Return Min(Vec3In vec1, Vec3In vec2) { return Vec3(Min(vec1.x, vec2.x), Min(vec1.y, vec2.y), Min(vec1.z, vec2.z));                      }
inline Vec4Return Min(Vec4In vec1, Vec4In vec2) { return Vec4(Min(vec1.x, vec2.x), Min(vec1.y, vec2.y), Min(vec1.z, vec2.z), Min(vec1.w, vec2.w)); }
//@}

//@{
//! Component-wise maximum of 2 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \return Vector containing the maximum components of vec1 and vec2
inline Vec2Return Max(Vec2In vec1, Vec2In vec2) { return Vec2(Max(vec1.x, vec2.x), Max(vec1.y, vec2.y));                                           }
inline Vec3Return Max(Vec3In vec1, Vec3In vec2) { return Vec3(Max(vec1.x, vec2.x), Max(vec1.y, vec2.y), Max(vec1.z, vec2.z));                      }
inline Vec4Return Max(Vec4In vec1, Vec4In vec2) { return Vec4(Max(vec1.x, vec2.x), Max(vec1.y, vec2.y), Max(vec1.z, vec2.z), Max(vec1.w, vec2.w)); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Component-wise minimum of 3 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \param vec3 The third vector
//! \return Vector containing the minimum components of vec1, vec2 and vec3
inline Vec2Return Min(Vec2In vec1, Vec2In vec2, Vec2In vec3)
    {
        return Vec2(Min(vec1.x, vec2.x, vec3.x),
                    Min(vec1.y, vec2.y, vec3.y));
    }

inline Vec3Return Min(Vec3In vec1, Vec3In vec2, Vec3In vec3)
    {
        return Vec3(Min(vec1.x, vec2.x, vec3.x),
                    Min(vec1.y, vec2.y, vec3.y),
                    Min(vec1.z, vec2.z, vec3.z));
    }

inline Vec4Return Min(Vec4In vec1, Vec4In vec2, Vec4In vec3)
    {
        return Vec4(Min(vec1.x, vec2.x, vec3.x),
                    Min(vec1.y, vec2.y, vec3.y),
                    Min(vec1.z, vec2.z, vec3.z),
                    Min(vec1.w, vec2.w, vec3.w));
    }
//@}

//@{
//! Component-wise maximum of 3 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \param vec3 The third vector
//! \return Vector containing the maximum components of vec1, vec2 and vec3
inline Vec2Return Max(Vec2In vec1, Vec2In vec2, Vec2In vec3)
    {
        return Vec2(Max(vec1.x, vec2.x, vec3.x),
                    Max(vec1.y, vec2.y, vec3.y));
    }

inline Vec3Return Max(Vec3In vec1, Vec3In vec2, Vec3In vec3)
    {
        return Vec3(Max(vec1.x, vec2.x, vec3.x),
                    Max(vec1.y, vec2.y, vec3.y),
                    Max(vec1.z, vec2.z, vec3.z));
    }

inline Vec4Return Max(Vec4In vec1, Vec4In vec2, Vec4In vec3)
    {
        return Vec4(Max(vec1.x, vec2.x, vec3.x),
                    Max(vec1.y, vec2.y, vec3.y),
                    Max(vec1.z, vec2.z, vec3.z),
                    Max(vec1.w, vec2.w, vec3.w));
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Component-wise minimum of 4 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \param vec3 The third vector
//! \param vec4 The fourth vector
//! \return Vector containing the minimum components of vec1, vec2, vec3 and vec4
inline Vec2Return Min(Vec2In vec1, Vec2In vec2, Vec2In vec3, Vec2In vec4)
    {
        return Vec2(Min(vec1.x, vec2.x, vec3.x, vec4.x),
                    Min(vec1.y, vec2.y, vec3.y, vec4.y));
    }

inline Vec3Return Min(Vec3In vec1, Vec3In vec2, Vec3In vec3, Vec3In vec4)
    {
        return Vec3(Min(vec1.x, vec2.x, vec3.x, vec4.x),
                    Min(vec1.y, vec2.y, vec3.y, vec4.y),
                    Min(vec1.z, vec2.z, vec3.z, vec4.z));
    }

inline Vec4Return Min(Vec4In vec1, Vec4In vec2, Vec4In vec3, Vec4In vec4)
    {
        return Vec4(Min(vec1.x, vec2.x, vec3.x, vec4.x),
                    Min(vec1.y, vec2.y, vec3.y, vec4.y),
                    Min(vec1.z, vec2.z, vec3.z, vec4.z),
                    Min(vec1.w, vec2.w, vec3.w, vec4.w));
    }
//@}

//@{
//! Component-wise maximum of 4 vectors
//! \param vec1 The first vector
//! \param vec2 The second vector
//! \param vec3 The third vector
//! \param vec4 The fourth vector
//! \return Vector containing the maximum components of vec1, vec2, vec3 and vec4
inline Vec2Return Max(Vec2In vec1, Vec2In vec2, Vec2In vec3, Vec2In vec4)
    {
        return Vec2(Max(vec1.x, vec2.x, vec3.x, vec4.x),
                    Max(vec1.y, vec2.y, vec3.y, vec4.y));
    }

inline Vec3Return Max(Vec3In vec1, Vec3In vec2, Vec3In vec3, Vec3In vec4)
    {
        return Vec3(Max(vec1.x, vec2.x, vec3.x, vec4.x),
                    Max(vec1.y, vec2.y, vec3.y, vec4.y),
                    Max(vec1.z, vec2.z, vec3.z, vec4.z));
    }

inline Vec4Return Max(Vec4In vec1, Vec4In vec2, Vec4In vec3, Vec4In vec4)
    {
        return Vec4(Max(vec1.x, vec2.x, vec3.x, vec4.x),
                    Max(vec1.y, vec2.y, vec3.y, vec4.y),
                    Max(vec1.z, vec2.z, vec3.z, vec4.z),
                    Max(vec1.w, vec2.w, vec3.w, vec4.w));
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Minimum among the components of a vector
//! \param vec Input vector
//! \return Minimum value among the components of the vector
inline PFloat32 Min(Vec2In vec) { return Min(vec.x, vec.y);               }
inline PFloat32 Min(Vec3In vec) { return Min(vec.x, vec.y, vec.z);        }
inline PFloat32 Min(Vec4In vec) { return Min(vec.x, vec.y, vec.z, vec.w); }
//@}

//@{
//! Maximum among the components of a vector
//! \param vec Input vector
//! \return Maximum value among the components of the vector
inline PFloat32 Max(Vec2In vec) { return Max(vec.x, vec.y);               }
inline PFloat32 Max(Vec3In vec) { return Max(vec.x, vec.y, vec.z);        }
inline PFloat32 Max(Vec4In vec) { return Max(vec.x, vec.y, vec.z, vec.w); }
//@}

//@{
//! Component-wise minimum of N vectors
//! \param vec The vectors to compare
//! \param n Number of vectors to compare
//! \return Vector containing the minimum components of the input vectors
Vec2Return Min(const Vec2 * vec, PUInt32 n);
Vec3Return Min(const Vec3 * vec, PUInt32 n);
Vec4Return Min(const Vec4 * vec, PUInt32 n);
//@}

//@{
//! Component-wise maximum of N vectors
//! \param vec The vectors to compare
//! \param n Number of vectors to compare
//! \return Vector containing the maximum components of the input vectors
Vec2Return Max(const Vec2 * vec, PUInt32 n);
Vec3Return Max(const Vec3 * vec, PUInt32 n);
Vec4Return Max(const Vec4 * vec, PUInt32 n);
//@}

//----------------------------------------------------------------------------------------

//@{
//! Round a vector to the lower nearest integer
//! \param vec The vector to round (also the result of the rounding)
inline void Floor(Vec2InOut vec) { vec.x = Floor(vec.x); vec.y = Floor(vec.y);                                             }
inline void Floor(Vec3InOut vec) { vec.x = Floor(vec.x); vec.y = Floor(vec.y); vec.z = Floor(vec.z);                       }
inline void Floor(Vec4InOut vec) { vec.x = Floor(vec.x); vec.y = Floor(vec.y); vec.z = Floor(vec.z); vec.w = Floor(vec.w); }
//@}

//@{
//! Round a vector to the lower nearest integer
//! \param vec The vector to round
//! \return Resulting rounded vector
inline Vec2Return Floor(Vec2In vec) { return Vec2(Floor(vec.x), Floor(vec.y));                             }
inline Vec3Return Floor(Vec3In vec) { return Vec3(Floor(vec.x), Floor(vec.y), Floor(vec.z));               }
inline Vec4Return Floor(Vec4In vec) { return Vec4(Floor(vec.x), Floor(vec.y), Floor(vec.z), Floor(vec.w)); }
//@}

//@{
//! Round a vector to the greater nearest integer
//! \param vec The vector to round (also the result of the rounding)
inline void Ceil(Vec2InOut vec) { vec.x = Ceil(vec.x); vec.y = Ceil(vec.y);                                           }
inline void Ceil(Vec3InOut vec) { vec.x = Ceil(vec.x); vec.y = Ceil(vec.y); vec.z = Ceil(vec.z);                      }
inline void Ceil(Vec4InOut vec) { vec.x = Ceil(vec.x); vec.y = Ceil(vec.y); vec.z = Ceil(vec.z); vec.w = Ceil(vec.w); }
//@}

//@{
//! Round a vector to the greater nearest integer
//! \param vec The vector to round
//! \return Resulting rounded vector
inline Vec2Return Ceil(Vec2In vec) { return Vec2(Ceil(vec.x), Ceil(vec.y));                           }
inline Vec3Return Ceil(Vec3In vec) { return Vec3(Ceil(vec.x), Ceil(vec.y), Ceil(vec.z));              }
inline Vec4Return Ceil(Vec4In vec) { return Vec4(Ceil(vec.x), Ceil(vec.y), Ceil(vec.z), Ceil(vec.w)); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Return the sign of each component of a vector
//! \param vec The input vector that is transformed, per component, to 1.0f if vec
//!             is positive, 0.0f if vec == 0.0f, -1.0f if v is negative
inline void Sign(Vec2InOut vec) { vec.x = Sign(vec.x); vec.y = Sign(vec.y);                                           }
inline void Sign(Vec3InOut vec) { vec.x = Sign(vec.x); vec.y = Sign(vec.y); vec.z = Sign(vec.z);                      }
inline void Sign(Vec4InOut vec) { vec.x = Sign(vec.x); vec.y = Sign(vec.y); vec.z = Sign(vec.z); vec.w = Sign(vec.w); }
//@}

//@{
//! Return the sign of each component of a vector
//! \param vec The input vector
//! \return Resulting vector that contains, per component, 1.0f if vec is positive,
//!         0.0f if vec == 0.0f, -1.0f if v is negative
inline Vec2Return Sign(Vec2In vec) { return Vec2(Sign(vec.x), Sign(vec.y));                           }
inline Vec3Return Sign(Vec3In vec) { return Vec3(Sign(vec.x), Sign(vec.y), Sign(vec.z));              }
inline Vec4Return Sign(Vec4In vec) { return Vec4(Sign(vec.x), Sign(vec.y), Sign(vec.z), Sign(vec.w)); }
//@}

//@{
//! Get the absolute value of a vector
//! \param vec The input vector (also the resulting absolute value vector)
inline void Abs(Vec2InOut vec) { vec.x = Abs(vec.x); vec.y = Abs(vec.y);                                         }
inline void Abs(Vec3InOut vec) { vec.x = Abs(vec.x); vec.y = Abs(vec.y); vec.z = Abs(vec.z);                     }
inline void Abs(Vec4InOut vec) { vec.x = Abs(vec.x); vec.y = Abs(vec.y); vec.z = Abs(vec.z); vec.w = Abs(vec.w); }
//@}

//@{
//! Get the absolute value of a vector
//! \param vec The input vector
//! \return Resulting vector, absolute value of vec
inline Vec2Return Abs(Vec2In vec) { return Vec2(Abs(vec.x), Abs(vec.y));                         }
inline Vec3Return Abs(Vec3In vec) { return Vec3(Abs(vec.x), Abs(vec.y), Abs(vec.z));             }
inline Vec4Return Abs(Vec4In vec) { return Vec4(Abs(vec.x), Abs(vec.y), Abs(vec.z), Abs(vec.w)); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Get the component-wise remainder of vect1 / vect2
//! \param vec1 First input vector (numerator)
//! \param vec2 Second input vector (denominator)
//! \return Component-wise remainder of vec1 / vec2 (same sign as vec1)
inline Vec2Return Mod(Vec2In vec1, Vec2In vec2) { return Vec2(Mod(vec1.x, vec2.x), Mod(vec1.y, vec2.y));                                           }
inline Vec3Return Mod(Vec3In vec1, Vec3In vec2) { return Vec3(Mod(vec1.x, vec2.x), Mod(vec1.y, vec2.y), Mod(vec1.z, vec2.z));                      }
inline Vec4Return Mod(Vec4In vec1, Vec4In vec2) { return Vec4(Mod(vec1.x, vec2.x), Mod(vec1.y, vec2.y), Mod(vec1.z, vec2.z), Mod(vec1.w, vec2.w)); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Swap the content of two vectors
//! \param vec1 First vector
//! \param vec2 Second vector
inline void Swap(Vec2InOut vec1, Vec2InOut vec2)
    {
        PFloat32 t = vec1.x; vec1.x = vec2.x; vec2.x = t;
                 t = vec1.y; vec1.y = vec2.y; vec2.y = t;
    }

inline void Swap(Vec3InOut vec1, Vec3InOut vec2)
    {
        PFloat32 t = vec1.x; vec1.x = vec2.x; vec2.x = t;
                 t = vec1.y; vec1.y = vec2.y; vec2.y = t;
                 t = vec1.z; vec1.z = vec2.z; vec2.z = t;
    }

inline void Swap(Vec4InOut vec1, Vec4InOut vec2)
    {
        PFloat32 t = vec1.x; vec1.x = vec2.x; vec2.x = t;
                 t = vec1.y; vec1.y = vec2.y; vec2.y = t;
                 t = vec1.z; vec1.z = vec2.z; vec2.z = t;
                 t = vec1.w; vec1.w = vec2.w; vec2.w = t;
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Linear interpolation of two vectors
//! \param v1 First vector (for t = 0)
//! \param v2 Second vector (for t = 1)
//! \param t Interpolation coefficient (between 0 and 1 to be valid,
//!          extrapolation occurs if outside these bounds)
//! \return Linear interpolation of v1 and v2 (= (1-t)*v1 + t*v2)
inline Vec2Return Lerp(Vec2In v1, Vec2In v2, PFloat32 t)
    {
        //! \todo Use a different equation to reduce the number of operations?
        const PFloat32 oneMinusT = 1.0f - t;
        return Vec2(oneMinusT * v1.x + t * v2.x,
                    oneMinusT * v1.y + t * v2.y);
    }

inline Vec3Return Lerp(Vec3In v1, Vec3In v2, PFloat32 t)
    {
        //! \todo Use a different equation to reduce the number of operations?
        const PFloat32 oneMinusT = 1.0f - t;
        return Vec3(oneMinusT * v1.x + t * v2.x,
                    oneMinusT * v1.y + t * v2.y,
                    oneMinusT * v1.z + t * v2.z);
    }

inline Vec4Return Lerp(Vec4In v1, Vec4In v2, PFloat32 t)
    {
        //! \todo Use a different equation to reduce the number of operations?
        const PFloat32 oneMinusT = 1.0f - t;
        return Vec4(oneMinusT * v1.x + t * v2.x,
                    oneMinusT * v1.y + t * v2.y,
                    oneMinusT * v1.z + t * v2.z,
                    oneMinusT * v1.w + t * v2.w);
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Cosine of the angles stored in a vector
//! \param vec The input vector (also the resulting vector) (with angles in radians)
inline void Cos(Vec2InOut vec) { vec.x = Cos(vec.x); vec.y = Cos(vec.y);                                         }
inline void Cos(Vec3InOut vec) { vec.x = Cos(vec.x); vec.y = Cos(vec.y); vec.z = Cos(vec.z);                     }
inline void Cos(Vec4InOut vec) { vec.x = Cos(vec.x); vec.y = Cos(vec.y); vec.z = Cos(vec.z); vec.w = Cos(vec.w); }
//@}

//@{
//! Cosine of the angles stored in a vector
//! \param vec The input vector (with angles in radians)
//! \return Resulting vector, cosines of the input vector components (in [-1,1])
inline Vec2Return Cos(Vec2In vec) { return Vec2(Cos(vec.x), Cos(vec.y));                         }
inline Vec3Return Cos(Vec3In vec) { return Vec3(Cos(vec.x), Cos(vec.y), Cos(vec.z));             }
inline Vec4Return Cos(Vec4In vec) { return Vec4(Cos(vec.x), Cos(vec.y), Cos(vec.z), Cos(vec.w)); }
//@}

//@{
//! Sine of the angles stored in a vector
//! \param vec The input vector (also the resulting vector) (with angles in radians)
inline void Sin(Vec2InOut vec) { vec.x = Sin(vec.x); vec.y = Sin(vec.y);                                         }
inline void Sin(Vec3InOut vec) { vec.x = Sin(vec.x); vec.y = Sin(vec.y); vec.z = Sin(vec.z);                     }
inline void Sin(Vec4InOut vec) { vec.x = Sin(vec.x); vec.y = Sin(vec.y); vec.z = Sin(vec.z); vec.w = Sin(vec.w); }
//@}

//@{
//! Sine of the angles stored in a vector
//! \param vec The input vector (with angles in radians)
//! \return Resulting vector, sines of the input vector components (in [-1,1])
inline Vec2Return Sin(Vec2In vec) { return Vec2(Sin(vec.x), Sin(vec.y));                         }
inline Vec3Return Sin(Vec3In vec) { return Vec3(Sin(vec.x), Sin(vec.y), Sin(vec.z));             }
inline Vec4Return Sin(Vec4In vec) { return Vec4(Sin(vec.x), Sin(vec.y), Sin(vec.z), Sin(vec.w)); }
//@}

//@{
//! Tangent of the angles stored in a vector
//! \param vec The input vector (also the resulting vector) (with angles in radians)
inline void Tan(Vec2InOut vec) { vec.x = Tan(vec.x); vec.y = Tan(vec.y);                                         }
inline void Tan(Vec3InOut vec) { vec.x = Tan(vec.x); vec.y = Tan(vec.y); vec.z = Tan(vec.z);                     }
inline void Tan(Vec4InOut vec) { vec.x = Tan(vec.x); vec.y = Tan(vec.y); vec.z = Tan(vec.z); vec.w = Tan(vec.w); }
//@}

//@{
//! Tangent of the angles stored in a vector
//! \param vec The input vector (with angles in radians)
//! \return Resulting vector, tangents of the input vector components
inline Vec2Return Tan(Vec2In vec) { return Vec2(Tan(vec.x), Tan(vec.y));                         }
inline Vec3Return Tan(Vec3In vec) { return Vec3(Tan(vec.x), Tan(vec.y), Tan(vec.z));             }
inline Vec4Return Tan(Vec4In vec) { return Vec4(Tan(vec.x), Tan(vec.y), Tan(vec.z), Tan(vec.w)); }
//@}

//@{
//! Cotangent of the angles stored in a vector (= 1.0f / Tan(vec))
//! \param vec The input vector (also the resulting vector) (with angles in radians)
inline void Cotan(Vec2InOut vec) { vec.x = Cotan(vec.x); vec.y = Cotan(vec.y);                                             }
inline void Cotan(Vec3InOut vec) { vec.x = Cotan(vec.x); vec.y = Cotan(vec.y); vec.z = Cotan(vec.z);                       }
inline void Cotan(Vec4InOut vec) { vec.x = Cotan(vec.x); vec.y = Cotan(vec.y); vec.z = Cotan(vec.z); vec.w = Cotan(vec.w); }
//@}

//@{
//! Cotangent of the angles stored in a vector (= 1.0f / Tan(vec))
//! \param vec The input vector (with angles in radians, for which Tan(angle) != 0)
//! \return Resulting vector, cotangents of the input vector components
inline Vec2Return Cotan(Vec2In vec) { return Vec2(Cotan(vec.x), Cotan(vec.y));                             }
inline Vec3Return Cotan(Vec3In vec) { return Vec3(Cotan(vec.x), Cotan(vec.y), Cotan(vec.z));               }
inline Vec4Return Cotan(Vec4In vec) { return Vec4(Cotan(vec.x), Cotan(vec.y), Cotan(vec.z), Cotan(vec.w)); }
//@}

//@{
//! Arccosine of the components of a vector
//! \param vec The input vector (in [-1,1]) (also the resulting vector in [0,pi]) 
inline void Acos(Vec2InOut vec) { vec.x = Acos(vec.x); vec.y = Acos(vec.y);                                           }
inline void Acos(Vec3InOut vec) { vec.x = Acos(vec.x); vec.y = Acos(vec.y); vec.z = Acos(vec.z);                      }
inline void Acos(Vec4InOut vec) { vec.x = Acos(vec.x); vec.y = Acos(vec.y); vec.z = Acos(vec.z); vec.w = Acos(vec.w); }
//@}

//@{
//! Arccosine of the components of a vector
//! \param vec The input vector (in [-1,1])
//! \return Resulting vector, arccosines of the input vector components (in [0,pi]) 
inline Vec2Return Acos(Vec2In vec) { return Vec2(Acos(vec.x), Acos(vec.y));                           }
inline Vec3Return Acos(Vec3In vec) { return Vec3(Acos(vec.x), Acos(vec.y), Acos(vec.z));              }
inline Vec4Return Acos(Vec4In vec) { return Vec4(Acos(vec.x), Acos(vec.y), Acos(vec.z), Acos(vec.w)); }
//@}

//@{
//! Arcsine of the components of a vector
//! \param vec The input vector (in [-1,1]) (also the resulting vector in [-pi/2,pi/2]) 
inline void Asin(Vec2InOut vec) { vec.x = Asin(vec.x); vec.y = Asin(vec.y);                                           }
inline void Asin(Vec3InOut vec) { vec.x = Asin(vec.x); vec.y = Asin(vec.y); vec.z = Asin(vec.z);                      }
inline void Asin(Vec4InOut vec) { vec.x = Asin(vec.x); vec.y = Asin(vec.y); vec.z = Asin(vec.z); vec.w = Asin(vec.w); }
//@}

//@{
//! Arcsine of the components of a vector
//! \param vec The input vector (in [-1,1])
//! \return Resulting vector, arcsines of the input vector components (in [-pi/2,pi/2]) 
inline Vec2Return Asin(Vec2In vec) { return Vec2(Asin(vec.x), Asin(vec.y));                           }
inline Vec3Return Asin(Vec3In vec) { return Vec3(Asin(vec.x), Asin(vec.y), Asin(vec.z));              }
inline Vec4Return Asin(Vec4In vec) { return Vec4(Asin(vec.x), Asin(vec.y), Asin(vec.z), Asin(vec.w)); }
//@}

//@{
//! Arctangent of the components of a vector
//! \param vec The input vector (also the resulting vector in ]-pi/2,pi/2[) 
inline void Atan(Vec2InOut vec) { vec.x = Atan(vec.x); vec.y = Atan(vec.y);                                           }
inline void Atan(Vec3InOut vec) { vec.x = Atan(vec.x); vec.y = Atan(vec.y); vec.z = Atan(vec.z);                      }
inline void Atan(Vec4InOut vec) { vec.x = Atan(vec.x); vec.y = Atan(vec.y); vec.z = Atan(vec.z); vec.w = Atan(vec.w); }
//@}

//@{
//! Arctangent of the components of a vector
//! \param vec The input vector
//! \return Resulting vector, arctangents of the input vector components (in ]-pi/2,pi/2[) 
inline Vec2Return Atan(Vec2In vec) { return Vec2(Atan(vec.x), Atan(vec.y));                           }
inline Vec3Return Atan(Vec3In vec) { return Vec3(Atan(vec.x), Atan(vec.y), Atan(vec.z));              }
inline Vec4Return Atan(Vec4In vec) { return Vec4(Atan(vec.x), Atan(vec.y), Atan(vec.z), Atan(vec.w)); }
//@}

//@{
//! Arctangent of the component-wise division of two vectors
//! \param vec1 First input vector (numerator)
//! \param vec2 Second input vector (denominator) (can be equal to 0)
//! \return Arctangent of the component-wise vec1 / vec2
//!         (in ]-pi/2,pi/2[, 0 if vec2 = 0)
inline Vec2Return Atan2(Vec2In vec1, Vec2In vec2) { return Vec2(Atan2(vec1.x, vec2.x), Atan2(vec1.y, vec2.y));                                               }
inline Vec3Return Atan2(Vec3In vec1, Vec3In vec2) { return Vec3(Atan2(vec1.x, vec2.x), Atan2(vec1.y, vec2.y), Atan2(vec1.z, vec2.z));                        }
inline Vec4Return Atan2(Vec4In vec1, Vec4In vec2) { return Vec4(Atan2(vec1.x, vec2.x), Atan2(vec1.y, vec2.y), Atan2(vec1.z, vec2.z), Atan2(vec1.w, vec2.w)); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Get vec raised to the power of y
//! \param vec Base vector (also the result of the operation)
//! \param y Exponent (>= 0 if vec = 0)
inline void Pow(Vec2InOut vec, PFloat32 y) { vec.x = Pow(vec.x, y); vec.y = Pow(vec.y, y); }
inline void Pow(Vec3InOut vec, PFloat32 y) { vec.x = Pow(vec.x, y); vec.y = Pow(vec.y, y); vec.z = Pow(vec.z, y); }
inline void Pow(Vec4InOut vec, PFloat32 y) { vec.x = Pow(vec.x, y); vec.y = Pow(vec.y, y); vec.z = Pow(vec.z, y); vec.w = Pow(vec.w, y); }
//@}

//@{
//! Get vec raised to the power of y
//! \param vec Base vector
//! \param y Exponent (>= 0 if vec = 0)
//! \return Resulting vector
inline Vec2Return Pow(Vec2In vec, PFloat32 y) { return Vec2(Pow(vec.x, y), Pow(vec.y, y));                               }
inline Vec3Return Pow(Vec3In vec, PFloat32 y) { return Vec3(Pow(vec.x, y), Pow(vec.y, y), Pow(vec.z, y));                }
inline Vec4Return Pow(Vec4In vec, PFloat32 y) { return Vec4(Pow(vec.x, y), Pow(vec.y, y), Pow(vec.z, y), Pow(vec.w, y)); }
//@}

//@{
//! Natural logarithm of the components of a vector
//! \param vec The input vector (> 0.0) (also the resulting vector)
inline void Log(Vec2InOut vec) { vec.x = Log(vec.x); vec.y = Log(vec.y);                                         }
inline void Log(Vec3InOut vec) { vec.x = Log(vec.x); vec.y = Log(vec.y); vec.z = Log(vec.z);                     }
inline void Log(Vec4InOut vec) { vec.x = Log(vec.x); vec.y = Log(vec.y); vec.z = Log(vec.z); vec.w = Log(vec.w); }
inline void Ln (Vec2InOut vec) { vec.x = Ln (vec.x); vec.y = Ln (vec.y);                                         }
inline void Ln (Vec3InOut vec) { vec.x = Ln (vec.x); vec.y = Ln (vec.y); vec.z = Ln (vec.z);                     }
inline void Ln (Vec4InOut vec) { vec.x = Ln (vec.x); vec.y = Ln (vec.y); vec.z = Ln (vec.z); vec.w = Ln (vec.w); }
//@}

//@{
//! Natural logarithm of the components of a vector
//! \param vec The input vector (> 0.0)
//! \return Resulting vector, natural logarithm of the input vector components
inline Vec2Return Log(Vec2In vec) { return Vec2(Log(vec.x), Log(vec.y));                         }
inline Vec3Return Log(Vec3In vec) { return Vec3(Log(vec.x), Log(vec.y), Log(vec.z));             }
inline Vec4Return Log(Vec4In vec) { return Vec4(Log(vec.x), Log(vec.y), Log(vec.z), Log(vec.w)); }
inline Vec2Return Ln (Vec2In vec) { return Vec2(Ln (vec.x), Ln (vec.y));                         }
inline Vec3Return Ln (Vec3In vec) { return Vec3(Ln (vec.x), Ln (vec.y), Ln (vec.z));             }
inline Vec4Return Ln (Vec4In vec) { return Vec4(Ln (vec.x), Ln (vec.y), Ln (vec.z), Ln (vec.w)); }
//@}

//@{
//! Base 10 logarithm of the components of a vector
//! \param vec The input vector (> 0.0) (also the resulting vector)
inline void Log10(Vec2InOut vec) { vec.x = Log10(vec.x); vec.y = Log10(vec.y);                                             }
inline void Log10(Vec3InOut vec) { vec.x = Log10(vec.x); vec.y = Log10(vec.y); vec.z = Log10(vec.z);                       }
inline void Log10(Vec4InOut vec) { vec.x = Log10(vec.x); vec.y = Log10(vec.y); vec.z = Log10(vec.z); vec.w = Log10(vec.w); }
//@}

//@{
//! Base 10 logarithm of the components of a vector
//! \param vec The input vector (> 0.0)
//! \return Resulting vector, base 10 logarithm of the input vector components
inline Vec2Return Log10(Vec2In vec) { return Vec2(Log10(vec.x), Log10(vec.y));                             }
inline Vec3Return Log10(Vec3In vec) { return Vec3(Log10(vec.x), Log10(vec.y), Log10(vec.z));               }
inline Vec4Return Log10(Vec4In vec) { return Vec4(Log10(vec.x), Log10(vec.y), Log10(vec.z), Log10(vec.w)); }
//@}

//@{
//! Exponential of the components of a vector
//! \param vec The input vector (> 0.0) (also the resulting vector)
inline void Exp(Vec2InOut vec) { vec.x = Exp(vec.x); vec.y = Exp(vec.y);                                         }
inline void Exp(Vec3InOut vec) { vec.x = Exp(vec.x); vec.y = Exp(vec.y); vec.z = Exp(vec.z);                     }
inline void Exp(Vec4InOut vec) { vec.x = Exp(vec.x); vec.y = Exp(vec.y); vec.z = Exp(vec.z); vec.w = Exp(vec.w); }
//@}

//@{
//! Exponential of the components of a vector
//! \param vec The input vector (> 0.0)
//! \return Resulting vector, exponential of the input vector components (> 0)
inline Vec2Return Exp(Vec2In vec) { return Vec2(Exp(vec.x), Exp(vec.y));                         }
inline Vec3Return Exp(Vec3In vec) { return Vec3(Exp(vec.x), Exp(vec.y), Exp(vec.z));             }
inline Vec4Return Exp(Vec4In vec) { return Vec4(Exp(vec.x), Exp(vec.y), Exp(vec.z), Exp(vec.w)); }
//@}

//----------------------------------------------------------------------------------------

//! Conversion from polar to cartesian coordinates (in the XZ plane)
//! \param radius Distance from the center
//! \param rho Polar angle (periodic, 0 = X axis -> 2pi)
//! \return Cartesian coordinates
Vec2Return PolarToCartesian(PFloat32 radius, PFloat32 rho);

//! Conversion from polar to cartesian coordinates (in the XZ plane) (on a unit circle)
//! \param rho Polar angle (periodic, 0 = X axis -> 2pi)
//! \return Cartesian coordinates
Vec2Return PolarToCartesian(PFloat32 rho);

//! Conversion from cartesian to polar coordinates (in the XZ plane)
//! \param radius Resulting distance from the center
//! \param rho Resulting polar angle (in [0,2pi[)
//! \param coords Input cartesian coordinates
void CartesianToPolar(PFloat32 & radius, PFloat32 & rho, Vec2In coords);

//! Conversion from cartesian to polar coordinates (in the XZ plane)
//! \param radius Resulting distance from the center
//! \param rho Resulting polar angle (in [0,2pi[)
//! \param coords Input cartesian coordinates (y is ignored)
void CartesianToPolar(PFloat32 & radius, PFloat32 & rho, Vec3In coords);

//! Conversion from cylindrical to cartesian coordinates
//! (Y is the vertical axis, the base circle is on the XZ plane)
//! \param radius Distance from the center
//! \param rho Azimuthal angle (periodic, 0 = XY plane -> 2pi)
//! \param height Distance from the XZ plane
//! \return Cartesian coordinates
Vec3Return CylindricalToCartesian(PFloat32 radius, PFloat32 rho, PFloat32 height);

//! Conversion from cylindrical to cartesian coordinates (unit radius)
//! (Y is the vertical axis, the base circle is on the XZ plane)
//! \param rho Azimuthal angle (periodic, 0 = XY plane -> 2pi)
//! \param height Distance from the XZ plane
//! \return Cartesian coordinates
Vec3Return CylindricalToCartesian(PFloat32 rho, PFloat32 height);

//! Conversion from cylindrical to cartesian coordinates (unit radius and height)
//! (Y is the vertical axis, the base circle is on the XZ plane)
//! \param rho Azimuthal angle (periodic, 0 = XY plane -> 2pi)
//! \return Cartesian coordinates
Vec3Return CylindricalToCartesian(PFloat32 rho);

//! Conversion from cartesian to cylindrical coordinates
//! (Y is the vertical axis, the base circle is on the XZ plane)
//! \param radius Resulting distance from the center
//! \param rho Resulting azimuthal angle (in [0,2pi[)
//! \param height Resulting distance from the XZ plane
//! \param coords Input cartesian coordinates
//! \return Cartesian coordinates
void CartesianToCylindrical(PFloat32 & radius, PFloat32 & rho, PFloat32 & height, Vec3In coords);

//! Conversion from spherical to cartesian coordinates
//! (Y is the vertical axis, corresponds to theta = 0)
//! \param radius Distance from the center
//! \param phi Azimuthal angle (periodic, 0 = XY plane -> 2pi)
//! \param theta Elevation angle (0 = Y axis -> PI = -Y axis)
//! \return Cartesian coordinates
Vec3Return SphericalToCartesian(PFloat32 radius, PFloat32 phi, PFloat32 theta);

//! Conversion from spherical to cartesian coordinates (on a unit sphere)
//! (Y is the vertical axis, corresponds to theta = 0)
//! \param phi Azimuthal angle (periodic, 0 = XY plane -> 2pi)
//! \param theta Elevation angle (0 = Y axis -> PI = -Y axis)
//! \return Cartesian coordinates
Vec3Return SphericalToCartesian(PFloat32 phi, PFloat32 theta);

//! Conversion from cartesian to spherical coordinates
//! (Y is the vertical axis, corresponds to theta = 0)
//! \param radius Resulting distance from the center
//! \param phi Resulting azimuthal angle (in [0,2pi[)
//! \param theta Resulting elevation angle (in [0,pi])
//! \param coords Input cartesian coordinates
void CartesianToSpherical(PFloat32 & radius, PFloat32 & phi, PFloat32 & theta, Vec3In coords);

//! Conversion from cartesian to spherical coordinates (only computes phi)
//! (Y is the vertical axis, corresponds to theta = 0)
//! \param phi Resulting azimuthal angle (in [0,2pi[)
//! \param coords Input cartesian coordinates
void CartesianToSphericalPhi(PFloat32 & phi, Vec3In coords);

//! Conversion from cartesian to spherical coordinates (only computes theta)
//! (Y is the vertical axis, corresponds to theta = 0)
//! \param theta Resulting elevation angle (in [0,pi])
//! \param coords Input cartesian coordinates
void CartesianToSphericalTheta(PFloat32 & theta, Vec3In coords);


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_VECTOR_H
