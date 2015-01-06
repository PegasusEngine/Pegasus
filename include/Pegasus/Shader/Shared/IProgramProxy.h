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

namespace Pegasus
{

namespace Graph
{
    class IGraphUserData;
}

namespace Shader
{

class IShaderProxy;

//! shader program proxy interface
class IProgramProxy : public Pegasus::Core::IBasicSourceProxy
{
public:
    //! Destructor
    virtual ~IProgramProxy(){}

    //! Gets the count of shaders this program has linked to itself
    //! \return count of shaders it has
    virtual int GetShaderCount() const = 0; 

    //! Returns shader proxy item, the i is the id going from 0 to ShaderCount
    //! \param i index position of shader requested
    //! \return shader proxy item
    virtual IShaderProxy * GetShader(unsigned i) = 0;

    //! Gets the shader name
    //! \return gets the shader name
    virtual const char * GetName() const = 0;

    //! Sets the user data for this particular program
    //! \param userData. the user data to retrieve
    virtual void SetUserData(Pegasus::Graph::IGraphUserData * userData) = 0;

    //! Gets the user data for this particular program 
    //! \return user data reference
    virtual Pegasus::Graph::IGraphUserData * GetUserData() const = 0;
    
};


}
}
#endif


