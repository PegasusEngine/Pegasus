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

    //! Returns the number of sources this manager has
    //! \return source count in the application
    virtual int GetSourceCount() const = 0;

    //! Gets the program based on its id. The id range goes from 0 to ProgramCount
    //! \return program proxy
    virtual ISourceCodeProxy* GetSource(int id) = 0;

    //! Sets the event listener to be used for the shader
    //! \param event listener reference
    virtual void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener) = 0;
};

}
}

#endif
