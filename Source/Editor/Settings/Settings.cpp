/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Settings.cpp
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Settings for the entire editor

#include "Settings/Settings.h"

#include <QStyleFactory>
#include <QStyle>
#include <QSettings>
#include <QApplication>
#include <QMainWindow>


const QColor Settings::sDefaultSyntaxHighlightColors[Settings::SYNTAX_COUNT] =
{
    QColor(0, 0, 0, 255),  //background
    QColor(20, 255, 20, 255),  //CPP Comment
    QColor(0, 255, 0, 255),  //C Comment
    QColor(255, 255, 255, 255),  //regular text
    QColor(130, 255, 50, 255),  //keyword
    QColor(230, 54, 0, 255) //imm number value
};

const char * Settings::sShaderEditorSyntaxStyleNames[Settings::SYNTAX_COUNT] =
{
    "background",
    "cpp comment",
    "c comment",
    "regular text",
    "keyword",
    "magic number"
};

Settings::Settings(QMainWindow * mainWindow)
:   QObject(),
    mMainWindow(mainWindow)
{
    // Set internal variables
    mWidgetStyleNameList = QStyleFactory::keys();
    ED_ASSERTSTR(!mWidgetStyleNameList.isEmpty(), "Invalid list of themes for the UI.");
    mOriginalPalette = QApplication::palette();

    // Load the settings if possible and apply them
    Load();
}

//----------------------------------------------------------------------------------------

Settings::~Settings()
{
}

//----------------------------------------------------------------------------------------

void Settings::Load()
{
    ED_LOG("Loading the editor settings");

    // Create the QSettings object that will read the application settings from file
    QSettings settings;

    settings.beginGroup("MainWindow");
    {
        // Main window geometry and state
        mMainWindow->restoreGeometry(settings.value("Geometry").toByteArray());
        mMainWindow->restoreState(settings.value("State").toByteArray());
    }
    settings.endGroup();    // MainWindow

    settings.beginGroup("Appearance");
    {
        // Widget style
        //! \todo Select the proper initial style in the combo box
        /*if (widgetStyleNameList.contains("WindowsVista"))
        widgetStyleName = "WindowsVista";
        else if (widgetStyleNameList.contains("WindowsXP"))
        widgetStyleName = "WindowsXP";
        else if (widgetStyleNameList.contains("Macintosh"))
        widgetStyleName = "Macintosh";
        else if (widgetStyleNameList.contains("Cleanlooks"))
        widgetStyleName = "Cleanlooks";
        else if (widgetStyleNameList.contains("Plastique"))
        widgetStyleName = "Plastique";
        else widgetStyleName = */mWidgetStyleNameList[0];
        SetWidgetStyleName(settings.value("WidgetStyle",
                                          mWidgetStyleName).toString());

        // Widget style palette
        mUseWidgetStylePalette = true;
        SetUseWidgetStylePalette(settings.value("UseStylePalette",
                                                mUseWidgetStylePalette).toBool());

        // Timeline graphics view antialiasing
        mUseTimelineAntialiasing = false;
        SetUseTimelineAntialiasing(settings.value("UseTimelineAntialiasing",
                                                  mUseTimelineAntialiasing).toBool());

        // Console colors
        mConsoleBackgroundColor = GetConsoleDefaultBackgroundColor();
        mConsoleTextDefaultColor = GetConsoleDefaultTextDefaultColor();
        SetConsoleBackgroundColor(settings.value("ConsoleBackgroundColor",
                                                 mConsoleBackgroundColor).value<QColor>());
        SetConsoleTextDefaultColor(settings.value("ConsoleTextDefaultColor",
                                                  mConsoleTextDefaultColor).value<QColor>());

        settings.beginGroup("ConsoleTextColor");
        {
            // Console colors for the log channels
            mLogChannelColorTable.clear();
            const QStringList loadedChannelNames = settings.allKeys();
            if (loadedChannelNames.count() == 0)
            {
                // If no key has been found, consider that no setting has been set
                // and use the default values for some of the keys
                SetDefaultConsoleTextColorForAllLogChannels();
            }
            else
            {
                // If keys have been found, apply them
                for (QStringList::const_iterator channelName = loadedChannelNames.constBegin();
                     channelName != loadedChannelNames.constEnd();
                     ++channelName)
                {
                    const Pegasus::Core::LogChannel logChannel = ConvertStringToLogChannel(*channelName);
                    if (logChannel != INVALID_LOG_CHANNEL)
                    {
                        QColor loadedColor = settings.value(*channelName).value<QColor>();
                        mLogChannelColorTable[logChannel] = loadedColor;
                    }
                }
            }
        }
        settings.endGroup();    // ConsoleTextColor

        settings.beginGroup("ShaderEditorSyntaxColor");
        {
            char syntaxColorStrName[256];
            for (unsigned i = 0; i < Settings::SYNTAX_COUNT; ++i)
            {
                sprintf_s(syntaxColorStrName, "syntaxcol_%d", i);
                SetShaderEditorColor(
                        static_cast<Settings::ShaderEditorSyntaxStyle>(i), 
                        settings.value(
                            syntaxColorStrName,
                            sDefaultSyntaxHighlightColors[i]).value<QColor>()
                );
            } 
        }
        settings.endGroup();    // ConsoleTextColor

    }
    settings.endGroup();    // Appearance
}

//----------------------------------------------------------------------------------------

void Settings::Save()
{
    ED_LOG("Saving the editor settings");

    // Create the QSettings object that will save the application settings
    QSettings settings;

    //! \todo Use settings items strings common for loading and saving

    settings.beginGroup("MainWindow");
    {
        // Main window geometry and state
        settings.setValue("Geometry", mMainWindow->saveGeometry());
        settings.setValue("State", mMainWindow->saveState());
    }
    settings.endGroup();    // MainWindow

    settings.beginGroup("Appearance");
    {
        // Widget style
        settings.setValue("WidgetStyle", mWidgetStyleName);
        settings.setValue("UseStylePalette", mUseWidgetStylePalette);

        // Timeline graphics view antialiasing
        settings.setValue("UseTimelineAntialiasing", mUseTimelineAntialiasing);

        // Console colors
        settings.setValue("ConsoleBackgroundColor", mConsoleBackgroundColor);
        settings.setValue("ConsoleTextDefaultColor", mConsoleTextDefaultColor);

        settings.beginGroup("ConsoleTextColor");
        {
            // Console log channel colors
            // (delete the old keys and write only the ones that are not using the default color)
            settings.remove("");
            QMap<Pegasus::Core::LogChannel, QColor>::const_iterator channel;
            for (channel = mLogChannelColorTable.constBegin(); channel != mLogChannelColorTable.constEnd(); ++channel)
            {
                const QString channelName(ConvertLogChannelToString(channel.key()));
                if (channelName.length() == 4)
                {
                    settings.setValue(channelName, channel.value());
                }
                else
                {
                    ED_FAILSTR("Invalid Pegasus log channel name (%s), it must be 4 characters long.", channelName.toLatin1().constData());
                }
            }
        }
        settings.endGroup();    // ConsoleTextColor

        settings.beginGroup("ShaderEditorSyntaxColor");
        {
            char syntaxColorStrName[256];
            for (unsigned i = 0; i < Settings::SYNTAX_COUNT; ++i)
            {
                sprintf_s(syntaxColorStrName, "syntaxcol_%d", i);
                settings.setValue(
                      syntaxColorStrName,
                      GetShaderSyntaxColor(static_cast<Settings::ShaderEditorSyntaxStyle>(i))
                );
            } 
        }
        settings.endGroup();    // ConsoleTextColor
    }
    settings.endGroup();    // Appearance
}

//----------------------------------------------------------------------------------------

const QColor Settings::GetConsoleTextColorForLogChannel(Pegasus::Core::LogChannel logChannel) const
{
    if (mLogChannelColorTable.contains(logChannel))
    {
        return mLogChannelColorTable[logChannel];
    }
    else
    {
        return mConsoleTextDefaultColor;
    }
}

//----------------------------------------------------------------------------------------

bool Settings::IsConsoleTextColorDefinedForLogChannel(Pegasus::Core::LogChannel logChannel) const
{
    return mLogChannelColorTable.contains(logChannel);
}

//----------------------------------------------------------------------------------------

void Settings::SetWidgetStyleName(const QString & name)
{
    if (mWidgetStyleNameList.contains(name))
    {
        ED_LOG("Applying the style '%s' to the user interface", name.toLatin1().constData());

        // Apply the new widget style
        mWidgetStyleName = name;
        QApplication::setStyle(QStyleFactory::create(mWidgetStyleName));

        // Set the palette of the new widget style
        SetUseWidgetStylePalette(mUseWidgetStylePalette);
    }
}

//----------------------------------------------------------------------------------------

void Settings::SetUseWidgetStylePalette(bool stylePalette)
{
    mUseWidgetStylePalette = stylePalette;

    if (mUseWidgetStylePalette)
    {
        ED_LOG("Setting the user interface to use the palette associated with the theme");

        QApplication::setPalette(QApplication::style()->standardPalette());
    }
    else
    {
        ED_LOG("Setting the user interface to not use the palette associated with the theme");

        QApplication::setPalette(mOriginalPalette);
    }
}

//----------------------------------------------------------------------------------------

void Settings::SetUseTimelineAntialiasing(bool timelineAntialiasing)
{
    if (timelineAntialiasing)
    {
        ED_LOG("Enabling antialiasing for the timeline");
    }
    else
    {
        ED_LOG("Disabling antialiasing for the timeline");
    }

    mUseTimelineAntialiasing = timelineAntialiasing;

    ED_ASSERT(Editor::GetInstance().GetTimelineDockWidget() != nullptr);
    Editor::GetInstance().GetTimelineDockWidget()->EnableAntialiasing(mUseTimelineAntialiasing);
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleBackgroundColor(const QColor & color)
{
    ED_LOG("Setting the console background color");

    mConsoleBackgroundColor = color;

    ED_ASSERT(Editor::GetInstance().GetConsoleDockWidget() != nullptr);
    Editor::GetInstance().GetConsoleDockWidget()->SetBackgroundColor(mConsoleBackgroundColor);
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleTextDefaultColor(const QColor & color)
{
    ED_LOG("Setting the console text default color");

    mConsoleTextDefaultColor = color;

    ED_ASSERT(Editor::GetInstance().GetConsoleDockWidget() != nullptr);
    Editor::GetInstance().GetConsoleDockWidget()->SetTextDefaultColor(mConsoleTextDefaultColor);
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleTextColorForLogChannel(Pegasus::Core::LogChannel logChannel, const QColor & color)
{
    ED_LOG("Setting the console text color for the log channel [%s]",
           ConvertLogChannelToString(logChannel).toLatin1().constData());

    mLogChannelColorTable[logChannel] = color;    
    Editor::GetInstance().GetConsoleDockWidget()->SetTextColorForLogChannel(logChannel, color);
}

//----------------------------------------------------------------------------------------

void Settings::SetShaderEditorColor(ShaderEditorSyntaxStyle style, const QColor& color)
{
    ED_LOG("Setting syntax highlight color");
    mShaderEditorColorStyles[style] = color;
    
    emit(OnShaderEditorStyleChanged());
}

//----------------------------------------------------------------------------------------

void Settings::SetDefaultConsoleTextColorForAllLogChannels()
{
    ED_LOG("Setting the default console text color for all log channels");

    // Remove all customized colors
    mLogChannelColorTable.clear();

    // Set the default colors of a few important channels
    mLogChannelColorTable['CRIT'] = QColor(255, 0, 0);
    mLogChannelColorTable['ERR_'] = QColor(255, 0, 0);
    mLogChannelColorTable['WARN'] = QColor(255, 128, 0);
    mLogChannelColorTable['ASRT'] = QColor(255, 128, 255);
    mLogChannelColorTable['EDIT'] = QColor(128, 255, 255);
    mLogChannelColorTable['TEMP'] = QColor(0, 255, 0);

    //! \todo Set the color of the existing log lines (set and remove custom colors)
}

//----------------------------------------------------------------------------------------

QColor Settings::GetConsoleDefaultBackgroundColor() const
{
    return QColor(0, 0, 48);
}

//----------------------------------------------------------------------------------------
	
QColor Settings::GetConsoleDefaultTextDefaultColor() const
{
    return Qt::yellow;
}

//----------------------------------------------------------------------------------------

QString Settings::ConvertLogChannelToString(Pegasus::Core::LogChannel logChannel)
{
    ED_ASSERTSTR(logChannel != 0, "Invalid log channel (%d). It should be != 0.", logChannel);

    return QString("%1%2%3%4").arg(char( logChannel >> 24        ))
                              .arg(char((logChannel >> 16) & 0xFF))
                              .arg(char((logChannel >> 8 ) & 0xFF))
                              .arg(char( logChannel        & 0xFF));
}

//----------------------------------------------------------------------------------------

Pegasus::Core::LogChannel Settings::ConvertStringToLogChannel(const QString & channelName)
{
    if (channelName.length() == 4)
    {
        // Check that the string contains only valid characters
        Pegasus::Core::LogChannel logChannel = 0;
        for (int c = 0; c < 4; ++c)
        {
            if (channelName[c].isLetterOrNumber() || (channelName[c] == QChar('_')))
            {
                logChannel |= Pegasus::Core::LogChannel(channelName[c].toLatin1()) << ((3 - c) * 8);
            }
            else
            {
                ED_FAILSTR("Invalid string (%s) to convert to a Pegasus log channel, it must be made of letters, numbers or underscores.",
                           channelName.toLatin1().constData());
                return INVALID_LOG_CHANNEL;
            }
        }

        // Check that the log channel exists in Pegasus
        for (Pegasus::Core::LogChannel pegasusChannel = 0; pegasusChannel < Pegasus::Core::NUM_LOG_CHANNELS; ++pegasusChannel)
        {
            if (logChannel == Pegasus::Core::sLogChannels[pegasusChannel])
            {
                return logChannel;
            }
        }

        // If the log channel does not match any of the Pegasus log channels, silently ignore 
        return INVALID_LOG_CHANNEL;
    }
    else
    {
        ED_FAILSTR("Invalid string (%s) to convert to a Pegasus log channel, it must be 4 characters long.",
                   channelName.toLatin1().constData());
        return INVALID_LOG_CHANNEL;
    }
}
