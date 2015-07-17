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

    //! Loads a shader from an asset. Keeps a reference to it to prevent deletion.
    //! \param the asset to use to load this proxy
    //! \param sets parameter to true if the shader was already open. False otherwise
    //! \return the shader proxy to load
    virtual IShaderProxy* OpenShader(AssetLib::IAssetProxy* asset, bool* wasAlreadyOpen = nullptr) = 0;

    //! Closes a shader.
    //! \param the shader proxy to close
    virtual void CloseShader(IShaderProxy* shader) = 0;

    //! Loads a program from an asset proxy
    //! \param the program proxy to use to load this program
    //! \return the program proxy to use
    virtual IProgramProxy* OpenProgram(AssetLib::IAssetProxy* asset) = 0;

    //! creates a blank new program using a respective path as its file to be saved as
    //! \param the path of the program to use.
    //! \param the actual program name to use.
    virtual IProgramProxy* CreateNewProgram(const char* programName) = 0;

    //! Closes a program.
    //! \param the program proxy to close
    virtual void CloseProgram(IProgramProxy* program) = 0;

    //! returns true if the asset passed is an actual shader
    //! \param asset the asset to test if its a shader
    //! \return  true if this asset is a shader, false otherwise
    virtual bool IsShader(const AssetLib::IAssetProxy* asset) const = 0; 

    //! returns true if this asset is a program.
    //! \param asset the asset to test if its a program
    //! \return true if this is a program, false otherwise
    virtual bool IsProgram(const AssetLib::IAssetProxy* asset) const = 0;
};

}
}

#endif
