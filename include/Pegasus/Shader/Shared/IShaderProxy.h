/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IShaderProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr
#ifndef PEGASUS_ISHADERPROXY_H
#define PEGASUS_ISHADERPROXY_H

#include "Pegasus/Shader/Shared/ShaderDefs.h"

namespace Pegasus
{
namespace Graph
{
    class IGraphUserData;
}
namespace Shader
{

class IShaderProxy
{
    public:
        //! Destructor
        virtual ~IShaderProxy(){}

        //! Gets the shader stage type
        //! \return returs the enum for the type of this shader
        virtual ShaderType GetStageType() const = 0;
    
        //! Gets the name of the shader
        //! \return the name given to the shader (the filename) as null terminated string
        virtual const char * GetName() const = 0;

        //! Gets the source of the shader
        //! \param outSrc output param to be filled with a string pointer containing the src
        //! \param outSize output param to an int, to be filled with the size of outSize
        virtual void GetSource(const char ** outSrc, int& outSize) const = 0;

        //! Sets the source and marks shader as dirty
        //! \param source string. Doesn't need to be null terminated
        //! \param source size to copy 
        virtual void SetSource(const char * source, int sourceSize) = 0;

        //! Sets the user data for this particular shader
        //! \param userData. the user data to retrieve
        virtual void SetUserData(Pegasus::Graph::IGraphUserData * userData) = 0;

        //! Gets the user data for this particular shader
        //! \return user data reference
        virtual Pegasus::Graph::IGraphUserData * GetUserData() const = 0;

        //! returns a 64 bit guid for a particular shader.
        //! \return the guid must correspond to a single shaderstage instance
        virtual long long GetGuid() const = 0;
};

}
}

#endif
