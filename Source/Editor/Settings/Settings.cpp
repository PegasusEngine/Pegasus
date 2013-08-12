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
        settings.endGroup();    // ConsoleTextColor
    }
    settings.endGroup();    // Appearance
}

//----------------------------------------------------------------------------------------

void Settings::Save()
{
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
                    ED_FAILSTR("Invalid Pegasus log channel name, it must be 4 characters long");
                }
            }
        }
        settings.endGroup();    // ConsoleTextColor
    }
    settings.endGroup();    // Appearance
}

//----------------------------------------------------------------------------------------

const QStringList & Settings::GetWidgetStyleNameList() const
{
    return mWidgetStyleNameList;
}

//----------------------------------------------------------------------------------------

const QString & Settings::GetWidgetStyleName() const
{
    return mWidgetStyleName;
}

//----------------------------------------------------------------------------------------

const QColor & Settings::GetConsoleBackgroundColor() const
{
    return mConsoleBackgroundColor;
}

//----------------------------------------------------------------------------------------

const QColor & Settings::GetConsoleTextDefaultColor() const
{
    return mConsoleTextDefaultColor;
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

void Settings::SetConsoleTextColorForLogChannel(Pegasus::Core::LogChannel logChannel, const QColor & color)
{
    mLogChannelColorTable[logChannel] = color;    

    //! \todo Update the content of the log console for the channels with custom colors
}

//----------------------------------------------------------------------------------------

bool Settings::IsConsoleTextColorDefinedForLogChannel(Pegasus::Core::LogChannel logChannel) const
{
    return mLogChannelColorTable.contains(logChannel);
}

//----------------------------------------------------------------------------------------
	
bool Settings::IsUsingWidgetStylePalette() const
{
    return mUseWidgetStylePalette;
}

//----------------------------------------------------------------------------------------

void Settings::SetWidgetStyleName(const QString & name)
{
    if (mWidgetStyleNameList.contains(name))
    {
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
        QApplication::setPalette(QApplication::style()->standardPalette());
    }
    else
    {
        QApplication::setPalette(mOriginalPalette);
    }
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleBackgroundColor(const QColor & color)
{
    mConsoleBackgroundColor = color;

    ED_ASSERT(Editor::GetInstance().GetConsoleDockWidget());
    Editor::GetInstance().GetConsoleDockWidget()->SetBackgroundColor(mConsoleBackgroundColor);
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleTextDefaultColor(const QColor & color)
{
    mConsoleTextDefaultColor = color;

    ED_ASSERT(Editor::GetInstance().GetConsoleDockWidget());
    Editor::GetInstance().GetConsoleDockWidget()->SetTextDefaultColor(mConsoleTextDefaultColor);
}

//----------------------------------------------------------------------------------------

QColor Settings::GetConsoleDefaultBackgroundColor() const
{
    return QColor(Qt::darkBlue).darker(200);
}

//----------------------------------------------------------------------------------------
	
QColor Settings::GetConsoleDefaultTextDefaultColor() const
{
    return Qt::yellow;
}

//----------------------------------------------------------------------------------------

QString Settings::ConvertLogChannelToString(Pegasus::Core::LogChannel logChannel)
{
    ED_ASSERTSTR(logChannel != 0, "Invalid log channel");

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
                ED_FAILSTR("Invalid string to convert to a Pegasus log channel, it must be made of letters, numbers or underscores");
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
        ED_FAILSTR("Invalid string to convert to a Pegasus log channel, it must be 4 characters long");
        return INVALID_LOG_CHANNEL;
    }
}