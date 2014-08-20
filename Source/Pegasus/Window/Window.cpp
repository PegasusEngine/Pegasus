/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.cpp
//! \author David Worsham
//! \date   04th July 2013
//! \brief  Class for a single window in a Pegasus application.

#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Window/IWindowContext.h"
#include "../Source/Pegasus/Window/IWindowImpl.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Window/WindowProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Wnd {

//! Message handler object for a Window
class WindowMessageHandler : public IWindowMessageHandler
{
public:
    explicit WindowMessageHandler(Window* parent);
    virtual ~WindowMessageHandler();

    // IWindowMessageHandler interface
    virtual void OnCreate(Os::WindowHandle handle);
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

WindowMessageHandler::WindowMessageHandler(Window* parent)
 : mParent(parent)
{
}

//----------------------------------------------------------------------------------------

WindowMessageHandler::~WindowMessageHandler()
{
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnCreate(Os::WindowHandle handle)
{
    Render::ContextConfig contextConfig;

    // Create context
    contextConfig.mAllocator = mParent->mRenderAllocator;
    contextConfig.mOwnerWindowHandle = handle;
    contextConfig.mDevice = mParent->GetRenderDevice();
    mParent->GetDimensions(contextConfig.mWidth, contextConfig.mHeight);
    mParent->mRenderContext = PG_NEW(mParent->mRenderAllocator, -1, "Render::Context", Pegasus::Alloc::PG_MEM_PERM) Render::Context(contextConfig);
    mParent->mContextCreated = true;
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnDestroy()
{
     // Destroy context
    PG_DELETE(mParent->mRenderAllocator, mParent->mRenderContext);
    mParent->mContextCreated = false;
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnRepaint()
{
    if (mParent->mContextCreated)
    {
        mParent->Refresh(true);
    }
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnResize(unsigned int width, unsigned int height)
{
    mParent->mWidth = width;
    mParent->mHeight = height;
    mParent->mRenderContext->Resize(width, height);
}

//----------------------------------------------------------------------------------------

Window::Window(const WindowConfig& config)
:   mAllocator(config.mAllocator),
    mRenderAllocator(config.mRenderAllocator),
    mDevice(config.mDevice),
    mWindowContext(config.mWindowContext),
    mRenderContext(nullptr),
    mContextCreated(false),
    mWidth(config.mWidth),
    mHeight(config.mHeight),
    mIsChild(config.mIsChild)
{
    // Create platform stuff
    mMessageHandler = PG_NEW(mAllocator, -1, "Window message handler", Pegasus::Alloc::PG_MEM_PERM) WindowMessageHandler(this);
    mPrivateImpl = IWindowImpl::CreateImpl(config, mAllocator, mMessageHandler);

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the timeline
    mProxy = PG_NEW(mAllocator, -1, "Window::mProxy", Pegasus::Alloc::PG_MEM_PERM) WindowProxy(this);
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Window::~Window()
{
    // Destroy platform stuff
    IWindowImpl::DestroyImpl(mPrivateImpl, mAllocator);
    PG_DELETE(mAllocator, mMessageHandler);

#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the window
    PG_DELETE(mAllocator, mProxy);
#endif
}

//----------------------------------------------------------------------------------------

Os::WindowHandle Window::GetHandle() const
{
    return mPrivateImpl->GetHandle();
}

//----------------------------------------------------------------------------------------

void Window::Refresh(bool updateTimeline)
{
    if (mRenderContext != nullptr)
    {
        mRenderContext->Bind();
        if (updateTimeline)
        {
            mWindowContext->GetTimeline()->Update();
        }
        Render();
    }
    else
    {
        PG_FAILSTR("Trying to refresh a window but its render context is undefined");
    }
}

//----------------------------------------------------------------------------------------

void Window::Resize(unsigned int width, unsigned int height)
{
    mPrivateImpl->Resize(width, height);
    mWidth = width;
    mHeight = height;
}

//----------------------------------------------------------------------------------------
void Window::HandleMainWindowEvents()
{
    if (!mIsChild)
    {
        mPrivateImpl->SetAsMainWindow();
    }
}

}   // namespace Wnd
}   // namespace Pegasus
