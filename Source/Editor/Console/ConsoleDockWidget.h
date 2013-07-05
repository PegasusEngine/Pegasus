/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConsoleDockWidget.h
//! \author	Kevin Boulanger
//! \date	05th July 2013
//! \brief	Dock widget containing the console with log messages

#ifndef EDITOR_CONSOLEDOCKWIDGET_H
#define EDITOR_CONSOLEDOCKWIDGET_H

#include <QDockWidget>

class QPlainTextEdit;


//! Dock widget containing the timeline graphics view
class ConsoleDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    ConsoleDockWidget(QWidget *parent);
    ~ConsoleDockWidget();

private:

	//! Text widget displaying the log messages
	QPlainTextEdit * mTextWidget;
};


#endif  // EDITOR_CONSOLEDOCKWIDGET_H
