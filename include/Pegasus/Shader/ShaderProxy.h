/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr

#ifndef PEGASUS_SHADERPROXY_H
#define PEGASUS_SHADERPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/Shared/IShaderProxy.h"

namespace Pegasus
{
namespace Shader
{

class ShaderStage;
class IUserData;

//! Shader proxy implementation
class ShaderProxy : public IShaderProxy
{
public:
    explicit ShaderProxy(ShaderStage * object);
    virtual ~ShaderProxy(){}

    //! \return gets the shader type
    virtual ShaderType GetStageType() const;

    //! \return gets the name of this shader
    virtual const char * GetName() const;

    //! Extracts the source of this shader.
    //! \param outSrc pointer to a string. The shader source string pointer is passed.
    //!        this string is not null terminated
    //! \param outSize the size of the string being outputed
    void GetSource(const char ** outSrc, int& outSize) const;

    //! Sets the source and marks shader as dirty
    //! \param source string. Doesn't need to be null terminated
    //! \param source size to copy 
    virtual void SetSource(const char * source, int sourceSize);
    
    //! Sets the user data for this particular shader
    //! \param userData. the user data to retrieve
    virtual void SetUserData(Pegasus::Graph::IGraphUserData * userData);

    //! Gets the user data for this particular shader
    //! \return user data reference
    virtual Pegasus::Graph::IGraphUserData * GetUserData() const;

    //! returns a 64 bit guid for a particular shader.
    //! \return the guid must correspond to a single shaderstage instance, on this
    //!         case is the address of the shader stage object, which should be unique
    virtual long long GetGuid() const { return reinterpret_cast<long long>(mObject); }

private:

    //! internal implementation
    ShaderStage * mObject;
};

}
}

#endif //Proxies
#endif //ifdef guard
