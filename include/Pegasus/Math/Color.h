/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Color.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Color types and operations

#ifndef PEGASUS_MATH_COLOR_H
#define PEGASUS_MATH_COLOR_H

#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Math {


//! (red, green, blue) color defined by float numbers
//! (0.0f -> 1.0f represents normal range, but can go further)
typedef Vec3 ColorRGB;

//! (red, green, blue, alpha) color defined by float numbers
//! (0.0f -> 1.0f represents normal range, but can go further,
//!  alpha has to be in [0.0,1.0])
typedef Vec4 ColorRGBA;


//! Function input type for an RGB color
typedef const ColorRGB &  ColorRGBIn;

//! Function input/output type for an RGB color
typedef       ColorRGB &  ColorRGBInOut;

//! Function return type for an RGB color
typedef       ColorRGB    ColorRGBReturn;

//! Function input type for an RGBA color
typedef const ColorRGBA & ColorRGBAIn;

//! Function input/output type for an RGBA color
typedef       ColorRGBA & ColorRGBAInOut;

//! Function return type for an RGBA color
typedef       ColorRGBA   ColorRGBAReturn;

//----------------------------------------------------------------------------------------

//! Color without alpha component and stored with 8-bit integer
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Color8RGB
{
    PUInt8 v[3];                                //!< Array version of the color
    PUInt8 rgb[3];                              //!< Array version of the color
    struct { PUInt8 red, green, blue; };     //!< Individual colors

    Color8RGB()                                 //!< Default constructor
        : red(0), green(0), blue(0) { }

    Color8RGB(const Color8RGB & c)              //!< Copy constructor
        : red(c.red), green(c.green), blue(c.blue) { }

    //Color8RGB(const Color8RGBA & c)             //!< Constructor that removes the alpha component
    //    : red(c.red), green(c.green), blue(c.blue) { }

    Color8RGB(ColorRGBIn c)               //!< Constructor that converts float numbers (clamps between 0.0 and 1.0)
        {
            const ColorRGB cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
        }

    Color8RGB(ColorRGBAIn c)              //!< Constructor that converts float numbers (clamps between 0.0 and 1.0)
        {
            const ColorRGBA cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
        }

    //Color8RGB(const ColorRGBA & c)              //!< Constructor that converts float
    //                                            //!< numbers (clamps between 0.0 and 1.0)
    //                                            //!< and removes the alpha component
    //    {
    //        const ColorRGBA c = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
    //        red   = (PUInt8)c.red;
    //        green = (PUInt8)c.green;
    //        blue  = (PUInt8)c.blue;
    //    }

    Color8RGB(PUInt8 red, PUInt8 green, PUInt8 blue)    //!< Constructor with single components
        : red(red), green(green), blue(blue) { }

    inline Color8RGB & operator = (const Color8RGB & c) //!< Assignment operator
        {
            red   = c.red;
            green = c.green;
            blue  = c.blue;
            return *this;
        }

    inline Color8RGB & operator = (ColorRGBIn c)  //!< Assignment operator with conversion
        {
            const ColorRGB cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
            return *this;
        }

    inline Color8RGB & operator = (ColorRGBAIn c) //!< Assignment operator with conversion
        {
            const ColorRGBA cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
            return *this;
        }
};

//----------------------------------------------------------------------------------------

//! Color with alpha component and stored with 8-bit integer
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Color8RGBA
{
    PUInt8 v[4];                                //!< Array version of the color
    PUInt8 rgba[4];                             //!< Array version of the color
    PUInt32 rgba32;                             //!< 32 bits integer version of the color
    struct { PUInt8 rgb[3]; PUInt8 alpha; };    //!< RGB and alpha
    struct { PUInt8 red, green, blue, alpha; }; //!< Individual colors

    Color8RGBA()                                //!< Default constructor
        : red(0), green(0), blue(0), alpha(255) { }

    Color8RGBA(const Color8RGBA & c)            //!< Copy constructor
        : red(c.red), green(c.green), blue(c.blue), alpha(c.alpha) { }

    Color8RGBA(const Color8RGB & c)             //!< Constructor that adds the alpha component
        : red(c.red), green(c.green), blue(c.blue), alpha(255) { }

    Color8RGBA(ColorRGBIn c)              //!< Constructor that converts float numbers (clamps between 0.0 and 1.0) and adds the alpha component
        : alpha(255)
        {
            ColorRGB cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
        }

    Color8RGBA(ColorRGBAIn c)             //!< Constructor that converts float numbers (clamps between 0.0 and 1.0)
        {
            ColorRGBA cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
            alpha = (PUInt8)cf.alpha;
        }

    Color8RGBA(PUInt8 red, PUInt8 green, PUInt8 blue, PUInt8 alpha) //!< Constructor with single components
        : red(red), green(green), blue(blue), alpha(alpha) { }

    inline Color8RGBA & operator = (const Color8RGBA & c)           //!< Assignment operator
        {
            red   = c.red;
            green = c.green;
            blue  = c.blue;
            alpha = c.alpha;
            return *this;
        }

    inline Color8RGBA & operator = (const Color8RGB & c)            //!< Assignment operator that adds the alpha component
        {
            red   = c.red;
            green = c.green;
            blue  = c.blue;
            alpha = 255;
            return *this;
        }

    inline Color8RGBA & operator = (ColorRGBIn c)             //!< Assignment operator conversion that adds the alpha component
        {
            const ColorRGB cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
            alpha = 255;
            return *this;                        }

    inline Color8RGBA & operator = (ColorRGBAIn c)            //!< Assignment operator with conversion
        {
            const ColorRGBA cf = Floor(Clamp(c, 0.0f, 1.0f) * 255.0f);
            red   = (PUInt8)cf.red;
            green = (PUInt8)cf.green;
            blue  = (PUInt8)cf.blue;
            alpha = (PUInt8)cf.alpha;
            return *this;
        }
};

//----------------------------------------------------------------------------------------

//! Function input type for an 8-bit RGB color
typedef const Color8RGB &  Color8RGBIn;

//! Function input/output type for an 8-bit RGB color
typedef       Color8RGB &  Color8RGBInOut;

//! Function return type for an 8-bit RGB color
typedef       Color8RGB    Color8RGBReturn;

//! Function input type for an 8-bit RGBA color
typedef const Color8RGBA & Color8RGBAIn;

//! Function input/output type for an 8-bit RGBA color
typedef       Color8RGBA & Color8RGBAInOut;

//! Function return type for an 8-bit RGBA color
typedef       Color8RGBA   Color8RGBAReturn;

//----------------------------------------------------------------------------------------

//! \todo Make those class constants

//! Predefined colors
const ColorRGB N3DCOLOR_BLACK        (0.0f , 0.0f , 0.0f );
const ColorRGB N3DCOLOR_DARKGRAY     (0.25f, 0.25f, 0.25f);
const ColorRGB N3DCOLOR_GRAY         (0.5f , 0.5f , 0.5f );
const ColorRGB N3DCOLOR_LIGHTGRAY    (0.75f, 0.75f, 0.75f);
const ColorRGB N3DCOLOR_WHITE        (1.0f , 1.0f , 1.0f );

const ColorRGB N3DCOLOR_RED          (1.0f , 0.0f , 0.0f );
const ColorRGB N3DCOLOR_GREEN        (0.0f , 1.0f , 0.0f );
const ColorRGB N3DCOLOR_BLUE         (0.0f , 0.0f , 1.0f );
const ColorRGB N3DCOLOR_CYAN         (0.0f , 1.0f , 1.0f );
const ColorRGB N3DCOLOR_MAGENTA      (1.0f , 0.0f , 1.0f );
const ColorRGB N3DCOLOR_YELLOW       (1.0f , 1.0f , 0.0f );

const ColorRGB N3DCOLOR_DARKRED      (0.5f , 0.0f , 0.0f );
const ColorRGB N3DCOLOR_DARKGREEN    (0.0f , 0.5f , 0.0f );
const ColorRGB N3DCOLOR_DARKBLUE     (0.0f , 0.0f , 0.5f );
const ColorRGB N3DCOLOR_DARKCYAN     (0.0f , 0.5f , 0.5f );
const ColorRGB N3DCOLOR_DARKMAGENTA  (0.5f , 0.0f , 0.5f );
const ColorRGB N3DCOLOR_DARKYELLOW   (0.5f , 0.5f , 0.0f );

const Color8RGB N3DCOLOR8_BLACK      (0  , 0  , 0  );
const Color8RGB N3DCOLOR8_DARKGRAY   (64 , 64 , 64 );
const Color8RGB N3DCOLOR8_GRAY       (128, 128, 128);
const Color8RGB N3DCOLOR8_LIGHTGRAY  (192, 192, 192);
const Color8RGB N3DCOLOR8_WHITE      (255, 255, 255);

const Color8RGB N3DCOLOR8_RED        (255, 0  , 0  );
const Color8RGB N3DCOLOR8_GREEN      (0  , 255, 0  );
const Color8RGB N3DCOLOR8_BLUE       (0  , 0  , 255);
const Color8RGB N3DCOLOR8_CYAN       (0  , 255, 255);
const Color8RGB N3DCOLOR8_MAGENTA    (255, 0  , 255);
const Color8RGB N3DCOLOR8_YELLOW     (255, 255, 0  );

const Color8RGB N3DCOLOR8_DARKRED    (128, 0  , 0  );
const Color8RGB N3DCOLOR8_DARKGREEN  (0  , 128, 0  );
const Color8RGB N3DCOLOR8_DARKBLUE   (0  , 0  , 128);
const Color8RGB N3DCOLOR8_DARKCYAN   (0  , 128, 128);
const Color8RGB N3DCOLOR8_DARKMAGENTA(128, 0  , 128);
const Color8RGB N3DCOLOR8_DARKYELLOW (128, 128, 0  );

//----------------------------------------------------------------------------------------

// Conversion functions

//! \todo Convert to class members

//! Remove the alpha component of a color defined with float numbers
//! \param color Color to convert
//! \return RGB color defined with float numbers
inline ColorRGB ToColorRGB(const ColorRGBA & color)
    { return ColorRGB(color.red, color.green, color.blue); }

//! Convert an RGB color defined with 8-bit integers to float numbers
//! \param color Color to convert
//! \return RGB color defined with float numbers
inline ColorRGB ToColorRGB(const Color8RGB & color)
    {
        return ColorRGB(PFloat32(color.red  ) * 255.0f,
                        PFloat32(color.green) * 255.0f,
                        PFloat32(color.blue ) * 255.0f);
    }

//! Convert an RGB color defined with 8-bit integers to float numbers
//! and remove the alpha component
//! \param color Color to convert
//! \return RGB color defined with float numbers
inline ColorRGB ToColorRGB(const Color8RGBA & color)
    {
        return ColorRGB(PFloat32(color.red  ) * 255.0f,
                        PFloat32(color.green) * 255.0f,
                        PFloat32(color.blue ) * 255.0f);
    }

//! Add an alpha component to a color defined with float numbers
//! \param color Color to convert
//! \return RGBA color defined with float numbers (alpha = 1.0f)
inline ColorRGBA ToColorRGBA(const ColorRGB & color)
    { return ColorRGBA(color.red, color.green, color.blue, 1.0f); }

//! Convert an RGB color defined with 8-bit integers to float numbers
//! and add an alpha component
//! \param color Color to convert
//! \return RGBA color defined with float numbers (alpha = 1.0f)
inline ColorRGBA ToColorRGBA(const Color8RGB & color)
    {
        return ColorRGBA(PFloat32(color.red  ) * 255.0f,
                         PFloat32(color.green) * 255.0f,
                         PFloat32(color.blue ) * 255.0f, 1.0f);
    }

//! Convert an RGBA color defined with 8-bit integers to float numbers
//! \param color Color to convert
//! \return RGBA color defined with float numbers
inline ColorRGBA ToColorRGBA(const Color8RGBA & color)
    {
        return ColorRGBA(PFloat32(color.red  ) * 255.0f,
                         PFloat32(color.green) * 255.0f,
                         PFloat32(color.blue ) * 255.0f,
                         PFloat32(color.alpha) * 255.0f);
    }

//! Convert from an RGB color defined with 8 bits per component to a grayscale value
//! \param color Color to convert
//! \return Grayscale value, from 0 to 255
inline PUInt8 ToGrayscale(const Color8RGB & color)
    {
        //return Min( (  (unsigned short)color.red   * 77
        //             + (unsigned short)color.green * 151
        //             + (unsigned short)color.blue  * 28 ) >> 8, 255);
        return (PUInt8)Min(PUInt16( ( (PUInt16)color.red   * 54
                                    + (PUInt16)color.green * 183
                                    + (PUInt16)color.blue  * 18 ) >> 8),
                           PUInt16(255));
    }

//! Convert from an RGBA color defined with 8 bits per component to a grayscale value
//! (the alpha component is ignored)
//! \param color Color to convert
//! \return Grayscale value, from 0 to 255
inline PUInt8 ToGrayscale(const Color8RGBA & color)
    {
        //return Min( (  (unsigned short)color.red   * 77
        //             + (unsigned short)color.green * 151
        //             + (unsigned short)color.blue  * 28 ) >> 8, 255);
        return (PUInt8)Min(PUInt16( ( (PUInt16)color.red   * 54
                                    + (PUInt16)color.green * 183
                                    + (PUInt16)color.blue  * 18 ) >> 8),
                           PUInt16(255));
  }

//! \todo Add differentiation between gamma and non-gamma spaces

//! Convert from an RGB color defined with float numbers to a grayscale value
//! \param color Color to convert
//! \return Grayscale value
inline PFloat32 ToGrayscale(const ColorRGB & color)
    {
        // return color.red * 0.3f + color.green * 0.59f + color.blue * 0.11f;
        return color.red * 0.2126f + color.green * 0.7152f + color.blue * 0.0722f;
    }

//! Convert from an RGB color defined with float numbers to a grayscale value
//! (the alpha component is ignored)
//! \param color Color to convert
//! \return Grayscale value
inline PFloat32 ToGrayscale(const ColorRGBA & color)
    {
        // return color.red * 0.3f + color.green * 0.59f + color.blue * 0.11f;
        return color.red * 0.2126f + color.green * 0.7152f + color.blue * 0.0722f;
    }

//----------------------------------------------------------------------------------------

//@{
//! For colors, test if it is black (alpha is ignored for the test)
//! \param color Color to test
//! \return true if the color is black
inline bool IsBlack(const ColorRGB  & color)
    { return (color.red <= 0.0f) && (color.green <= 0.0f) && (color.blue <= 0.0f); }

inline bool IsBlack(const ColorRGBA & color)
    { return (color.red <= 0.0f) && (color.green <= 0.0f) && (color.blue <= 0.0f); }

inline bool IsBlack(const Color8RGB  & color)
    { return (color.red == 0) && (color.green == 0) && (color.blue == 0); }

inline bool IsBlack(const Color8RGBA & color)
    { return (color.red == 0) && (color.green == 0) && (color.blue == 0); }

//@}


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_COLOR_H
