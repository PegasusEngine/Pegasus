/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportWidget.h
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Widget containing one application viewport

#ifndef EDITOR_VIEWPORTWIDGET_H
#define EDITOR_VIEWPORTWIDGET_H

#include "MessageControllers/MsgDefines.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"

#include <QWidget>

class QResizeEvent;

namespace Pegasus
{
    namespace Wnd
    {
        class IWindowProxy;
    }
}

//! Widget containing one application viewport
class ViewportWidget : public QWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent of the widget
    explicit ViewportWidget(QWidget * parent, Pegasus::App::ComponentTypeFlags componentFlags);

    //! Destructor
    virtual ~ViewportWidget();

    //! Get the platform-independent window handle of the widget
    //! \todo Do not use unsigned int, use a Pegasus type instead
    //! \return Platform-independent window handle of the widget
    inline unsigned int GetWindowHandle() const { return mWindowHandle; }

    //! Get the width of the viewport
    //! \return Current width of the viewport in pixels
    inline int GetWidth() const { return size().width(); }

    //! Get the height of the viewport
    //! \return Current height of the viewport in pixels
    inline int GetHeight() const { return size().height(); }

    //! Returns the internal pegasus window that this viewport is interacting with.
    //! \return mWindowProxy
    inline Pegasus::Wnd::IWindowProxy* GetWindowProxy() const { return mWindowProxy; }

    //! Sets a boolean property of a component
    void SetBoolProperty(Pegasus::App::ComponentType component, const char* name, bool value);

    //! Sets a float property of a component
    void SetFloatProperty(Pegasus::App::ComponentType component, const char* name, float value);

    //! Sets an int property of a component
    void SetIntProperty(Pegasus::App::ComponentType component, const char* name, int value);

    //! Callback when app is loaded.
    void OnAppLoaded();

    //! Callback when app is unloaded
    void OnAppUnloaded();

    //! Call only on the render thread!!
    void DrawPegasusWindow();

    //! Attaches a window proxy to this viewport
    //! \param window - the window to attach
    //! \note - this function has to be called from the render thread only.
    void AttachWindowProxy(Pegasus::Wnd::IWindowProxy* window); 

    void OnInitialized(const PropertyGridHandle& handle, const QString& title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy);

    void OnUpdated(const PropertyGridHandle& handle, const QVector<PropertyGridIOMCUpdateElement>& els);

    void OnShutdown(const PropertyGridHandle& handle);

    //------------------------------------------------------------------------------------
signals:

    //! Sends a message to the window controller
    void OnSendWindowIoMessage(WindowIOMCMessage msg);

    //! Sends a message to the property grid controller
    void OnSendPropertyGridIoMessage(PropertyGridIOMCMessage msg);

    //! Sends a message when this window is ready
    void OnWindowProxyReady();

    //------------------------------------------------------------------------------------

protected:

    //! Called when the viewport is resized, used to update the application-side viewport
    //! \param event Event object containing the new size of the viewport
    void resizeEvent(QResizeEvent * event);

    //------------------------------------------------------------------------------------
    
private:

    //! Observer of this widget, used to communicate with the IO controller.
    PropertyGridObserver* mObserver;

    //! Sets a boolean property of a component
    void InitUpdateElement(Pegasus::App::ComponentType component, const char* name, PropertyGridIOMCUpdateElement& outUpdateEl);
    void SendUpdateEl(Pegasus::App::ComponentType component, PropertyGridIOMCUpdateElement& el);

    PropertyGridHandle mComponentPropertyHandles[Pegasus::App::COMPONENT_COUNT]; 
    QMap<QString, int> mComponentPropertyLookups[Pegasus::App::COMPONENT_COUNT]; 

    //! The window proxy this window contains.
    Pegasus::Wnd::IWindowProxy* mWindowProxy;

    //! Native window ID, stored so it is accessible from other threads
    unsigned int mWindowHandle;

    //! The components that this viewport uses
    Pegasus::App::ComponentTypeFlags mComponentFlags;
};


#endif  // EDITOR_VIEWPORTWIDGET_H
