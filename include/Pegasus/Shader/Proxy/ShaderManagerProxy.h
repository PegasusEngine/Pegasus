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
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/ShaderTracker.h"
#include "Pegasus/Shader/Proxy/ProgramProxy.h"
#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Shader/ProgramLinkage.h"

namespace Pegasus {
    namespace Core {
        namespace CompilerEvents {
            class ICompilerEventListener;
        }
    }

    namespace AssetLib {
        class IAssetProxy;
    }
}

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
    virtual void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener);

    //! number of shaders is the number of sources
    virtual int GetSourceCount() const { return GetShaderCount(); }

    //! returns the respective shader
    virtual Pegasus::Core::ISourceCodeProxy* GetSource(int id) { return GetShader(id); }

private:

    //! reference to internal program
    ShaderManager * mObject;
};
}
}

#endif // Proxy
#endif // Include guard
