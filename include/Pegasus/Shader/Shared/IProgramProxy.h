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
    ~IProgramProxy(){}
    virtual int GetShaderCount() const = 0; 
    virtual IShaderProxy * GetShader(unsigned i) = 0;
    virtual const char * GetName() const = 0;
};


}
}
#endif


