#include "MessageControllers/WindowIOMessageController.h"
#include "Viewport/ViewportWidget.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"

WindowIOMessageController::WindowIOMessageController(Pegasus::App::IApplicationProxy* app) :
    mApplication(app)
{
}

WindowIOMessageController::~WindowIOMessageController()
{
    DestroyWindows();
}

void WindowIOMessageController::OnRenderThreadProcessMessage(const WindowIOMCMessage& msg)
{
    switch (msg.GetMessageType())
    {
    case WindowIOMCMessage::INITIALIZE_WINDOW:
        OnRenderInitializeWindow(msg.GetViewportWidget(), msg.GetComponentFlags());
        break;
    case WindowIOMCMessage::DRAW_WINDOW:
        OnRenderDrawWindow(msg.GetViewportWidget());
        break;
    case WindowIOMCMessage::WINDOW_RESIZED:
        OnRenderResizeWindow(msg.GetViewportWidget(), msg.GetWidth(), msg.GetHeight());
        break;
    case WindowIOMCMessage::ENABLE_DRAW:
        OnRenderEnableDraw(msg.GetViewportWidget(), msg.GetEnableDraw());
        break;
    }
}

void WindowIOMessageController::OnRenderInitializeWindow(ViewportWidget* viewportWidget, Pegasus::App::ComponentTypeFlags componentFlags)
{
    ED_ASSERT(viewportWidget != nullptr);
    Pegasus::App::AppWindowConfig windowConfig;

    windowConfig.mComponentFlags = componentFlags;
    windowConfig.mIsChild = true;
    windowConfig.mParentWindowHandle = viewportWidget->GetWindowHandle();
    windowConfig.mWidth = viewportWidget->GetWidth();
    windowConfig.mHeight = viewportWidget->GetHeight();
    windowConfig.mRedrawEditorCallback = WindowIOMessageController::RequestRedrawCallback;
    windowConfig.mRedrawArgCallback = static_cast<void*>(this);
    Pegasus::Wnd::IWindowProxy* wnd = mApplication->AttachWindow(windowConfig);
    wnd->EnableDraw(false); //disable draw until window is ready. Avoids OS calling draw on the window, unless we processed once already.
    viewportWidget->AttachWindowProxy(wnd);
    ED_ASSERT(wnd != nullptr);
    mAppWindowsList.append(wnd);
}

void WindowIOMessageController::OnRenderDrawWindow(ViewportWidget* window)
{
    ED_ASSERT(window->GetWindowProxy() != nullptr);
    window->GetWindowProxy()->Draw();
}

void WindowIOMessageController::OnRenderResizeWindow(ViewportWidget* window, unsigned width, unsigned height)
{
    ED_ASSERT(window->GetWindowProxy() != nullptr);
    window->GetWindowProxy()->Resize(width, height);
}

void WindowIOMessageController::OnRenderEnableDraw(ViewportWidget* window, bool enable)
{
    ED_ASSERT(window->GetWindowProxy() != nullptr);
    window->GetWindowProxy()->EnableDraw(enable);
}

void WindowIOMessageController::DestroyWindows()
{
    for (int i = 0; i < mAppWindowsList.count(); ++i)
    {
        mApplication->DetachWindow(mAppWindowsList[i]);
    }

    mAppWindowsList.clear();
}

void WindowIOMessageController::RequestRedrawCallback(void* arg)
{
    WindowIOMessageController* ctroller = static_cast<WindowIOMessageController*>(arg);
    emit ctroller->RedrawFrame();
}

