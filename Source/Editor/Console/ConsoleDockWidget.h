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
#include <QPlainTextEdit>
#include <QColor>

class QMenu;


//! Text widget object inherited from QPlainTextEdit so its stylesheet can be set without affecting other text edit widgets
class ConsoleTextEditWidget : public QPlainTextEdit
{
    Q_OBJECT

public:

    ConsoleTextEditWidget(QWidget * parent) : QPlainTextEdit(parent) { }
};

//----------------------------------------------------------------------------------------

//! Dock widget containing the timeline graphics view
class ConsoleDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    ConsoleDockWidget(QWidget * parent);
    virtual ~ConsoleDockWidget();

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

    //! Set the filtering state associated with a log channel
    //! \param logChannel Pegasus log channel to set the filtering state of
    //! \param state New filtering state associated with the log channel (true for enabled)
    void SetFilterStateForLogChannel(Pegasus::Core::LogChannel logChannel, bool state);

    //! Set the color associated with a log channel
    //! \note If the log channel was associated with the default text color,
    //!       it starts using the custom color. If the color already existed,
    //!       it is replaced.
    //! \param logChannel Pegasus log channel to set the color of
    //! \param color New color associated with the log channel
    void SetTextColorForLogChannel(Pegasus::Core::LogChannel logChannel, const QColor & color);

    //------------------------------------------------------------------------------------

private slots:

    //! Called when the context menu is called with a right-click
    //! \param pos Position of the click within the current widget
    void ContextMenuRequested(const QPoint & pos);

    //! Clear the content of the console
    void Clear();

    //------------------------------------------------------------------------------------

private:

    //! Set the stylesheet of the text edit depending on the current color state
    void SetTextEditStyleSheetFromColors();


	//! Text widget displaying the log messages
	ConsoleTextEditWidget * mTextWidget;

    //! Current background color of the text widget
    QColor mTextWidgetBackgroundColor;

    //! Current default text color of the text widget
    QColor mTextWidgetDefaultTextColor;

    //! Context menu associated with the text field
    QMenu * mContextMenu;
};


#endif  // EDITOR_CONSOLEDOCKWIDGET_H
