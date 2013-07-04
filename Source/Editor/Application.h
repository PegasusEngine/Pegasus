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

class ViewportWidget;


class Application : public QThread
{
    Q_OBJECT

public:

    //! Constructor, does not initialize the application, use \a Open() for that
    Application(QObject *parent = 0);

    //! Destructor, closes the running application
    ~Application();


    //! Set the file name of the application to load
    //! \param fileName Path and name of the library file to open
    //! \todo Add error management
    void SetFile(const QString & fileName);

    //! Set the viewport widget associated with the application
    //! \param viewportWidget Viewport widget that will be associated with the rendering window
    //! \todo Handle multiple viewports
    void SetViewport(/**index,*/ ViewportWidget * viewportWidget);


    //! Error codes for the application thread
    typedef enum Error
    {
        ERROR_INVALID_FILE_NAME = 0,    //!< Invalid file name (not properly formatted)
        ERROR_FILE_NOT_FOUND,           //!< Application file not found
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
    
private:

    //! File name of the application to load
    QString mFileName;

    //! Viewport widget associated with the application
    ViewportWidget * mViewportWidget;
};


#endif  // EDITOR_APPLICATION_H
