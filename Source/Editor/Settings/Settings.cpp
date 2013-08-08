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

    // Main window geometry and state
    mMainWindow->restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
    mMainWindow->restoreState(settings.value("MainWindow/State").toByteArray());

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
    SetWidgetStyleName(settings.value("Appearance/WidgetStyle",
                                      mWidgetStyleName).toString());

    // Widget style palette
    mUseWidgetStylePalette = true;
    SetUseWidgetStylePalette(settings.value("Appearance/UseStylePalette",
                                            mUseWidgetStylePalette).toBool());

    // Console colors
    mConsoleBackgroundColor = GetConsoleDefaultBackgroundColor();
    mConsoleTextDefaultColor = GetConsoleDefaultTextDefaultColor();
    SetConsoleBackgroundColor(settings.value("Appearance/ConsoleBackgroundColor",
                                             mConsoleBackgroundColor).value<QColor>());
    SetConsoleTextDefaultColor(settings.value("Appearance/ConsoleTextDefaultColor",
                                              mConsoleTextDefaultColor).value<QColor>());

    // Console colors for the log channels
    /*****/
    //! \todo Handle the color settings
    mLogChannelColorTable['CRIT'] = QColor(255, 0, 0);
    mLogChannelColorTable['ERR_'] = QColor(255, 0, 0);
    mLogChannelColorTable['WARN'] = QColor(255, 128, 0);
    mLogChannelColorTable['ASRT'] = QColor(255, 128, 0);
}

//----------------------------------------------------------------------------------------

void Settings::Save()
{
    // Create the QSettings object that will save the application settings
    QSettings settings;

    //! \todo Use settings items strings common for loading and saving

    // Main window geometry and state
    settings.setValue("MainWindow/Geometry", mMainWindow->saveGeometry());
    settings.setValue("MainWindow/State", mMainWindow->saveState());

    // Widget style
    settings.setValue("Appearance/WidgetStyle", mWidgetStyleName);
    settings.setValue("Appearance/UseStylePalette", mUseWidgetStylePalette);

    // Console colors
    settings.setValue("Appearance/ConsoleBackgroundColor", mConsoleBackgroundColor);
    settings.setValue("Appearance/ConsoleTextDefaultColor", mConsoleTextDefaultColor);

    // Console log channel colors
    //! \todo 
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
