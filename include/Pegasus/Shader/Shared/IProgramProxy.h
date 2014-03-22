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

namespace Pegasus
{
namespace Shader
{

class IShaderProxy;

class IProgramProxy
{
public:
    //! Destructor
    virtual ~IProgramProxy(){}

    //! Gets the count of shaders this program has linked to itself
    //! \return count of shaders it has
    virtual int GetShaderCount() const = 0; 

    //! Returns shader proxy item, the i is the id going from 0 to ShaderCount
    //! \return shader proxy item
    virtual IShaderProxy * GetShader(unsigned i) = 0;

    //! Gets the shader name
    //! \return gets the shader name
    virtual const char * GetName() const = 0;
};


}
}
#endif


