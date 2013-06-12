#include "Pegasus/Render/GL/GLExtensions.h"
#include "Pegasus/Libs/GLEW/glew.h"
#include "Pegasus/Libs/GLEW/wglew.h"

//! \todo Make this VStudio specific
//! \todo Make the lib static in rel mode
#pragma comment (lib, "glew32.lib")


namespace Pegasus {
namespace Render {


GLExtensions * GLExtensions::sInstance = nullptr;

//----------------------------------------------------------------------------------------

void GLExtensions::CreateInstance()
{
    if (sInstance == nullptr)
    {
        //! \todo Allocator management
        sInstance = new GLExtensions;
    }
    else
    {
        //! \todo Assertion error
    }
}

//----------------------------------------------------------------------------------------

void GLExtensions::DestroyInstance()
{
    if (sInstance != nullptr)
    {
        delete sInstance;
        sInstance = nullptr;
    }
    else
    {
        //! \todo Assertion error
    }
}

//----------------------------------------------------------------------------------------

GLExtensions::GLExtensions()
{
    mGLEWInitialized = false;

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        //! \todo Proper error handling
        //fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(error));

        return;
    }

    //! \todo Handle logging
    //fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

    mGLEWInitialized = true;
}

//----------------------------------------------------------------------------------------

GLExtensions::Profile GLExtensions::GetMaximumProfile() const
{
    if (mGLEWInitialized)
    {
        if (GLEW_VERSION_4_3)
        {
            return PROFILE_GL_4_3;
        }
        else if (GLEW_VERSION_3_3)
        {
            return PROFILE_GL_3_3;
        }
        else
        {
            return PROFILE_NO_GL;
        }

    }
    else
    {
        return PROFILE_NO_GL;
    }
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsProfileSupported(Profile profile) const
{
    if (!mGLEWInitialized)
    {
        //! \todo Assertion for mGLEWInitialized

        return false;
    }

    if (IsProfileValid(profile))
    {
        const Profile maxProfile = GetMaximumProfile();
        if (IsProfileValid(maxProfile))
        {
            return (profile <= GetMaximumProfile());
        }
        else
        {
            // Invalid max profile, or OpenGL unsupported
            return false;
        }
    }
    else
    {
        //! \todo Assertion error

        return false;
    }
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsGLExtensionSupported(const char * name) const
{
    if (!mGLEWInitialized)
    {
        //! \todo Assertion for mGLEWInitialized

        return false;
    }

    if (name != nullptr)
    {
        return (glewIsSupported(name) != 0);
    }
    else
    {
        //! \todo Assertion for empty strings

        return false;
    }
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsWGLExtensionSupported(const char * name) const
{
    if (!mGLEWInitialized)
    {
        //! \todo Assertion for mGLEWInitialized

        return false;
    }

    if (name != nullptr)
    {
        return (wglewIsSupported(name) != 0);
    }
    else
    {
        //! \todo Assertion for empty strings

        return false;
    }
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsProfileValid(Profile profile) const
{
    return (profile > PROFILE_NO_GL) && (profile < PROFILE_MAX);
}


}   // namespace Render
}   // namespace Pegasus
