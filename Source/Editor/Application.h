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

    //! Running function of the thread, initializes the engine and the application,
    //! then runs the main loop
    //! \warning Do not rename to Run(), as this function is an overload of QThread
    void run();


private:

    //! File name of the application to load
    QString mFileName;
};


#endif  // EDITOR_APPLICATION_H
