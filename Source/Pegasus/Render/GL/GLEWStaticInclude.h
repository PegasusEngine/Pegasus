/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLEWStaticInclude.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Static include header for GLEW.

#ifndef PEGASUS_RENDER_GLEWSTATICINCLUDE_H
#define PEGASUS_RENDER_GLEWSTATICINCLUDE_H

// Use the GLEW static mode to avoid exporting symbols as for a dynamic library
#define GLEW_STATIC 1
#include "Pegasus/Libs/GLEW/glew.h"

#if PEGASUS_PLATFORM_WINDOWS
#include "Pegasus/Libs/GLEW/wglew.h"
#endif

#endif  // PEGASUS_RENDER_GLEWSTATICINCLUDE_H
