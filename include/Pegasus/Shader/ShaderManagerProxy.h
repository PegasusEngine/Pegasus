/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderManagerProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr

#ifndef PEGASUS_SHADERMANAGERPROXY_H
#define PEGASUS_SHADERMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/ShaderTracker.h"
#include "Pegasus/Shader/ProgramProxy.h"
#include "Pegasus/Shader/ShaderManager.h"

namespace Pegasus
{
namespace Shader
{

class IProgramProxy;
class ShaderManager;

//! Implementation of IShaderManagerProxy and wrapper proxy to expose to the editor
class ShaderManagerProxy : public IShaderManagerProxy
{
public:
    ShaderManagerProxy(ShaderManager * object);
    virtual ~ShaderManagerProxy(){}
    virtual int GetProgramCount() const;
    virtual IProgramProxy* GetProgram(int i);
    virtual int GetShaderCount() const;
    virtual IShaderProxy* GetShader(int id); 

private:
    ShaderManager * mObject;
    ShaderProxy  mShaderProxyPool [ShaderTracker::MAX_SHADER_CONTAINER];
    ProgramProxy mProgramProxyPool [ShaderTracker::MAX_SHADER_CONTAINER];
};
}
}

#endif // Proxy
#endif // Include guard
