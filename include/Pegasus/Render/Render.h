/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Render.h
//! \author Kleber Garcia
//! \date   25th March of 2014
//! \brief  Master render library. PARR - Pegasus Abstract Render Recipes	
//!         This library encapsulates most of the underlying API and serves as a way 
//!         of accessing it. Provides render shortcuts.

#ifndef PARR_H
#define PARR_H

#include "Pegasus/Shader/ProgramLinkage.h"

namespace Pegasus
{

namespace Render
{
    void Dispatch (Shader::ProgramLinkageRef program);
}

}

#endif
