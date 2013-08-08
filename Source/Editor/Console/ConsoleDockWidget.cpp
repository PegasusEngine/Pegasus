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
#include <QMenu>
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QTime>


//! Invalid log channel, used for the default text color or unmatched tags
static const Pegasus::Core::LogChannel INVALID_LOG_CHANNEL = 0;

//----------------------------------------------------------------------------------------

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

    // Set the context menu associated with the text field
    QAction * clearAction = new QAction(tr("&Clear"), this);
	clearAction->setStatusTip(tr("Clear the content of the log console"));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(Clear()));
    QAction * setColorsAction = new QAction(tr("&Set Colors..."), this);
	setColorsAction->setStatusTip(tr("Set the colors associated with the channels"));
    connect(setColorsAction, SIGNAL(triggered()), this, SLOT(OpenSetColors()));

    mContextMenu = mTextWidget->createStandardContextMenu();
    mContextMenu->insertSeparator(*mContextMenu->actions().begin());
    mContextMenu->insertAction(*mContextMenu->actions().begin(), setColorsAction);
    mContextMenu->insertAction(*mContextMenu->actions().begin(), clearAction);
    mTextWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTextWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(ContextMenuRequested(const QPoint &)));

    // Set the elements of the layout
    horizontalLayout->addWidget(mTextWidget);

    //! \todo Add support for filter buttons

    //QTextBlock block = mTextWidget->document()->begin();
    //block = block.next();
    //block = block.next();
    //block = block.next();
    //block.setVisible(false);
}

//----------------------------------------------------------------------------------------

ConsoleDockWidget::~ConsoleDockWidget()
{
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::AddMessage(const QString & message)
{
    // Set the invalid category to the message
    AddMessage(INVALID_LOG_CHANNEL, message);
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::AddMessage(Pegasus::Core::LogChannel logChannel, const QString & message)
{
	ED_ASSERT(mTextWidget != nullptr);

    static const QString htmlString1("<p style=\"color:");
    static const QString htmlString2("\">");
    static const QString htmlString3("</p>");

    const Settings * const settings = Editor::GetSettings();
    ED_ASSERTSTR(settings != nullptr, "Settings have not been initialized yet.");

    // Get the current time and convert it into a string
    QString timeString(QTime::currentTime().toString("hh:mm:ss:zzz "));

    if (   (logChannel != INVALID_LOG_CHANNEL)
        && (settings != nullptr)
        && (settings->IsConsoleTextColorDefinedForLogChannel(logChannel)) )
    {
        // HTML string to display (with CSS color and channel tag)
        mTextWidget->appendHtml(  htmlString1
                                + settings->GetConsoleTextColorForLogChannel(logChannel).name()
                                + htmlString2
                                + timeString
                                + "["
                                + ConvertLogChannelToString(logChannel)
                                + "] "
                                + message
                                + htmlString3);
    }
    else if (logChannel != INVALID_LOG_CHANNEL)
    {
        // Standard message with default text color
        mTextWidget->appendPlainText(  timeString 
                                     +  "["
                                     + ConvertLogChannelToString(logChannel)
                                     + "] "
                                     + message);
    }
    else
    {
        // Standard message with default text color
        mTextWidget->appendPlainText(timeString + message);
    }

    //! \todo Do we need manual scrolling when an arbitrary location of the scrollview
    //!       is shown and new messages are added
	//textWidget->moveCursor(QTextCursor::End);
	//textWidget->ensureCursorVisible();
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

//----------------------------------------------------------------------------------------

QString ConsoleDockWidget::ConvertLogChannelToString(Pegasus::Core::LogChannel logChannel)
{
    ED_ASSERTSTR(logChannel != 0, "Invalid log channel");

    return QString("%1%2%3%4").arg(char( logChannel >> 24        ))
                              .arg(char((logChannel >> 16) & 0xFF))
                              .arg(char((logChannel >> 8 ) & 0xFF))
                              .arg(char( logChannel        & 0xFF));
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::ContextMenuRequested(const QPoint & pos)
{
    mContextMenu->exec(QCursor::pos());
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::Clear()
{
    mTextWidget->clear();
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::OpenSetColors()
{
    //! \todo Open the set colors dialog box
}
