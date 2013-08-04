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
    mTextWidget->setWordWrapMode(QTextOption::NoWrap);
	mTextWidget->setReadOnly(true);
	QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mTextWidget->sizePolicy().hasHeightForWidth());
    mTextWidget->setSizePolicy(sizePolicy);
    mTextWidget->setMinimumSize(QSize(256, 32));

    //! \todo Add support for limited number of lines
    //mTextWidget->setMaximumBlockCount(50);

    // Set the font of the text widget
	QFont font("Courier New");
	font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
	font.setPointSize(8);
	mTextWidget->setFont(font);

    // Set the elements of the layout
    horizontalLayout->addWidget(mTextWidget);

    //! \todo Add support for filter buttons

	// Test message
    /****/
	for (int i = 0; i < 100; i++)
    {
        if ((i / 10) & 1)
        {
            static const QString htmlString1("<p style=\"color:");
            static const QString htmlString2("\">");
            static const QString htmlString3("</p>");
            QString htmlString(htmlString1);
            QColor color(i * 2, (unsigned char)(i * 37), (unsigned char)(i * 137));
            htmlString.append(color.name());
            htmlString.append(htmlString2);
            htmlString.append(QString("Hello, world %1!").arg(i));
            htmlString.append(htmlString3);
            mTextWidget->appendHtml(htmlString);
        }
        else
        {
            mTextWidget->appendHtml(QString("Hello, world %1!").arg(i));
        }
    }
	//mTextWidget->moveCursor(QTextCursor::End);
	//mTextWidget->ensureCursorVisible();
}

//----------------------------------------------------------------------------------------

ConsoleDockWidget::~ConsoleDockWidget()
{

}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::SetBackgroundColor(const QColor & color)
{
	ED_ASSERT(mTextWidget);

	QPalette palette = mTextWidget->palette();
	palette.setColor(QPalette::Base, color);
	mTextWidget->setPalette(palette);
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::SetTextDefaultColor(const QColor & color)
{
	ED_ASSERT(mTextWidget);

	QPalette palette = mTextWidget->palette();
	palette.setColor(QPalette::Text, color);
	mTextWidget->setPalette(palette);
}

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
