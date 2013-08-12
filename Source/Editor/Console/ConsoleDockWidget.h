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
class QMenu;


//! Dock widget containing the timeline graphics view
class ConsoleDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    ConsoleDockWidget(QWidget *parent);
    ~ConsoleDockWidget();

    //! Add a message to the console text view (default color, no tag)
    //! \param message Message to display (will take an entire line)
    void AddMessage(const QString & message);

    //! Add a message to the console text view with a category tag
    //! (colored based on the settings, with a bracket tag at the beginning of the line)
    //! \param logChannel Pegasus log channel to use for the message
    //! \param message Message to display (will take an entire line)
    void AddMessage(Pegasus::Core::LogChannel logChannel, const QString & message);

    //! Set the background color of the text area
    //! \param color Background color of the text area
    void SetBackgroundColor(const QColor & color);

    //! Set the default text color of the text area
    //! \param color Default text color of the text area
    void SetTextDefaultColor(const QColor & color);

    //------------------------------------------------------------------------------------

private slots:

    //! Called when the context menu is called with a right-click
    //! \param pos Position of the click within the current widget
    void ContextMenuRequested(const QPoint & pos);

    //! Clear the content of the console
    void Clear();

    //! Open the color settings dialog box
    void OpenSetColors();

    //------------------------------------------------------------------------------------

private:

	//! Text widget displaying the log messages
	QPlainTextEdit * mTextWidget;

    //! Context menu associated with the text field
    QMenu * mContextMenu;
};


#endif  // EDITOR_CONSOLEDOCKWIDGET_H
