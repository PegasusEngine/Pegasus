/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Main.cpp
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Entry point of Pegasus Editor

#include "Editor.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Editor w;
    w.show();
    return a.exec();
}
