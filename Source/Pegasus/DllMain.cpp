#include <windows.h>

#include "Pegasus\TestPegasus.h"

// Check for C or C++ interface
#ifdef __cplusplus
extern "C" {
#endif

// Dll entry point, for init/cleanup
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
    return true;
}

// Test exporting a function
int __cdecl testFunc(int val)
{
    return 2 * val;
}

#ifdef __cplusplus
}
#endif


#if 0

// Once the GL initialization has been moved inside the DLL,
// place the following code in the proper locations
// (warning, GLExtensions has to be initialized AFTER OpenGL)

            #include "Pegasus/Render/GL/GLExtensions.h" // for testing



            // Initialize the extensions manager, after OpenGL is initialized
            //! \todo Needs to be done somewhere inside the Pegasus project, not in the launcher
            using namespace Pegasus::Render;
            GLExtensions::CreateInstance();

            // Write some temporary debugging information
            GLExtensions & extensions = GLExtensions::GetInstance();
            switch (extensions.GetMaximumProfile())
            {
                case GLExtensions::PROFILE_GL_3_3:
                    OutputDebugString("OpenGL 3.3 is the maximum detected profile.\n");
                    break;

                case GLExtensions::PROFILE_GL_4_3:
                    OutputDebugString("OpenGL 4.3 is the maximum detected profile.\n");
                    break;

                default:
                    OutputDebugString("Error when initializing GLExtensions.\n");
                    break;
            }
            if (extensions.IsGLExtensionSupported("GL_ARB_draw_indirect"))
            {
                OutputDebugString("GL_ARB_draw_indirect detected.\n");
            }
            else
            {
                OutputDebugString("GL_ARB_draw_indirect NOT detected.\n");
            }
            if (extensions.IsGLExtensionSupported("GL_ATI_fragment_shader"))
            {
                OutputDebugString("GL_ATI_fragment_shader detected.\n");
            }
            else
            {
                OutputDebugString("GL_ATI_fragment_shader NOT detected.\n");
            }
            if (extensions.IsWGLExtensionSupported("WGL_ARB_buffer_region"))
            {
                OutputDebugString("WGL_ARB_buffer_region detected.\n");
            }
            else
            {
                OutputDebugString("WGL_ARB_buffer_region NOT detected.\n");
            }
            if (extensions.IsWGLExtensionSupported("WGL_3DL_stereo_control"))
            {
                OutputDebugString("WGL_3DL_stereo_control detected.\n");
            }
            else
            {
                OutputDebugString("WGL_3DL_stereo_control NOT detected.\n");
            }




            // Destroy the extensions manager
            //! \todo Needs to be done somewhere inside the Pegasus project, not in the launcher
            Pegasus::Render::GLExtensions::DestroyInstance();


#endif