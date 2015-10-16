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

void WindowIOMessageController::OnRenderThreadProcessMessage(const WindowIOMessageController::Message& msg)
{
    switch (msg.GetMessageType())
    {
    case WindowIOMessageController::Message::INITIALIZE_WINDOW:
        OnRenderInitializeWindow(msg.GetViewportWidget(), msg.GetComponentFlags());
        break;
    case WindowIOMessageController::Message::DRAW_WINDOW:
        OnRenderDrawWindow(msg.GetViewportWidget());
        break;
    case WindowIOMessageController::Message::WINDOW_RESIZED:
        OnRenderResizeWindow(msg.GetViewportWidget(), msg.GetWidth(), msg.GetHeight());
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
    Pegasus::Wnd::IWindowProxy* wnd = mApplication->AttachWindow(windowConfig);
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

void WindowIOMessageController::DestroyWindows()
{
    for (int i = 0; i < mAppWindowsList.count(); ++i)
    {
        mApplication->DetachWindow(mAppWindowsList[i]);
    }

    mAppWindowsList.clear();
}

