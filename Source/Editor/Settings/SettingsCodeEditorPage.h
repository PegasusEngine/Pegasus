/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsCodeEditorPage.h
//! \author	Kleber Garcia
//! \date	4rth March 2014
//! \brief	Page to configure the appearance settings of the Code IDE editor

#ifndef EDITOR_SETTINGSCODEEDITORPAGE_H
#define EDITOR_SETTINGSCODEEDITORPAGE_H

class ColorPickerBox;
class QSignalMapper;
class QComboBox;


//! Page to configure the appearance settings of the Code IDE editor
class SettingsCodeEditorPage : public QWidget
{
    Q_OBJECT

public:

    SettingsCodeEditorPage(QWidget *parent);
    virtual ~SettingsCodeEditorPage();

private slots:
    //! Slot called when the color from a color widget is set
    //! \param index of the color widget to copy color down the application
    void ApplyColor(int i);

    //! Slot called when the the color is set to default
    //! \param index of the default color of the syntax highlight
    void SetDefaultColor(int i);

    //! Slot called when the font size is changed
    //! \param font size
    void ApplyCodeFontSize(int i);

    //! Slot called when the tabstop size is changed
    //! \param the actual tab stop size
    void ApplyCodeTabSize(int i);

private:
    ColorPickerBox ** mColorPickersList;
    QSignalMapper * mSignalMapper;
    QSignalMapper * mSignalDefaultButtonMapper;
    QComboBox * mFontSizeCombo;
    QComboBox * mTabSizeCombo;
};


#endif  // EDITOR_SETTINGSAPPEARANCEPAGE_H
