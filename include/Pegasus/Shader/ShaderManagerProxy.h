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
class IEventListener;
class ShaderManager;

//! Implementation of IShaderManagerProxy and wrapper proxy to expose to the editor
class ShaderManagerProxy : public IShaderManagerProxy
{
public:
    ShaderManagerProxy(ShaderManager * object);
    virtual ~ShaderManagerProxy(){}

    //! \return gets the number of programs in the application
    virtual int GetProgramCount() const;
    
    //! \param i index of the program requested
    //! \return gets a reference to a program
    virtual IProgramProxy* GetProgram(int i);

    //! \return total number of shaders in the application
    virtual int GetShaderCount() const;

    //! \param id  of the shader requested
    //! \return gets a reference to a shader
    virtual IShaderProxy* GetShader(int id); 

    //! Sets the event listener to be used for the shader
    //! \param event listener reference
    virtual void RegisterEventListener(IShaderEventListener * eventListener);

    //! Updates all the programs, only those which changed
    virtual void UpdateAllPrograms();

private:

    //! reference to internal program
    ShaderManager * mObject;

    //! pool of memory of shader proxies
    ShaderProxy  mShaderProxyPool [ShaderTracker::MAX_SHADER_CONTAINER];

    //! pool of memory of program proxies
    ProgramProxy mProgramProxyPool [ShaderTracker::MAX_SHADER_CONTAINER];
};
}
}

#endif // Proxy
#endif // Include guard
