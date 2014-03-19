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


namespace Pegasus
{
namespace Shader
{

class IProgramProxy;

class IShaderManagerProxy
{

public:
    //! Destructor
    ~IShaderManagerProxy() {};
    virtual int GetProgramCount() const = 0;
    virtual IProgramProxy* GetProgram(int id) = 0;

};

}
}

#endif
