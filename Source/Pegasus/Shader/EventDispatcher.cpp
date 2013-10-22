/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	EventDispatcher.cpp
//! \author	Kleber Garcia
//! \date	16th October 2013
//! \brief	Event dispatcher class

#include "Pegasus/Shader/Shared/IEventListener.h"
#include "Pegasus/Shader/EventDispatcher.h"
#include "Pegasus/Shader/Shared/ShaderEvent.h"


Pegasus::Shader::EventDispatcher::EventDispatcher()
 : mEventListener(nullptr), mUserData(nullptr)
{
}

Pegasus::Shader::EventDispatcher::~EventDispatcher()
{
}

void Pegasus::Shader::EventDispatcher::DispatchEvent(Pegasus::Shader::Event& event)
{
    if (mEventListener != nullptr)
    {
        //use visitor pattern to pick the event to dispatch
        event.Visit(GetUserData(), mEventListener);
    }
}
