/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	WindowIOMessageController.h
//! \author	Kleber Garcia
//! \date	24th September 2015
//! \brief	Render side state manager for window operations.

#ifndef EDITOR_WINDOW_IO_CONTROLLER_H
#define EDITOR_WINDOW_IO_CONTROLLER_H

#include <QObject>
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "MessageControllers/MsgDefines.h"

//fwd declarations
class ViewportWidget;

namespace Pegasus
{
    namespace App 
    {
        class IApplicationProxy;
    }

    namespace Wnd
    {
        class IWindowProxy;
    }
}

//! Window IO Message controller
class WindowIOMessageController : public QObject
{
    Q_OBJECT;

public:
    
    //! Constructor
    explicit WindowIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor 
    virtual ~WindowIOMessageController();

    //! Called by the render thread when we open a message
    //! \param msg the message to process and translate.
    void OnRenderThreadProcessMessage(const WindowIOMCMessage& msg);

    //! Called on destruction of this controller.
    //! Must call after all viewports remove references to their windows.
    void DestroyWindows();

    static void RequestRedrawCallback(void* arg);

signals:
    void RedrawFrame();

private:

    void OnRenderInitializeWindow(ViewportWidget* window, Pegasus::App::ComponentTypeFlags componentFlags);

    void OnRenderDrawWindow(ViewportWidget* window);

    void OnRenderResizeWindow(ViewportWidget* window, unsigned width, unsigned height);

    void OnRenderEnableDraw(ViewportWidget* window, bool enable);

    Pegasus::App::IApplicationProxy*  mApplication;

    //! App window list used. Generally used on destruction.
    QList<Pegasus::Wnd::IWindowProxy*> mAppWindowsList;
    
};

#endif


