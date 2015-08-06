/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   Application.h
//! \author Karolyn Boulanger
//! \date   02nd July 2013
//! \brief  Worker thread to contain an application to run

#ifndef EDITOR_APPLICATION_H
#define EDITOR_APPLICATION_H

#include "Viewport/ViewportType.h"

#include <QThread>

#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Shared/AssertReturnCode.h"

class ApplicationInterface;
class ViewportWidget;

namespace Pegasus {
    namespace App {
        class IApplicationProxy;
    }
    namespace PropertyGrid {
        class IPropertyGridManagerProxy;
    }
    namespace Timeline {
        class ITimelineManagerProxy;
    }
    namespace Wnd {
        class IWindowProxy;
    }
    namespace Shader {
        class IShaderManagerProxy;
    }
    namespace Texture {
        class ITextureManagerProxy;
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
    void SetFileName(const QString & fileName);

    //! Get the file name of the loaded application
    //! \return Path and name of the opened library file
    inline const QString & GetFileName() const { return mFileName; }


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


    //! Get the application proxy object
    //! \return Pointer to the application proxy object created when running the application DLL
    inline Pegasus::App::IApplicationProxy * GetApplicationProxy() const { return mApplication; }

    //! Get the property grid manager proxy object
    //! \return Pointer to the property grid manager proxy object
    Pegasus::PropertyGrid::IPropertyGridManagerProxy * GetPropertyGridManagerProxy() const;

    //! Get the window used to render one of the viewports of the application
    //! \param viewportType Type of the viewport widget to get (VIEWPORTTYPE_xxx constant)
    //! \return Pointer to the window used to render the viewport of the application
    //!         (nullptr in case of error)
    Pegasus::Wnd::IWindowProxy * GetWindowProxy(ViewportType viewportType) const;

    //! Get the timeline proxy object
    //! \return Pointer to the timeline proxy object
    Pegasus::Timeline::ITimelineManagerProxy * GetTimelineProxy() const;

    //! Get the shader manager proxy object
    //! \return Pointer to the shader manager proxy object
    Pegasus::Shader::IShaderManagerProxy * GetShaderManagerProxy() const;

    //! Get the texture manager proxy object
    //! \return Pointer to the texture manager proxy object
    Pegasus::Texture::ITextureManagerProxy * GetTextureManagerProxy() const;

    //! Function called by the log handler, emitting the \a LogSentFromApplication signal
    //! \warning To be called in the application thread, not the editor thread
    //! \param logChannel Pegasus log channel
    //! \param msgStr Content of the log message
    void EmitLogFromApplication(Pegasus::Core::LogChannel logChannel, const QString & msgStr);

    //! Function called by the assertion handler, emitting the \a AssertionSentFromApplication signal
    //! \warning To be called in the application thread, not the editor thread
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand.
    //!               Empty string if no message is defined.
    //! \return Pegasus::Core::ASSERTION_xxx constant,
    //!         chosen from the button the user clicked on
    Pegasus::Core::AssertReturnCode EmitAssertionFromApplication(const QString & testStr,
                                                                 const QString & fileStr,
                                                                 int line,
                                                                 const QString & msgStr);

    void SetAsyncHandleDestruction(bool asyncHandleDestruction)
    {
        mAsyncHandleDestruction = asyncHandleDestruction;
    }

    //------------------------------------------------------------------------------------
    
signals:

	//! Signal emitted when an error occurred when loading the application
    //! \param error Error code
	void LoadingError(Application::Error error);

    //! Signal emitted when the application has successfully loaded (not running)
    void LoadingSucceeded();

    //! Signal emitted from the application thread when the application sends a log message
    //! \param logChannel Pegasus log channel
    //! \param msgStr Content of the log message
    void LogSentFromApplication(Pegasus::Core::LogChannel logChannel, const QString & msgStr);

    //! Signal emitted from the application thread when the application sends an assertion error
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand.
    //!               Empty string if no message is defined.
    void AssertionSentFromApplication(const QString & testStr, const QString & fileStr, int line, const QString & msgStr);

    //! Emitted when a frame is requested after the previous frame is done rendering
    //! while being in play mode
    void FrameRequestedInPlayMode();

    //! Emitted when unloading of an application has succeeded
    void ApplicationFinished();

    //------------------------------------------------------------------------------------

private slots:
        
    //! Called when a log message is received from the application thread
    //! through a queue connection
    //! \warning This is the editor thread function, which does the final post
    //!          into the console dock window
    //! \param logChannel Pegasus log channel
    //! \param msgStr Content of the log message
    void LogReceivedFromApplication(Pegasus::Core::LogChannel logChannel, const QString & msgStr);

    //! Called when an assertion error is received from the application thread
    //! through a blocking queue connection
    //! \warning This is the editor thread function, which shows the assertion dialog box
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand.
    //!               Empty string if no message is defined.
    void AssertionReceivedFromApplication(const QString & testStr, const QString & fileStr, int line, const QString & msgStr);

    //! Update the user interface after a current beat update, and if in play mode,
    //! request the drawing of a new frame
    //! \param beat Updated beat, can have fractional part
    void UpdateUIAndRequestFrameInPlayMode(float beat);

    //------------------------------------------------------------------------------------

private:

    //! Convert a viewport type to a window type string
    //! \param viewportType Type of the viewport (VIEWPORTTYPE_xxx constant)
    //! \return String of the window type, nullptr in case of error
    const char * GetWindowTypeFromViewportType(ViewportType viewportType) const;

    //! Handler for log messages coming from the application itself
    //! \warning This is a static function, so it cannot emit any signal.
    //!          We have to call a member function, \a EmitLogFromApplication,
    //!          running in the application thread
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log
    static void LogHandler(Pegasus::Core::LogChannel logChannel, const char * msgStr);

    //! Handler for assertion errors coming from the application itself
    //! \warning This is a static function, so it cannot emit any signal.
    //!          We have to call a member function, \a EmitAssertionFromApplication,
    //!          running in the application thread
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand.
    //!               nullptr if no message is defined.
    //! \return Pegasus::Core::ASSERTION_xxx constant,
    //!         chosen from the button the user clicked on
    static Pegasus::Core::AssertReturnCode AssertionHandler(const char * testStr,
                                                            const char * fileStr,
                                                            int line,
                                                            const char * msgStr);

    //------------------------------------------------------------------------------------

private:

    //! Application interface object, instantiated in the application thread
    ApplicationInterface * mApplicationInterface;

    //! File name of the application to load
    QString mFileName;

    //! Application proxy object created when running the application DLL
    Pegasus::App::IApplicationProxy * mApplication;

    //! Windows used to render the viewports of the application
    Pegasus::Wnd::IWindowProxy * mAppWindow[NUM_VIEWPORT_TYPES];

    //! Return code of the assertion handler. AssertionManager::ASSERTION_INVALID by default.
    //! The value is not the default only when leaving an assertion dialog box.
    //! This is used to freeze the application thread until a return code is present
    AssertionManager::ReturnCode mAssertionReturnCode;

    //! if true, the application fires an OnApplicationFinished message when closed, this allows the message
    //   pump of the editor window to destroy any child windows
    bool mAsyncHandleDestruction;
};


#endif  // EDITOR_APPLICATION_H
