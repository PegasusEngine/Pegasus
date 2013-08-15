/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsAppearancePage.h
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Page to configure the appearance settings of the editor

#ifndef EDITOR_SETTINGSAPPEARANCEPAGE_H
#define EDITOR_SETTINGSAPPEARANCEPAGE_H

class ColorPickerBox;
class ConsoleChannelColorTableView ;


//! Page to configure the appearance settings of the editor
class SettingsAppearancePage : public QWidget
{
    Q_OBJECT

public:

    SettingsAppearancePage(QWidget *parent);
    ~SettingsAppearancePage();

    //------------------------------------------------------------------------------------

private slots:

    //! Called when the "use style palette" checkbox state has changed
    //! \param state Qt::Checked or Qt::Unchecked
    void SetUseWidgetStylePalette(int state);

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

    //------------------------------------------------------------------------------------

private:

    //! Color picker box for the console background color
    ColorPickerBox * mConsoleBackgroundColorPickerBox;

    //! Color picker box for the console text color
    ColorPickerBox * mConsoleTextDefaultColorPickerBox;

    //! Table view for the text color associated with the log channels
    ConsoleChannelColorTableView * mColorTableView;
};


#endif  // EDITOR_SETTINGSAPPEARANCEPAGE_H
