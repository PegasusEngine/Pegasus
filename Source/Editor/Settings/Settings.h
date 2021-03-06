/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Settings.h
//! \author	Karolyn Boulanger
//! \date	10th July 2013
//! \brief	Settings for the entire editor

#ifndef EDITOR_SETTINGS_H
#define EDITOR_SETTINGS_H

#include <QString>
#include <QStringList>
#include <QPalette>
#include <QMap>

#include "Pegasus/Core/Shared/LogChannel.h"

class QMainWindow;
class Editor;


//! Settings for the entire editor
class Settings : public QObject
{
    Q_OBJECT

public:

    //! Constructor reading the parameters from the operating system,
    //! setting the default values for the unknown parameters
    //! and applying the values to the application
    //! \param mainWindow Pointer to the main editor window. This allows to save its geometry and the state of the dock widgets
    Settings(Editor* editor);

    //! Destructor
    virtual ~Settings();

    //! Load the settings from a configuration file or the registry
    //! \note The function does not fail. If loading of the settings fails, the default values are used
    void Load();

    //! Save the settings into a configuration file or the registry
    //! \note The function does not fail. If saving of the settings fails, nothing happens
    void Save();

    //! Forces a signal that the settings have been changed.
    void NotifySettingsChanged();

    //------------------------------------------------------------------------------------

public:

    // definitions

    //syntax style list
    enum CodeEditorSyntaxStyle
    {
        SYNTAX_BACKGROUND = 0,
        SYNTAX_CPP_COMMENT,
        SYNTAX_C_COMMENT,
        SYNTAX_NORMAL_TEXT,
        SYNTAX_KEYWORD,
        SYNTAX_NUMBER_VALUE,
        SYNTAX_LINE_NUMBER_BG,
        SYNTAX_LINE_NUMBER_COL,
        SYNTAX_SELECTED_LINE_BG_COL,
        SYNTAX_COUNT
    };

    //name of syntax styles
    static const char * sCodeEditorSyntaxStyleNames[SYNTAX_COUNT];

    static const QColor sDefaultSyntaxHighlightColors[SYNTAX_COUNT];

    // Appearance settings

    //! Get the list of possible widget style names
    //! Get the list of possible widget style names
    inline const QStringList & GetWidgetStyleNameList() const { return mWidgetStyleNameList; }

    //! Get the name of the widget style
    //! \return Name of the widget style ("Windows", "CDE", "Plastique", etc. for example)
    inline const QString & GetWidgetStyleName() const { return mWidgetStyleName; }

    //! Test whether the widget style uses the style palette
    //! \return True if the widget style uses the style palette,
    //!         false if it uses the original palette
    inline bool IsUsingWidgetStylePalette() const { return mUseWidgetStylePalette; }

    //! Test whether the timeline graphics view uses antialiasing
    //! \return True if the timeline graphics view uses antialiasing
    inline bool IsUsingTimelineAntialiasing() const { return mUseTimelineAntialiasing; }


    // Console settings

    //! Get the background color of the console
    //! \return Background color of the console
    inline const QColor & GetConsoleBackgroundColor() const { return mConsoleBackgroundColor; }

    //! Get the default text color of the console
    //! \return Default text color of the console
    inline const QColor & GetConsoleTextDefaultColor() const { return mConsoleTextDefaultColor; }

    //! Get the filtering state associated with a log channel, enabled if not found
    //! \param logChannel Pegasus log channel to get the filtering state of
    //! \return Filtering state associated with the log channel (false if channel disabled),
    //!         true if not found
    bool GetConsoleFilterStateForLogChannel(Pegasus::Core::LogChannel logChannel) const;

    //! Test if a console log channel has an associated filter state
    //! \param logChannel Pegasus log channel to test
    bool IsConsoleFilterStateDefinedForLogChannel(Pegasus::Core::LogChannel logChannel) const;

    //! Get the color associated with a log channel, default text color if not found
    //! \param logChannel Pegasus log channel to get the color of
    //! \return Color associated with the log channel, default text color if not found
    //!         (not a reference since it seems to not work well with the internal QMap)
    QColor GetConsoleTextColorForLogChannel(Pegasus::Core::LogChannel logChannel) const;

    //! Test if a console log channel has an associated color
    //! \param logChannel Pegasus log channel to test
    bool IsConsoleTextColorDefinedForLogChannel(Pegasus::Core::LogChannel logChannel) const;


    // Code editor settings

    //! Set Code editor style color
    //! \param style the syntax to set for a particular color
    //! \param color the color
    void SetCodeEditorColor(CodeEditorSyntaxStyle style, const QColor& color);

    //! Returns the Code editor color from a particular syntax style
    //! \return the syntax style to get the color from
    const QColor& GetCodeSyntaxColor(CodeEditorSyntaxStyle style) const { return mCodeEditorColorStyles[style]; }

    //! Returns the font size of the Code editor font
    //! \return the font size
    int GetCodeEditorFontSize() const { return mCodeEditorFontSize; }

    //! Returns the tab size of the Code editor
    //! \return the tab size
    int GetCodeEditorTabSize()  const { return mCodeEditorTabSize; }

    //! sets the editor font size and emits UI update event
    //! \param the new font size
    void SetCodeEditorFontSize(int newFontSize);

    //! sets the editor tab size and emits UI update event
    //! \param the new tab size
    void SetCodeEditorTabSize(int newTabSize);



public slots:

    //! Set the name of the widget style
    //! \note If the input name is invalid, no style change occurs
    //! \param name Name of the widget style (one of the style names
    //!             returned by \a GetWidgetStyleNameList)
    void SetWidgetStyleName(const QString & name);

    //! Set the palette to use for the widget style
    //! \param stylePalette true to use the style palette,
    //!                     false to use the original palette
    void SetUseWidgetStylePalette(bool stylePalette);

    //! Set the use of antialiasing for the timeline graphics view
    //! \param timelineAntialiasing True if the timeline graphics view uses antialiasing
    void SetUseTimelineAntialiasing(bool timelineAntialiasing);

    //! Set the background color of the console
    //! \param color Background color of the console
    void SetConsoleBackgroundColor(const QColor & color);

    //! Set the default text color of the console
    //! \param color Default text color of the console
    void SetConsoleTextDefaultColor(const QColor & color);

    //! Set the filtering state associated with a log channel
    //! \param logChannel Pegasus log channel to set the filtering state of
    //! \param state True to enable the channel, false otherwise
    void SetConsoleFilterStateForLogChannel(Pegasus::Core::LogChannel logChannel, bool state);

    //! Set the default filter state associated with all log channels
    void SetDefaultConsoleFilterStateForAllLogChannels();

    //! Set the color associated with a log channel
    //! \note If the log channel was associated with the default text color,
    //!       it starts using the custom color. If the color already existed,
    //!       it is replaced.
    //! \param logChannel Pegasus log channel to set the color of
    //! \param color Text color for the log channel
    void SetConsoleTextColorForLogChannel(Pegasus::Core::LogChannel logChannel, const QColor & color);

    //! Set the default colors associated with all log channels
    //! \note Sets a few channels to special colors (like errors),
    //!       all other channels use the default text color
    void SetDefaultConsoleTextColorForAllLogChannels();

    //------------------------------------------------------------------------------------

signals:
    void OnCodeEditorStyleChanged();

public:

    // Default value for some of the settings

    //! Get the default background color of the console
    //! \return Default background color of the console
    QColor GetConsoleDefaultBackgroundColor() const;

    //! Get the default default text color of the console
    //! \return Default default text color of the console
    QColor GetConsoleDefaultTextDefaultColor() const;

    //------------------------------------------------------------------------------------

public:

    //! Invalid log channel, used for the default text color or unmatched tags
    static const Pegasus::Core::LogChannel INVALID_LOG_CHANNEL = 0;

    //! Convert a Pegasus log channel into a string
    //! \param logChannel Pegasus log channel
    //! \return 4-character string representing the log channel
    static QString ConvertLogChannelToString(Pegasus::Core::LogChannel logChannel);

    //! Convert a log channel string to a Pegasus log channel
    //! \param name 4-character channel name
    //! \return Converted log channel, INVALID_LOG_CHANNEL if the input format is invalid
    //!         (also throws an assertion error) or if the log channel is unknown (no assert)
    static Pegasus::Core::LogChannel ConvertStringToLogChannel(const QString & channelName);

    //------------------------------------------------------------------------------------

private:

    // Appearance settings

    //! Name of the widget style ("Windows", "CDE", "Plastique", etc. for example)
    QString mWidgetStyleName;

    //! True if the widget style uses the style palette,
    //! false if it uses the original palette
    bool mUseWidgetStylePalette;

    //! True if the timeline graphics view uses antialiasing
    bool mUseTimelineAntialiasing;

    //! Background color of the console
    QColor mConsoleBackgroundColor;

    //! Default text color of the console
    QColor mConsoleTextDefaultColor;

    typedef QMap<Pegasus::Core::LogChannel, bool> LogChannelFilterTable;
    typedef QMap<Pegasus::Core::LogChannel, QColor> LogChannelColorTable;

    //! Conversion table between a log channel and an associated filtering state
    //! (contains only the channels that are disabled)
    LogChannelFilterTable mLogChannelFilterTable;

    //! Conversion table between a log channel and an associated color
    //! (contains only the channels that are overridden, not the ones using the text default color)
    LogChannelColorTable mLogChannelColorTable;

    //! list of colors for syntax highlight
    QColor mCodeEditorColorStyles[SYNTAX_COUNT];

    //! font size for Code editor
    int mCodeEditorFontSize;

    //! tab size for Code editor
    int mCodeEditorTabSize;

    //------------------------------------------------------------------------------------

private:

    // Internal variables

    //! Pointer to the main editor window.
    Editor * mEditor;

    //! List of possible widget style names
    QStringList mWidgetStyleNameList;

    //! Original palette of the application
    QPalette mOriginalPalette;
};


#endif  // EDITOR_SETTINGS_H
