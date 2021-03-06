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

};

}
}

#endif
