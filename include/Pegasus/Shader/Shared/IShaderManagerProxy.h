/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IShaderManagerProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr

#ifndef PEGASUS_SHADERMGR_PROXY_H
#define PEGASUS_SHADERMGR_PROXY_H

#include "Pegasus/Core/Shared/ISourceCodeManagerProxy.h"


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
class IShaderProxy;

class IShaderManagerProxy : public Pegasus::Core::ISourceCodeManagerProxy
{

public:
    //! Destructor
    virtual ~IShaderManagerProxy() {};

    //! Returns the number of programs existant on the application
    //! \return program count in the application
    virtual int GetProgramCount() const = 0;

    //! Gets the program based on its id. The id range goes from 0 to ProgramCount
    //! \return program proxy
    virtual IProgramProxy* GetProgram(int id) = 0;

    //! Gets the number of shaders existant on the application
    //! \return shader count in the application
    virtual int GetShaderCount() const = 0;
    
    //! Gets the shader based on it's id. The id range goes from 0 to GetShaderCount()
    //! \param id index position of shader proxy to get
    //! \return shader proxy
    virtual IShaderProxy* GetShader(int id) = 0; 

};

}
}

#endif
