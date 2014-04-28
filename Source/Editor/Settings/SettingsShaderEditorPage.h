/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsShaderEditorPage.h
//! \author	Kleber Garcia
//! \date	4rth March 2014
//! \brief	Page to configure the appearance settings of the shader IDE editor

#ifndef EDITOR_SETTINGSSHADEREDITORPAGE_H
#define EDITOR_SETTINGSSHADEREDITORPAGE_H

class ColorPickerBox;
class QSignalMapper;
class QComboBox;


//! Page to configure the appearance settings of the shader IDE editor
class SettingsShaderEditorPage : public QWidget
{
    Q_OBJECT

public:

    SettingsShaderEditorPage(QWidget *parent);
    virtual ~SettingsShaderEditorPage();

private slots:
    //! Slot called when the color from a color widget is set
    //! \param index of the color widget to copy color down the application
    void ApplyColor(int i);

    //! Slot called when the the color is set to default
    //! \param index of the default color of the syntax highlight
    void SetDefaultColor(int i);

    //! Slot called when the font size is changed
    //! \param font size
    void ApplyShaderFontSize(int i);

    //! Slot called when the tabstop size is changed
    //! \param the actual tab stop size
    void ApplyShaderTabSize(int i);

private:
    ColorPickerBox ** mColorPickersList;
    QSignalMapper * mSignalMapper;
    QSignalMapper * mSignalDefaultButtonMapper;
    QComboBox * mFontSizeCombo;
    QComboBox * mTabSizeCombo;
};


#endif  // EDITOR_SETTINGSAPPEARANCEPAGE_H
