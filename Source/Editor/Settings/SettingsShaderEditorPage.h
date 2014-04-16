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

//! Page to configure the appearance settings of the shader IDE editor
class SettingsShaderEditorPage : public QWidget
{
    Q_OBJECT

public:

    SettingsShaderEditorPage(QWidget *parent);
    virtual ~SettingsShaderEditorPage();

private slots:
    void ApplyColor(int i);
    void SetDefaultColor(int i);

private:
    ColorPickerBox ** mColorPickersList;
    QSignalMapper * mSignalMapper;
    QSignalMapper * mSignalDefaultButtonMapper;
};


#endif  // EDITOR_SETTINGSAPPEARANCEPAGE_H
