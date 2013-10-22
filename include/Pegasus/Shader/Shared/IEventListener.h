/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IEventListener.h
//! \author	Kleber Garcia
//! \date	16th October 2013
//! \brief	event listener interface

#ifndef PEGASUS_EVENTLISTENER_H 
#define PEGASUS_EVENTLISTENER_H

//forward declarations
namespace Pegasus
{
    namespace Shader
    {
        class IUserData;
        class CompilationEvent;
        class LinkingEvent;
        class FileOperationEvent;
        class ShaderLoadedEvent;
        class CompilationNotification;
    }
}


namespace Pegasus
{
namespace Shader
{
    class IEventListener
    {
    public:
        IEventListener(){}
        virtual ~IEventListener(){}
        virtual void OnEvent(IUserData * userData, CompilationEvent& e) = 0;
        virtual void OnEvent(IUserData * userData, LinkingEvent& e) = 0;
        virtual void OnEvent(IUserData * userData, FileOperationEvent& e) = 0;
        virtual void OnEvent(IUserData * userData, ShaderLoadedEvent& e) = 0;
        virtual void OnEvent(IUserData * userData, CompilationNotification& e) = 0;
    };
}
}

#endif
