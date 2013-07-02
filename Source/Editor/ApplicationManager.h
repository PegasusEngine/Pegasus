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

#include "Application.h"
#include "Pegasus/Core/Singleton.h"


//! \class Manager for the opened applications in the editor
class ApplicationManager : public Pegasus::Core::ManualSingleton<ApplicationManager>
{
public:

    ApplicationManager();
    ~ApplicationManager();

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


private:

    //! Pointer to the opened application, nullptr is not assigned
    Application * mApplication;
};


#endif  // EDITOR_APPLICATIONMANAGER_H
