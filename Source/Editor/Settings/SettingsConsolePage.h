/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsConsolePage.h
//! \author	Kevin Boulanger
//! \date	28th September 2013
//! \brief	Page to configure the console settings of the editor

#ifndef EDITOR_SETTINGSCONSOLEPAGE_H
#define EDITOR_SETTINGSCONSOLEPAGE_H

class ColorPickerBox;
class ConsoleChannelColorTableView ;


//! Page to configure the console settings of the editor
class SettingsConsolePage : public QWidget
{
    Q_OBJECT

public:

    SettingsConsolePage(QWidget *parent);
    ~SettingsConsolePage();

    //------------------------------------------------------------------------------------

private slots:

    //! Set the background color of the console
    //! \param color New background color of the console
    void SetConsoleBackgroundColor(const QColor & color);

    //! Set the background color of the console to its default
    void SetDefaultConsoleBackgroundColor();

    //! Set the default text color of the console
    //! \param color New default text color of the console
    void SetConsoleTextDefaultColor(const QColor & color);

    //! Set the default text color of the console to its default
    void SetDefaultConsoleTextDefaultColor();

    //! Reset all the text colors for the channels to their defaults
    void ResetAllConsoleTextColors();

    //------------------------------------------------------------------------------------

private:

    //! Color picker box for the console background color
    ColorPickerBox * mConsoleBackgroundColorPickerBox;

    //! Color picker box for the console text color
    ColorPickerBox * mConsoleTextDefaultColorPickerBox;

    //! Table view for the text color associated with the log channels
    ConsoleChannelColorTableView * mColorTableView;
};


#endif  // EDITOR_SETTINGSCONSOLEPAGE_H
