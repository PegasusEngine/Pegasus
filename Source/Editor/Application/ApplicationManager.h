/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ApplicationManager.h
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Manager for the opened applications in the editor

#ifndef EDITOR_APPLICATIONMANAGER_H
#define EDITOR_APPLICATIONMANAGER_H

#include "Application/Application.h"

class Editor;
class ViewportDockWidget;


//! Manager for the opened applications in the editor
class ApplicationManager : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param editor Main window of the editor
    //! \todo Handle multiple dock widgets
    ApplicationManager(Editor * editor, QObject *parent = 0);

    //! Destructor
    virtual ~ApplicationManager();


    //! Open a new application
    //! \param fileName Path and name of the library file to open
    //! \todo Add error management
    void OpenApplication(const QString & fileName);

    //! Close the currently opened application
    void CloseApplication();

    //! Get the currently opened application
    //! \return Pointer to the opened application, nullptr if there is none
    inline Application * GetApplication() const { return mApplication; }

    //! Test if an application is currently opened
    //! \return True if an application is currently opened, false otherwise
    inline bool IsApplicationOpened() const { return mApplication != nullptr; }

    //! Test if an application is running
    //! \return True if an application is running (only after the loading occurred)
    inline bool IsApplicationRunning() const { return mIsApplicationRunning; }

    //------------------------------------------------------------------------------------
    
private slots:

	//! Called when an error occurred when loading the application
    //! \param error Error code
	void LoadingError(Application::Error error);

    //! Called when the application has successfully loaded (not running)
    void LoadingSucceeded();

    //! Called when the application has finished executing
    void ApplicationFinished();

    //------------------------------------------------------------------------------------

private:

    //! Editor main window
    Editor * mEditor;

    //! Pointer to the opened application, nullptr is not assigned
    Application * mApplication;

    //! True if an application is running (only after the loading occurred)
    bool mIsApplicationRunning;
};


#endif  // EDITOR_APPLICATIONMANAGER_H
