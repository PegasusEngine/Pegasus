/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.cpp
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Class for a single window in a Pegasus application.

#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Window/IWindowContext.h"
#include "../Source/Pegasus/Window/IWindowImpl.h"

namespace Pegasus {
namespace Window {

//! Message handler object for a Window
class WindowMessageHandler : public IWindowMessageHandler
{
public:
    WindowMessageHandler(bool useBasicContext, Window* parent);
    virtual ~WindowMessageHandler();

    // IWindowMessageHandler interface
    virtual void OnCreate(Render::DeviceContextHandle handle);
    virtual void OnDestroy();
    virtual void OnRepaint();
    virtual void OnResize(unsigned int width, unsigned int height);

private:
    // No copies allowed
    PG_DISABLE_COPY(WindowMessageHandler);

    bool mUseBasicContext;
    Window* mParent;
};

//----------------------------------------------------------------------------------------

WindowMessageHandler::WindowMessageHandler(bool useBasicContext, Window* parent)
    : mUseBasicContext(useBasicContext), mParent(parent)
{
}

//----------------------------------------------------------------------------------------

WindowMessageHandler::~WindowMessageHandler()
{
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnCreate(Render::DeviceContextHandle handle)
{
    Render::ContextConfig contextConfig;

    // Create context
    contextConfig.mDeviceContextHandle = handle;
    contextConfig.mStartupContext = mUseBasicContext;
    mParent->mRenderContext = PG_NEW("Render::Context", Pegasus::Memory::PG_MEM_PERM) Render::Context(contextConfig);
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnDestroy()
{
     // Destroy context
    PG_DELETE mParent->mRenderContext;
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnRepaint()
{
    mParent->Refresh();
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnResize(unsigned int width, unsigned int height)
{
    mParent->mWidth = width;
    mParent->mHeight = height;
}

//----------------------------------------------------------------------------------------

Window::Window(const WindowConfig& config)
:   mWindowContext(config.mWindowContext),
    mRenderContext(nullptr),
    mWidth(config.mWidth),
    mHeight(config.mHeight)
{
    // Create platform stuff
    mMessageHandler = PG_NEW("Window message handler", Pegasus::Memory::PG_MEM_PERM) WindowMessageHandler(config.mUseBasicContext, this);
    mPrivateImpl = IWindowImpl::CreateImpl(config, mMessageHandler);
}

//----------------------------------------------------------------------------------------

Window::~Window()
{
    // Destroy platform stuff
    IWindowImpl::DestroyImpl(mPrivateImpl);
    PG_DELETE mMessageHandler;
}

//----------------------------------------------------------------------------------------

WindowHandle Window::GetHandle() const
{
    return mPrivateImpl->GetHandle();
}

//----------------------------------------------------------------------------------------

void Window::Resize(unsigned int width, unsigned int height)
{
    mPrivateImpl->Resize(width, height);
    mWidth = width;
    mHeight = height;
}


}   // namespace Window
}   // namespace Pegasus
