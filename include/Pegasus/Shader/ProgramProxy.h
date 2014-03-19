/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ProgramProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr

#ifndef PEGASUS_PROGRAMPROXY_H
#define PEGASUS_PROGRAMPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/ShaderProxy.h"

namespace Pegasus
{
namespace Shader
{
class ProgramLinkage;

class ProgramProxy : public IProgramProxy
{
public:
    ProgramProxy(){}
    virtual ~ProgramProxy() {}
    virtual int GetShaderCount() const; 
    virtual IShaderProxy * GetShader(unsigned i);
    virtual const char * GetName() const;
    void Wrap(ProgramLinkage * object) { mObject = object; }
    ProgramLinkage * Unwrap() const { return mObject; }

private:
    ProgramLinkage * mObject;
    ShaderProxy mShaderProxyPool[SHADER_STAGES_COUNT];
    
};

}
}
#endif // Proxies 
#endif // ifdef guard
