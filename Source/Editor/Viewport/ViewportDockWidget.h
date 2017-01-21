/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportDockWidget.h
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the application viewports

#ifndef EDITOR_VIEWPORTDOCKWIDGET_H
#define EDITOR_VIEWPORTDOCKWIDGET_H

#include "Widgets/PegasusDockWidget.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"

#include <QDockWidget>
#include <QVariant>
#include <QThread>
#include <QSemaphore>

class ViewportDockWidget;
class ViewportWidget;
class ViewportUpdaterInterface;
class ViewportUpdaterThread;

class Editor;
class QToolBar;
class QPushButton;
class QSignalMapper;

//! Helper class to handle threading updates for spin rendering.
//! Spin rendering: calling Render() indefinitely wihtout calling Update, for a specific window
class ViewportUpdaterThread : public QThread
{
     Q_OBJECT
public:
    ViewportUpdaterThread(ViewportDockWidget* parent);

    virtual ~ViewportUpdaterThread();

    virtual void run();

private:
    QSemaphore mInitializeSemaphore;
    ViewportDockWidget* mDockWidget;
    ViewportUpdaterInterface* mInterface;
};

//! Helper that executes events on a per frame basis for the viewport
//! This object lives in the ViewportUpdaterThread.
class ViewportUpdaterInterface : public QObject
{
     Q_OBJECT
public:
    ViewportUpdaterInterface(ViewportDockWidget* parent);

signals:
    void RenderLoopSignal();
    void DrawPegasusWindowSignal();

private slots:
    void ToggleSpinRenderSlot(bool enabled);
    void RenderOnceSlot();
    void RenderLoopSlot();

private:
    bool mIsSpinning;
    ViewportDockWidget* mDockWidget;
};

//! Dock widget containing the viewports
class ViewportDockWidget : public PegasusDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent editor
    //! \param parent Parent of the dock widget
    //! \param title of this widget
    //! \param object name of this widget
    ViewportDockWidget(
        Editor* editor, 
        QWidget * parent, 
        const char* title, 
        const char* objName);

    //! Destructor
    virtual ~ViewportDockWidget();

    //! Get the viewport widget associated with the dock widget
    //! \return Viewport widget associated with the dock widget
    inline ViewportWidget * GetViewportWidget() const { return mViewportWidget; }

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi();

    //! Returns the name this widget
    virtual const char* GetName() const { return mObjName; }

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return mTitle; }

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

    //! Retrieves a list of the button statuses, in a qvariant, for storage in the qt settings.
    void GetButtonStatuses(QVariantList& outputList);

    //! Sets a list of booleans (if match the size) into the button states of the viewport controls.
    void SetButtonStatuses(const QVariantList& inputList);


signals:
    //Signals used on the ViewportUpdaterInterface
    void ToggleSpinRender(bool enabled);

    //! signal sent to the update thread
    void RenderOnce();


private slots:
    void OnSetButtonProperty(int buttonIdx);

    void OnWindowProxyReady();
    
    void OnToggleSpinRender(bool enabled);

    void OnRenderOnceButton();

    void ReceiveDrawRequest();

private:

    //! Reads ui state and sets the appropiate events to the updater thread.
    void FlushUiStateToThreadUpdater ();

    //! Create the menu of the dock widget
    //! \param mainWidget Main widget of the dock, covering the entire child area
    //! \return Menu bar object to attach to the widget's layout
    QToolBar * CreateMenu(QWidget * mainWidget);
    void RegisterPropertyButton(QPushButton* button, const char* propertyName, Pegasus::App::ComponentType component);

    //! Viewport widget embedded in the dock widget
    ViewportWidget * mViewportWidget;
    //! Data for pegasus display functions
    const char* mTitle;
    const char* mObjName;
    
    //struct holding properties of bool buttons into the viewport
    struct PropertyButton
    {
        QPushButton* button;
        Pegasus::App::ComponentType component;
        const char* name;
    };

    QSignalMapper* mButtonMapper;
    QVector<PropertyButton> mButtons;

    //renders every frame
    bool mSpinRenderEnabled;

    //Thread that triggers a render spin on this viewport. Used for forcing draws on the current viewport.
    //This is not where the regular render functionality from playing the demo occurs.
    class ViewportUpdaterThread* mUpdaterThread;
};


#endif  // EDITOR_VIEWPORTDOCKWIDGET_H
