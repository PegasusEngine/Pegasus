/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLExtensions.h
//! \author	Kevin Boulanger
//! \date	11th June 2013
//! \brief	Management of GL and WGL extensions, available throughout the engine
//!         to check if features are available before using them

#ifndef PEGASUS_RENDER_GLEXTENSIONS_H
#define PEGASUS_RENDER_GLEXTENSIONS_H

namespace Pegasus {
namespace Render {


//! \class Singleton that manages GL and WGL extensions, available throughout the engine
//!        to check if features are available before using them
class GLExtensions
{
public:

    //! Create the unique instance of the class
    static void CreateInstance();

    //! Destroy the unique instance of the class
    static void DestroyInstance();

    //! Get the unique instance of the class
    inline static GLExtensions & GetInstance() { return *sInstance; }


    //! Definition of available OpenGL profiles
    enum Profile
    {
        PROFILE_NO_GL,      // No proper OpenGL support
        PROFILE_GL_3_3,     // GLSL 3.30, VS+GS+PS, samplers, floating point textures, instancing
        PROFILE_GL_4_3,     // GLSL 4.30, tessellation, compute shaders
        PROFILE_MAX
    };

    //! Get the maximum profile handled by the GPU and the driver
    //! \return PROFILE_x constant representing the maximal profile handled by the GPU.
    //!         All the profiles with a lower version number are supported.
    Profile GetMaximumProfile() const;

    //! Test if a profile is supported by the GPU and the driver
    //! \param profile Profile to test, PROFILE_x constant
    //! \return True if the input profile is supported
    bool IsProfileSupported(Profile profile) const;

    //! Test if a OpenGL extension is supported
    //! \param name Full name of the extension, "GL_ARB_multitexture" for example
    //! \return True if the OpenGL extension is supported
    bool IsGLExtensionSupported(const char * name) const;

    //! Test if a WGL extension is supported
    //! \todo Make this function available only on Windows
    //! \param name Full name of the extension, "WGL_ARB_pbuffer" for example
    //! \return True if the WGL extension is supported
    bool IsWGLExtensionSupported(const char * name) const;


private:

    //! Constructor.
    //! Initializes the extensions manager for OpenGL.
    //! Performs detection of the OpenGL version and the list of extensions available.
    GLExtensions();

    //! Destructor.
    ~GLExtensions();

    //! Unique instance of the class
    static GLExtensions * sInstance;


    //! Test if a profile is a valid one
    //! \param profile Profile to test (PROFILE_x constant)
    //! \return True if the profile is valid (between PROFILE_NO_GL and PROFILE_MAX)
    bool IsProfileValid(Profile profile) const;


    //! True when GLEW has been initialized properly
    bool mGLEWInitialized;
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_GLEXTENSIONS_H
