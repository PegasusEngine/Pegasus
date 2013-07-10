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

#include <QWidget>

//class ColorPickerBox;
//! \todo Implement ColorPickerBox
typedef QWidget ColorPickerBox;


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

    //! Set the background color of the console to its default
    void SetConsoleDefaultBackgroundColor();

    //! Set the text color of the console to its default
    void SetConsoleDefaultTextColor();

    //------------------------------------------------------------------------------------

private:

    //! Color picker box for the console background color
    ColorPickerBox * mBackgroundColorPickerBox;

    //! Color picker box for the console text color
    ColorPickerBox * mTextColorPickerBox;
};


#endif  // EDITOR_SETTINGSAPPEARANCEPAGE_H
