/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowProxy.cpp
//! \author David Worsham
//! \date   06th October 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app window.

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus\Window\WindowProxy.h"
#include "Pegasus\Window\Window.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridObjectProxy.h"
#include "Pegasus/Window/WindowComponentState.h"

namespace Pegasus {
namespace Wnd {

WindowProxy::WindowProxy(Window* wnd)
    : mObject(wnd)
{
}

//----------------------------------------------------------------------------------------

WindowProxy::~WindowProxy()
{
}

//----------------------------------------------------------------------------------------

void WindowProxy::Draw()
{
    mObject->Draw();
}

//----------------------------------------------------------------------------------------

void WindowProxy::Resize(unsigned int width, unsigned int height)
{
    mObject->Resize(width, height);
}

//----------------------------------------------------------------------------------------

Window* WindowProxy::Unwrap() const
{
    return mObject;
}

Pegasus::PropertyGrid::IPropertyGridObjectProxy* WindowProxy::GetComponentState(Pegasus::App::ComponentType type)
{
    if (mObject != nullptr)
    {
        const Utils::Vector<Window::StateComponentPair>& components = mObject->GetComponents();
        for (unsigned i = 0; i < components.GetSize(); ++i)
        {
            const Window::StateComponentPair& pair = components[i];
            if (pair.mComponent->GetUniqueId() == type && pair.mState != nullptr)
            {
                return pair.mState->GetPropertyGridProxy();
            }
        }
    }
    return nullptr;
}

void WindowProxy::EnableDraw(bool enabled)
{
    mObject->EnableDraw(enabled);
}

void WindowProxy::HandleKeyEvent(Keys k, bool isDown)
{
    mObject->OnKeyEvent(k, isDown); 
}

}   // namespace Wnd
}   // namespace Pegasus

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif  // PEGASUS_ENABLE_PROXIES
