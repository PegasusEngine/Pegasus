/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Preprocessor.h
//! \author	Kevin Boulanger
//! \date	12th June 2013
//! \brief	Preprocessor file for the entire Pegasus engine.
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

// DLL mode for the engine, used when compiling the engine as a dynamic library
#ifdef _PEGASUS_DLL
#if PEGASUS_REL
#error Pegasus REL mode cannot be compiled as a DLL. This is reserved to DEV mode
#endif
#define PEGASUS_DLL             1
#else
#define PEGASUS_DLL             0
#endif

//----------------------------------------------------------------------------------------

// Flags telling if features are enabled based on the compilation profile.
// Use those rather than the ones above to help future maintenance.

// Enable logging in the console output
#define PEGASUS_ENABLE_LOG                  (PEGASUS_DEBUG || PEGASUS_OPT)

// Enable the assertion tests
#define PEGASUS_ENABLE_ASSERT               (PEGASUS_DEBUG || PEGASUS_OPT)


#endif  // PEGASUS_PREPROCESSOR_H