/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ApplicationInterface.h
//! \author	Kevin Boulanger
//! \date	20th October 2013
//! \brief	Interface object used to interface with the Pegasus related messages,
//!         created in the application thread

#ifndef EDITOR_APPLICATIONINTERFACE_H
#define EDITOR_APPLICATIONINTERFACE_H

class QTimer;

namespace Pegasus {
    namespace Application {
        class IApplicationProxy;
    }
    namespace Window {
        class IWindowProxy;
    }
}


//! Interface object used to interface with the Pegasus related messages,
//! created in the application thread
class ApplicationInterface : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param application Pegasus application proxy (!= nullptr)
    //! \param appWindow Pegasus application window proxy (!= nullptr)
    //! \param parent Parent Qt object
    ApplicationInterface(Pegasus::Application::IApplicationProxy * application,
                         Pegasus::Window::IWindowProxy * appWindow,
                         QObject *parent = 0);

    //! Destructor, closes the running application
    virtual ~ApplicationInterface();


    //! Set the state indicating if an assertion error is currently being handled
    //! \param state True if an assertion is currently being handled
    //! \todo Seems not useful anymore. Test and remove if possible
    //inline void SetAssertionBeingHandled(bool state) { mAssertionBeingHandled = state; }

    //------------------------------------------------------------------------------------

public slots:

    //! Start the window redrawing timer
    void StartRedrawTimer();

    //! Stop the window redrawing 
    void StopRedrawTimer();

    //----------------------------------------------------------------------------------------
    
private slots:

    //! Called when the viewport needs to be resized
    //! \param width New width of the viewport, in pixels
    //! \param height New height of the viewport, in pixels
    void ResizeViewport(int width, int height);

    //! Called when a request to redraw the content of the application windows is sent
    //! \todo Temporary, we need a better way to draw, on a per-window basis
    void RedrawChildWindows();

    //------------------------------------------------------------------------------------

private:

    //! Timer used to forcefully render the content of the app windows
    QTimer * mTimer;

    //! Application proxy object created when running the application DLL
    Pegasus::Application::IApplicationProxy * mApplication;

    //! Window used to the render the viewport of the application
    //! \todo Handle multiple windows
    Pegasus::Window::IWindowProxy * mAppWindow;

    //! True while an assertion dialog box is shown to prevent any paint message to reach the application windows
    //! \todo Seems not useful anymore. Test and remove if possible
    //bool mAssertionBeingHandled;
};


#endif  // EDITOR_APPLICATIONINTERFACE_H
