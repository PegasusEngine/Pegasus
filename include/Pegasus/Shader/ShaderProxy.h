/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr

#ifndef PEGASUS_SHADERPROXY_H
#define PEGASUS_SHADERPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/Shared/IShaderProxy.h"

namespace Pegasus
{
namespace Shader
{

class ShaderStage;

class ShaderProxy : public IShaderProxy
{
public:
    ShaderProxy();
    void Wrap(ShaderStage * stage) { mObject = stage; } 
    ShaderStage * Unwrap() { return mObject; }
    virtual ~ShaderProxy(){}
    virtual ShaderType GetStageType() const;
    virtual const char * GetName() const;

private:
    ShaderStage * mObject;
};

}
}

#endif //Proxies
#endif //ifdef guard
