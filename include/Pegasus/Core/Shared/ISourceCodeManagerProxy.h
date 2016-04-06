/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ISourceCodeManagerProxy.h
//! \author Kleber Garcia 
//! \date   November 1st 2014 
//! \brief  Proxy interface, used to manage a list of source files and add listener to the
//!         corresponding compiler

#ifndef PEGASUS_SOURCE_CODE_MGR_I_H
#define PEGASUS_SOURCE_CODE_MGR_I_H

namespace Pegasus
{
namespace Core
{

    namespace CompilerEvents
    {
        class ICompilerEventListener;
    }


class ISourceCodeProxy;

//! manager interface for source code proxies
class ISourceCodeManagerProxy
{
public:
    //! Constructor
    ISourceCodeManagerProxy() {};

    //! Destructor
    virtual ~ISourceCodeManagerProxy() {};

    //! Sets the event listener to be used for the shader
    //! \param event listener reference
    virtual void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener) = 0;
};

}
}

#endif
