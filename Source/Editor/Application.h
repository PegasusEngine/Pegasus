/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Application.h
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Worker thread to contain an application to run

#ifndef EDITOR_APPLICATION_H
#define EDITOR_APPLICATION_H

#include <QThread>

#include "Pegasus/Core/Log.h"
#include "Pegasus/Window/WindowDefs.h"

class ViewportWidget;

namespace Pegasus {
    namespace Application {
        class IApplicationProxy;
    }
    namespace Window {
        class IWindowProxy;
    }
}


//! Application worker thread
class Application : public QThread
{
    Q_OBJECT

public:

    //! Constructor, does not initialize the application, use \a Open() for that
    Application(QObject *parent = 0);

    //! Destructor, closes the running application
    virtual ~Application();


    //! Set the file name of the application to load
    //! \param fileName Path and name of the library file to open
    //! \todo Add error management
    void SetFile(const QString & fileName);

    //! Set the viewport parameters for the window associated with the application
    //! \param windowHandle Viewport window handle that will become the parent of the rendering window
    //! \param width Initial width of the viewport in pixels (> 0)
    //! \param height Initial height of the viewport in pixels (> 0)
    //! \todo Handle multiple viewports
    void SetViewportParameters(/**index,*/ Pegasus::Window::WindowHandle windowHandle,
                               int width, int height);


    //! Error codes for the application thread
    typedef enum Error
    {
        ERROR_INVALID_FILE_NAME = 0,    //!< Invalid file name (not properly formatted)
        ERROR_INVALID_APPLICATION,      //!< The library of the application does not have the correct entry points
        ERROR_INVALID_INTERFACE,        //!< The library interface does not have the right type
        ERROR_INVALID_VIEWPORT          //!< Invalid viewport given to the application
    };


    //! Running function of the thread, initializes the engine and the application,
    //! then runs the main loop
    //! \warning Do not rename to Run(), as this function is an overload of QThread
    void run();

    //------------------------------------------------------------------------------------
    
signals:

	//! Signal emitted when an error occurred when loading the application
    //! \param error Error code
	void LoadingError(Application::Error error);

    //! Signal emitted when the application has successfully loaded (not running)
    void LoadingSucceeded();

    //------------------------------------------------------------------------------------

private slots:
        
    //! Called when the viewport is resized
    //! \param width New width of the viewport, in pixels
    //! \param height New height of the viewport, in pixels
    void ViewportResized(int width, int height);

    //------------------------------------------------------------------------------------

private:

    //! Handler for log messages coming from the application itself
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log
    static void LogHandler(Pegasus::Core::LogChannel logChannel, const char * msgStr);

    //------------------------------------------------------------------------------------

private:

    //! File name of the application to load
    QString mFileName;

    //! Application proxy object created when running the application DLL
    Pegasus::Application::IApplicationProxy * mApplication;

    //! Window used to the render the viewport of the application
    //! \todo Handle multiple windows
    Pegasus::Window::IWindowProxy * mAppWindow;

    //! Viewport window handle associated with the application
    //! (will become the parent of the rendering window)
    Pegasus::Window::WindowHandle mViewportWindowHandle;

    //! Initial width of the viewport in pixels (> 0)
    int mViewportInitialWidth;

    //! Initial height of the viewport in pixels (> 0)
    int mViewportInitialHeight;
};


#endif  // EDITOR_APPLICATION_H
