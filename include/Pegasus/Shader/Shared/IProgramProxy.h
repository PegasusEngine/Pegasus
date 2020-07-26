/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IProgramProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr
#ifndef PEGASUS_IPROGRAMPROXY_H
#define PEGASUS_IPROGRAMPROXY_H

#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

namespace Pegasus
{

namespace Graph
{
    class IGraphUserData;
}

namespace Shader
{

//! shader program proxy interface
class IProgramProxy : public Pegasus::Core::IBasicSourceProxy
{
public:
    //! Destructor
    virtual ~IProgramProxy(){}

    //! Gets the shader name
    //! \return gets the shader name
    virtual const char * GetName() const = 0;

    //! Sets the user data for this particular program
    //! \param userData. the user data to retrieve
    virtual void SetUserData(Pegasus::Core::IEventUserData * userData) = 0;

    virtual void SetSourceCode(Pegasus::Core::ISourceCodeProxy* sourceCode) = 0;
    virtual Pegasus::Core::ISourceCodeProxy* GetSourceCode() const = 0;

    //! Gets the user data for this particular program 
    //! \return user data reference
    virtual Pegasus::Core::IEventUserData * GetUserData() const = 0;
    
};


}
}
#endif


