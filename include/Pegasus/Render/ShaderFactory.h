/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderFactory.h
//! \author Kleber Garcia
//! \date   24rd March 2014
//! \brief  Factory creator functions. These function pointers will return factories for
//!         the node classes to be used in the conversion of gpu to cpu data
#ifndef PEGASUS_RENDER_SHADER_FACTORY_H
#define PEGASUS_RENDER_SHADER_FACTORY_H

#include "Pegasus/Shader/IShaderFactory.h"

namespace Pegasus {
namespace Render {

//! \return returns a shader factory implementation.
//!         this implementation needs to be passed to the ShaderManager
Pegasus::Shader::IShaderFactory * GetRenderShaderFactory();

}
}

#endif
