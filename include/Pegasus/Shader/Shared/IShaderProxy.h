/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IShaderProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr
#ifndef PEGASUS_ISHADERPROXY_H
#define PEGASUS_ISHADERPROXY_H

#include "Pegasus/Shader/Shared/ShaderDefs.h"

namespace Pegasus
{
namespace Shader
{

class IShaderProxy
{
    public:
        ~IShaderProxy(){}
        virtual ShaderType GetStageType() const = 0;
        virtual const char * GetName() const = 0;
};

}
}

#endif
