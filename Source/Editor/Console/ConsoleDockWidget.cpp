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
#include "Settings/Settings.h"
#include "Editor.h"

#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QTime>


//! Class to store user data per text block (each line of text)
class TextBlockUserData : public QTextBlockUserData
{
public:

    TextBlockUserData(Pegasus::Core::LogChannel logChannel)
        : mLogChannel(logChannel) { }

    inline Pegasus::Core::LogChannel GetLogChannel() const { return mLogChannel; }

private:

    //! Log channel associated with the text block,
    //! Settings::INVALID_LOG_CHANNEL if not associated with any channel
    const Pegasus::Core::LogChannel mLogChannel;
};

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
    connect(setColorsAction, SIGNAL(triggered()),
            &Editor::GetInstance(), SLOT(OpenPreferencesConsole()));

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
}

//----------------------------------------------------------------------------------------

ConsoleDockWidget::~ConsoleDockWidget()
{
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::AddMessage(const QString & message)
{
    // Set the invalid category to the message
    AddMessage(Settings::INVALID_LOG_CHANNEL, message);
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::AddMessage(Pegasus::Core::LogChannel logChannel, const QString & message)
{
	ED_ASSERT(mTextWidget != nullptr);

    static const QString htmlString1("<p style=\"color:");
    static const QString htmlString2("\">");
    static const QString htmlString3("</p>");

    const Settings * const settings = Editor::GetSettings();

    // Get the current time and convert it into a string
    QString timeString(QTime::currentTime().toString("hh:mm:ss:zzz "));

    if (   (logChannel != Settings::INVALID_LOG_CHANNEL)
        && (settings != nullptr)
        && (settings->IsConsoleTextColorDefinedForLogChannel(logChannel)) )
    {
        // HTML string to display (with CSS color and channel tag)
        mTextWidget->appendHtml(  htmlString1
                                + settings->GetConsoleTextColorForLogChannel(logChannel).name()
                                + htmlString2
                                + timeString
                                + "["
                                + Settings::ConvertLogChannelToString(logChannel)
                                + "] "
                                + message
                                + htmlString3);
    }
    else if (logChannel != Settings::INVALID_LOG_CHANNEL)
    {
        // Standard message with default text color
        mTextWidget->appendPlainText(  timeString 
                                     +  "["
                                     + Settings::ConvertLogChannelToString(logChannel)
                                     + "] "
                                     + message);
    }
    else
    {
        // Standard message with default text color
        mTextWidget->appendPlainText(timeString + message);
    }

    // Create user data for the text block,
    // to optimize the recoloring after changing settings
    QTextDocument * doc = mTextWidget->document();
    QTextBlock block = doc->lastBlock();
    QTextBlockUserData * userData = new TextBlockUserData(logChannel);
    block.setUserData(userData);
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

    // No need to go through the text blocks.
    // No HTML tag is used when the default color is used, so the color changes
    // automatically without user interaction
}

//----------------------------------------------------------------------------------------

void ConsoleDockWidget::SetTextColorForLogChannel(Pegasus::Core::LogChannel logChannel, const QColor & color)
{
    ED_ASSERT(mTextWidget);
    if (logChannel == Settings::INVALID_LOG_CHANNEL)
    {
        ED_FAILSTR("Invalid log channel, it must be defined in Pegasus");
        return;
    }

    // Check every text block for the color being used and update if required
    QTextDocument * doc = mTextWidget->document();
    QTextCharFormat charFormat;
    charFormat.setForeground(color);
    for (QTextBlock textBlock = doc->begin(); textBlock != doc->end(); textBlock = textBlock.next())
    {
        // Check whether the block corresponds to the current channel
        TextBlockUserData * userData = static_cast<TextBlockUserData *>(textBlock.userData());
        if (   userData != nullptr
            && (userData->GetLogChannel() == logChannel))
        {
            // Create a cursor belonging to the text block
            QTextCursor textCursor(textBlock);

            // Set the selection of the text to the entire block.
            // Without this, only inserted text gets the new color, not the existing one.
            textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

            // Apply the color to the selected text
            textCursor.mergeCharFormat(charFormat);
        }
    }
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
