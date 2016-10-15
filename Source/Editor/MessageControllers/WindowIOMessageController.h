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
    
    //! Window IO Message Controller
    class Message
    {
    public:
        enum MessageType
        {
            INVALID = -1,
            INITIALIZE_WINDOW,    
            DRAW_WINDOW,    
            WINDOW_RESIZED,
            ENABLE_DRAW
        };

        //! Constructor
        Message() : mMessageType(INVALID),
                    mViewportWidget(nullptr),
                    mComponentFlags(Pegasus::App::COMPONENT_FLAG_UNUSED_SLOT),
                    mWidth(1),
                    mHeight(1),
                    mEnableDraw(false)
        {} 

        ~Message() {}
        
        //! Getters
        MessageType GetMessageType() const { return mMessageType; }
        ViewportWidget* GetViewportWidget() const { return mViewportWidget; }
        Pegasus::App::ComponentTypeFlags GetComponentFlags() const { return mComponentFlags; }
        unsigned GetWidth() const { return mWidth; }
        unsigned GetHeight() const { return mHeight; }
        bool GetEnableDraw() const { return mEnableDraw; }

        //! Setters
        void SetMessageType(MessageType type) { mMessageType = type; }
        void SetViewportWidget(ViewportWidget* vp) { mViewportWidget = vp; }
        void SetComponentFlags(Pegasus::App::ComponentTypeFlags flags)  { mComponentFlags = flags; }
        void SetWidth(unsigned w)  { mWidth = w; }
        void SetHeight(unsigned h) { mHeight = h; }
        void SetEnableDraw(bool enableDraw) { mEnableDraw = enableDraw; }

    private:
        MessageType     mMessageType;
        ViewportWidget* mViewportWidget;
        Pegasus::App::ComponentTypeFlags mComponentFlags;
        bool mEnableDraw;
        unsigned mWidth;
        unsigned mHeight;
        
    };

    //! Constructor
    explicit WindowIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor 
    virtual ~WindowIOMessageController();

    //! Called by the render thread when we open a message
    //! \param msg the message to process and translate.
    void OnRenderThreadProcessMessage(const Message& msg);

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


