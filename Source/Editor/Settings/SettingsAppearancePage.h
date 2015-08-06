/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsAppearancePage.h
//! \author	Karolyn Boulanger
//! \date	10th July 2013
//! \brief	Page to configure the appearance settings of the editor

#ifndef EDITOR_SETTINGSAPPEARANCEPAGE_H
#define EDITOR_SETTINGSAPPEARANCEPAGE_H


//! Page to configure the appearance settings of the editor
class SettingsAppearancePage : public QWidget
{
    Q_OBJECT

public:

    SettingsAppearancePage(QWidget *parent);
    virtual ~SettingsAppearancePage();

    //------------------------------------------------------------------------------------

private slots:

    //! Called when the "use style palette" checkbox state has changed
    //! \param state Qt::Checked or Qt::Unchecked
    void SetUseWidgetStylePalette(int state);

    //! Called when the "use timeline antialiasing" checkbox state has changed
    //! \param state Qt::Checked or Qt::Unchecked
    void SetUseTimelineAntialiasing(int state);
};


#endif  // EDITOR_SETTINGSAPPEARANCEPAGE_H
