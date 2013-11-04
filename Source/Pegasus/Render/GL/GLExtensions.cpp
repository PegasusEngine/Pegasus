/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLExtensions.cpp
//! \author Kevin Boulanger
//! \date   11th June 2013
//! \brief  Management of GL and WGL extensions, available throughout the engine
//!         to check if features are available before using them

#include "Pegasus/Render/GL/GLExtensions.h"
#include "Pegasus/Render/GL/GLEWStaticInclude.h" 

namespace Pegasus {
namespace Render {

//----------------------------------------------------------------------------------------

GLExtensions::GLExtensions()
    : Core::Singleton<GLExtensions>()
{
    mGLEWInitialized = false;

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        PG_FAILSTR("Glew failed to initialize: %s", glewGetErrorString(error));

        return;
    }

    //! \todo Handle logging
    PG_LOG('OGL_', "Using glew: %u", glewGetString(GLEW_VERSION));

    mGLEWInitialized = true;
}

//----------------------------------------------------------------------------------------

GLExtensions::~GLExtensions()
{
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
        PG_FAILSTR("GLEW is not initialized!");

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
        PG_FAILSTR("Invalid profile: %u", (unsigned int) profile);

        return false;
    }
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsGLExtensionSupported(const char* name) const
{
    if (!mGLEWInitialized)
    {
        PG_FAILSTR("GLEW is not initialized!");

        return false;
    }

    return (glewIsSupported(name) != 0);
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsWGLExtensionSupported(const char* name) const
{
    if (!mGLEWInitialized)
    {
        PG_FAILSTR("GLEW is not initialized!");

        return false;
    }

    return (wglewIsSupported(name) != 0);
}

//----------------------------------------------------------------------------------------

bool GLExtensions::IsProfileValid(Profile profile) const
{
    return (profile > PROFILE_NO_GL) && (profile < PROFILE_MAX);
}


}   // namespace Render
}   // namespace Pegasus
