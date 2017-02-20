/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Preprocessor.h
//! \author Karolyn Boulanger
//! \date   12th June 2013
//! \brief  Preprocessor file for the entire Pegasus engine.
//!         This file is included everywhere, using the Visual Studio project option
//!         C/C++ -> Advanced -> Forced Include File.
//! \warning All defines must be used using #if, and not #ifdef.
//! \warning The usage of the defines with a prefix underscore is limited to the projects,
//!          they must not be used in the code outside of this file.

#ifndef PEGASUS_PREPROCESSOR_H
#define PEGASUS_PREPROCESSOR_H

// Pegasus Engine only define, to differentiate compilation of the engine itself
// with an actual app
#ifdef _PEGASUS_ENGINE
#define PEGASUS_ENGINE          1
#else
#define PEGASUS_ENGINE          0
#endif

//----------------------------------------------------------------------------------------

// Development mode flag, to enable all tools and pipelines
#ifdef _PEGASUS_DEV
#ifdef _PEGASUS_REL
#error Cannot compile Pegasus in DEV and REL modes simultaneously
#endif
#define PEGASUS_DEV             1
#else
#define PEGASUS_DEV             0
#endif

// Release mode flag, to embed only the required code to run a final application
#ifdef _PEGASUS_REL
#ifdef _PEGASUS_DEV
#error Cannot compile Pegasus in DEV and REL modes simultaneously
#endif
#define PEGASUS_REL             1
#else
#define PEGASUS_REL             0
#endif

//----------------------------------------------------------------------------------------

// Debug mode, the slowest but that allows fully featured step debugging
#ifdef _PEGASUS_DEBUG
#define PEGASUS_DEBUG           1
#else
#define PEGASUS_DEBUG           0
#endif

// Optimized mode, optimizing the code for performance but not removing the debugging tools
#ifdef _PEGASUS_OPT
#define PEGASUS_OPT             1
#else
#define PEGASUS_OPT             0
#endif

// Final mode, optimizing the code for performance, with all debugging tools removed
#ifdef _PEGASUS_FINAL
#define PEGASUS_FINAL           1
#else
#define PEGASUS_FINAL           0
#endif

// Profiling mode, equivalent to final, but with performance markers enabled
#ifdef _PEGASUS_PROFILE
#define PEGASUS_PROFILE         1
#else
#define PEGASUS_PROFILE         0
#endif

// Small mode, equivalent to final, but with code optimized for size rather than speed
#ifdef _PEGASUS_SMALL
#define PEGASUS_SMALL           1
#else
#define PEGASUS_SMALL           0
#endif

//----------------------------------------------------------------------------------------

// Platform being compiled
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)

#define PEGASUS_PLATFORM_WIN64      1
#define PEGASUS_PLATFORM_WIN32      0
#define PEGASUS_PLATFORM_WINDOWS    1
#define PEGASUS_PLATFORM_MACOS      0
#define PEGASUS_PLATFORM_LINUX      0

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define PEGASUS_PLATFORM_WIN64      0
#define PEGASUS_PLATFORM_WIN32      1
#define PEGASUS_PLATFORM_WINDOWS    1
#define PEGASUS_PLATFORM_MACOS      0
#define PEGASUS_PLATFORM_LINUX      0

#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)

#define PEGASUS_PLATFORM_WIN64      0
#define PEGASUS_PLATFORM_WIN32      0
#define PEGASUS_PLATFORM_WINDOWS    0
#define PEGASUS_PLATFORM_MACOS      1
#define PEGASUS_PLATFORM_LINUX      0

#elif  defined(linux) || defined(__linux) || defined(__linux__)

#define PEGASUS_PLATFORM_WIN64      0
#define PEGASUS_PLATFORM_WIN32      0
#define PEGASUS_PLATFORM_WINDOWS    0
#define PEGASUS_PLATFORM_MACOS      0
#define PEGASUS_PLATFORM_LINUX      1

#else
#error "Unable to detect the platform in Pegasus/Preprocessor.h"
#endif

//----------------------------------------------------------------------------------------

// detecting 64 bit or 32 bit platform
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)

#define PEGASUS_POINTERSIZE_32BIT  0
#define PEGASUS_POINTERSIZE_64BIT  1

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define PEGASUS_POINTERSIZE_32BIT  1
#define PEGASUS_POINTERSIZE_64BIT  0

#elif defined(__LP64__)

#define PEGASUS_POINTERSIZE_32BIT  0
#define PEGASUS_POINTERSIZE_64BIT  1

#elif defined (_ILP32)

#define PEGASUS_POINTERSIZE_32BIT  1
#define PEGASUS_POINTERSIZE_64BIT  0

#else
#error "Unable to detect if 64bit or 32bit environment"
#endif

//----------------------------------------------------------------------------------------

// Compiler
#ifdef _MSC_VER
#ifdef __INTEL_COMPILER

#define PEGASUS_COMPILER_MSVC       0
#define PEGASUS_COMPILER_ICC        1
#define PEGASUS_COMPILER_GCC        0
#define PEGASUS_COMPILER_UNKNOWN    0

#else

#define PEGASUS_COMPILER_MSVC       1
#define PEGASUS_COMPILER_ICC        0
#define PEGASUS_COMPILER_GCC        0
#define PEGASUS_COMPILER_UNKNOWN    0

#endif  // __INTEL_COMPILER
#elif defined __GNUC__

#define PEGASUS_COMPILER_MSVC       0
#define PEGASUS_COMPILER_ICC        0
#define PEGASUS_COMPILER_GCC        1
#define PEGASUS_COMPILER_UNKNOWN    0

#else

#define PEGASUS_COMPILER_MSVC       0
#define PEGASUS_COMPILER_ICC        0
#define PEGASUS_COMPILER_GCC        0
#define PEGASUS_COMPILER_UNKNOWN    1

#endif

// No support for languages other than C++
#if !defined __cplusplus
#error "Only C++ compilers are supported for Pegasus"
#endif

//----------------------------------------------------------------------------------------

// Graphics API
#if PEGASUS_PLATFORM_WINDOWS

#define PEGASUS_GAPI_GL             0
#define PEGASUS_GAPI_GLES           0
#define PEGASUS_GAPI_DX             1

#else

#define PEGASUS_GAPI_GL             1
#define PEGASUS_GAPI_GLES           0
#define PEGASUS_GAPI_DX             0

#endif

//----------------------------------------------------------------------------------------

// Sound API
#define PEGASUS_SAPI_FMOD           1

//----------------------------------------------------------------------------------------

// DLL mode for the application, used when compiling the app as a dynamic library, in DEV mode
#ifdef _PEGASUSAPP_DLL
#if PEGASUS_REL
#error Pegasus REL mode cannot be compiled as a DLL. This is reserved to DEV mode
#endif
#define PEGASUSAPP_DLL          1
#else
#define PEGASUSAPP_DLL          0
#endif

//----------------------------------------------------------------------------------------

// Flag for importing/exporting objects or functions from the DLL (app in DEV mode)
#if PEGASUS_DEV && PEGASUS_PLATFORM_WINDOWS
#if PEGASUSAPP_DLL
#define PEGASUSAPP_SHARED __declspec(dllexport)
#else
#define PEGASUSAPP_SHARED __declspec(dllimport)
#endif
#else
// No DLL in release mode or in other platforms than Windows
#define PEGASUSAPP_SHARED
#endif

//----------------------------------------------------------------------------------------

//! Macro to disable the copy and assignment operators of a class.
//! Call this macro in the private section of the class.
//! If a copy or assignment is used, the compiler generates an error.
#define PG_DISABLE_COPY(C)		\
	C(const C &);				\
	C & operator=(const C &);	\


// Macro to suppress the following warning in Visual Studio:
// warning LNK4221: no public symbols found; archive member will be inaccessible.
// Place the macro at the top of each .cpp file triggering the warning.
// It is better to place it inside a namespace, but it is not mandatory.
// If an #if block is the reason the file can become empty, place the macro before the #if block.
#if PEGASUS_COMPILER_MSVC
#define PEGASUS_AVOID_EMPTY_FILE_WARNING        namespace { char NoEmptyFileDummy##__LINE__; }
#else
#define PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif

//! Macro combination that sets up struct alignment in c++
#if PEGASUS_COMPILER_MSVC
#define PEGASUS_ALIGN_BEGIN(n) __declspec(align(n))
#define PEGASUS_ALIGN_END(n)
#elif PEGASUS_COMPILER_GCC
#define PEGASUS_ALIGN_BEGIN(n)
#define PEGASUS_ALIGN_END(n) __attribute__ ((aligned(n)))
#elif
    #error "Declare the appropiate alignment set of macros"
#endif


//----------------------------------------------------------------------------------------

// Flags telling if features are enabled based on the compilation profile.
// Use those rather than the ones above to help future maintenance.

// Include the entry point when the engine is compiled as an executable
#define PEGASUS_INCLUDE_LAUNCHER                        (!PEGASUS_DEV)

// Enable logging in the console output
#define PEGASUS_ENABLE_LOG                              (PEGASUS_DEBUG || PEGASUS_OPT)

// Enable the detailed logging
// (performance issue, can flood the log window, but useful in the early stages of development)
#define PEGASUS_ENABLE_DETAILED_LOG                     (PEGASUS_ENABLE_LOG && PEGASUS_DEBUG)

// Enable the assertion tests
#define PEGASUS_ENABLE_ASSERT                           (PEGASUS_DEBUG || PEGASUS_OPT)

// Enable the detailed assertion tests
// (performance issue, but useful in the early stages of development)
// (used only when PEGASUS_ENABLE_ASSERT is defined)
#define PEGASUS_ENABLE_DETAILED_ASSERT                  (PEGASUS_ENABLE_ASSERT && PEGASUS_DEBUG)

// Use GLEW (GL Extension Wrangler) as embedded source code rather than a dynamic library
#define PEGASUS_EMBED_GLEW                              (PEGASUS_REL)

// Flag for the use of proxy objects
#define PEGASUS_ENABLE_PROXIES                          (PEGASUS_DEV)

// Enable native application file opening, closing (or c runtime implementation)
// See the implementation of the functions living in the Pegasus::Io namespace for more details
#define PEGASUS_USE_NATIVE_IO_CALLS                     1

//Enable events only if dev mode. In rel mode the boiler plate code gets removed
//Events are used to communicate information to an editor app.
#define PEGASUS_USE_EVENTS                        (PEGASUS_DEV)

#define PEGASUS_SHADER_USE_SHADER_EVENTS                (PEGASUS_DEV)

//Enable asset category tracking for editor purposes.
#define PEGASUS_ASSETLIB_ENABLE_CATEGORIES (PEGASUS_DEV)

//Enables error checkin on functions in blockscript that are called out of context
#define PEGASUS_ENABLE_SCRIPT_PERMISSIONS (PEGASUS_DEV)

// Enable GPU data RunTime Type Information, for safe casting (asserts thrown if wrong type being casted)
#define PEGASUS_GRAPH_GPUDATA_RTTI                      (PEGASUS_DEBUG)

// Enable to stream the music file from disk rather than preloading it
#define PEGASUS_SOUND_STREAM_MUSIC                      0

// Enable editor reflection. If this is enabled, Pegasus will gather library data
// from the runtime libraries registered in BlockScript. This info will be exposed to the editor
// so the user has easy documentation access to the available BlockScript functions
#define PEGASUS_ENABLE_BS_REFLECTION_INFO               (PEGASUS_ENABLE_PROXIES)

//! Enable size checks in the property grid accessors
#define PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR       (PEGASUS_DEBUG)

//! Maximum total amount of world windows in dev mode.
//! A world window is a window that has support for demo visualization and that renders what the timeline showcases.
#if PEGASUS_DEV
#define PEGASUS_MAX_WORLD_WINDOW_COUNT 6
#else
#define PEGASUS_MAX_WORLD_WINDOW_COUNT 1
#endif

// Enable blockscript safe mode, where invalid memory access will get reported, at the cost of performance.
#define BLOCKSCRIPT_SAFEMODE PEGASUS_DEV

#endif  // PEGASUS_PREPROCESSOR_H
