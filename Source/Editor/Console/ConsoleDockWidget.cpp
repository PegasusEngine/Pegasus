/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConsoleDockWidget.h
//! \author	Kevin Boulanger
//! \date	05th July 2013
//! \brief	Dock widget containing the console with log messages

#include "Console/ConsoleDockWidget.h"

#include <QPlainTextEdit>
#include <QHBoxLayout>


ConsoleDockWidget::ConsoleDockWidget(QWidget *parent)
:   QDockWidget(parent)
{
    // Set the dock widget parameters
    setWindowTitle(tr("Console"));
    setObjectName("ConsoleDockWidget");
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

    // Create the main widget of the dock, covering the entire child area
    QWidget * mainWidget = new QWidget(this);
    setWidget(mainWidget);

    // Create the horizontal layout that will contain the console and the filter buttons
    QHBoxLayout * horizontalLayout = new QHBoxLayout(mainWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    //// Create the console widget and set its size policy
    //mConsoleWidget = new ConsoleWidget(mainWidget);

    //! \todo Add support for colored text

    // Create the text widget displaying the log messages
    mTextWidget = new QPlainTextEdit(mainWidget);
	mTextWidget->setAcceptDrops(false);
	mTextWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	mTextWidget->setUndoRedoEnabled(false);
	mTextWidget->setLineWrapMode(QPlainTextEdit::NoWrap);
	mTextWidget->setReadOnly(true);
	mTextWidget->setTextInteractionFlags(Qt::NoTextInteraction);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mTextWidget->sizePolicy().hasHeightForWidth());
    mTextWidget->setSizePolicy(sizePolicy);
    mTextWidget->setMinimumSize(QSize(256, 128));

    // Set the font of the text widget
    //! \todo Why is TypeWriter not working?
	QFont font = mTextWidget->font();
	font.setStyleHint(QFont::TypeWriter);
	font.setPointSize(9);
	mTextWidget->setFont(font);

    // Set the elements of the layout
    horizontalLayout->addWidget(mTextWidget);

    //! \todo Add support for filter buttons

	// Test message
    /****/
	mTextWidget->appendPlainText("Hello, world!");
	mTextWidget->moveCursor(QTextCursor::End);
	mTextWidget->ensureCursorVisible();
}

//----------------------------------------------------------------------------------------

ConsoleDockWidget::~ConsoleDockWidget()
{

}

////----------------------------------------------------------------------------------------
//
//void ED_Console::SetBackgroundColor(const QColor & _color)
//{
//	N3D_ASSERT(textWidget);
//
//	QPalette _palette = textWidget->palette();
//	_palette.setColor(QPalette::Base, _color);
//	textWidget->setPalette(_palette);
//}
//
////----------------------------------------------------------------------------------------
//
//void ED_Console::SetTextColor(const QColor & _color)
//{
//	N3D_ASSERT(textWidget);
//
//	QPalette _palette = textWidget->palette();
//	_palette.setColor(QPalette::Text, _color);
//	textWidget->setPalette(_palette);
//}
////----------------------------------------------------------------------------------------
//
//QSize ED_Console::sizeHint() const
//{
//	return QSize(300, 85);
//}
//
////----------------------------------------------------------------------------------------
//
//QSize ED_Console::minimumSizeHint() const
//{
//	return QSize(300, 85);
//}
////----------------------------------------------------------------------------------------
//
//void ED_ConsoleDebugOutput::AddMessage(const QString & _message)
//{
//	N3D_ASSERT(textWidget);
//	
//	// Append the message to the text widget
//	textWidget->appendPlainText(_message);
//
//	// Select the last item of the list widget
//	//textWidget->verticalScrollBar()->setValue(
//	//			textWidget->verticalScrollBar()->maximum());
//	textWidget->moveCursor(QTextCursor::End);
//	textWidget->ensureCursorVisible();
//}
