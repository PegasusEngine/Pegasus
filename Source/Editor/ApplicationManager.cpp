/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ApplicationManager.cpp
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Manager for the opened applications in the editor

#include "ApplicationManager.h"
//#include "Pegasus/Core/Assertion.h"


ApplicationManager::ApplicationManager()
:   mApplication(nullptr)
{
}

//----------------------------------------------------------------------------------------

ApplicationManager::~ApplicationManager()
{
    if (IsApplicationOpened())
    {
        CloseApplication();
    }
}

//----------------------------------------------------------------------------------------

void ApplicationManager::OpenApplication(const QString & fileName)
{
    //PG_ASSERTSTR(fileName != nullptr, "Invalid application to open, the name cannot be an empty string");
    //PG_ASSERTSTR(fileName[0] != '\0', "Invalid application to open, the name cannot be an empty string");

    //! \todo Implement the application opening
}

//----------------------------------------------------------------------------------------

void ApplicationManager::CloseApplication()
{
    //PG_ASSERTSTR(mApplication != nullptr, "Trying to close an application that is not opened");

    //! \todo Implement the application closing

    mApplication = nullptr;
}

