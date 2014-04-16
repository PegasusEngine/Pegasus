/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsDialog.h
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Preferences dialog box

#ifndef EDITOR_SETTINGSDIALOG_H
#define EDITOR_SETTINGSDIALOG_H

#include <QDialog>

class QListWidgetItem;
class QListWidget;
class QStackedWidget;


//! Preferences dialog box
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:

    SettingsDialog(QWidget *parent);
    virtual ~SettingsDialog();

    //! List of pages of the settings dialog box
    enum Page
    {
        PAGE_APPEARANCE = 0,
        PAGE_CONSOLE,
        PAGE_SHADER_EDITOR,
        NUM_PAGES
    };

    //! Set the page to display
    //! \param page PAGE_xxx constant, < NUM_PAGES
    void SetCurrentPage(Page page);

    //------------------------------------------------------------------------------------
	
private slots:

    //! Change the page that is displayed in the stacked widget
    //! \param current List item of the current category icon
    //! \param previous List item of the previous category icon
    void ChangePage(QListWidgetItem * current, QListWidgetItem * previous);

    //------------------------------------------------------------------------------------

private:

    //! Create the list of icons for each category of settings
    void CreateIcons();

    //! List of icons for each category of settings
    QListWidget * mIcons;

    //! Stack of settings pages, one for each category of settings
    QStackedWidget * mSettingsPages;
};


#endif  // EDITOR_SETTINGSDIALOG_H
