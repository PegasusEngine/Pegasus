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
};

}
}

#endif
