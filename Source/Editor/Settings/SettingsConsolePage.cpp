/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsConsolePage.cpp
//! \author	Kevin Boulanger
//! \date	28th September 2013
//! \brief	Page to configure the console settings of the editor

#include "Settings/SettingsConsolePage.h"
#include "Widgets/ColorPickerBox.h"
#include "Console/ConsoleChannelColorTable/ConsoleChannelColorTableView.h"
#include "Editor.h"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>


SettingsConsolePage::SettingsConsolePage(QWidget *parent)
:   QWidget(parent)
{
    const Settings * const settings = Editor::GetSettings();

    // Console default group box
    QGroupBox * consoleDefaultGroup = new QGroupBox(tr("Default colors"));
    QLabel * backgroundColorLabel = new QLabel(tr("&Background color:"));
    backgroundColorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mConsoleBackgroundColorPickerBox = new ColorPickerBox;
    mConsoleBackgroundColorPickerBox->SetColor(settings->GetConsoleBackgroundColor());
    backgroundColorLabel->setBuddy(mConsoleBackgroundColorPickerBox);
    QPushButton * backgroundColorDefaultButton = new QPushButton(tr("Set default"));
    QLabel * textColorLabel = new QLabel(tr("&Text default color:"));
    mConsoleTextDefaultColorPickerBox = new ColorPickerBox();
    mConsoleTextDefaultColorPickerBox->SetColor(settings->GetConsoleTextDefaultColor());
    textColorLabel->setBuddy(mConsoleTextDefaultColorPickerBox);
    QPushButton * textColorDefaultButton = new QPushButton(tr("Set default"));
    
    QGridLayout * consoleDefaultLayout = new QGridLayout();
    consoleDefaultLayout->addWidget(backgroundColorLabel, 0, 0);
    consoleDefaultLayout->addWidget(mConsoleBackgroundColorPickerBox, 0, 1);
    consoleDefaultLayout->addWidget(backgroundColorDefaultButton, 0, 2);
    consoleDefaultLayout->addWidget(textColorLabel, 1, 0);
    consoleDefaultLayout->addWidget(mConsoleTextDefaultColorPickerBox, 1, 1);
    consoleDefaultLayout->addWidget(textColorDefaultButton, 1, 2);
    consoleDefaultGroup->setLayout(consoleDefaultLayout);

    connect(mConsoleBackgroundColorPickerBox, SIGNAL(ColorChanged(const QColor &)),
            this, SLOT(SetConsoleBackgroundColor(const QColor &)));
    connect(backgroundColorDefaultButton, SIGNAL(clicked()),
            this, SLOT(SetDefaultConsoleBackgroundColor()));

    connect(mConsoleTextDefaultColorPickerBox, SIGNAL(ColorChanged(const QColor &)),
            this, SLOT(SetConsoleTextDefaultColor(const QColor &)));
    connect(textColorDefaultButton, SIGNAL(clicked()),
            this, SLOT(SetDefaultConsoleTextDefaultColor()));

    // Console channel color group box
    QGroupBox * consoleChannelColorGroup = new QGroupBox(tr("Log channel colors"));
    mColorTableView = new ConsoleChannelColorTableView;
    QPushButton * colorTableResetAllButton = new QPushButton(tr("Set all to default"));

    QVBoxLayout * consoleChannelColorVLayout = new QVBoxLayout();
    consoleChannelColorVLayout->addWidget(colorTableResetAllButton);
    consoleChannelColorVLayout->addStretch(1);

    QHBoxLayout * consoleChannelColorHLayout = new QHBoxLayout();
    consoleChannelColorHLayout->addWidget(mColorTableView);
    consoleChannelColorHLayout->addLayout(consoleChannelColorVLayout);
    consoleChannelColorGroup->setLayout(consoleChannelColorHLayout);

    connect(colorTableResetAllButton, SIGNAL(clicked()),
            this, SLOT(ResetAllConsoleTextColors()));

    // Main layout
    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->addWidget(consoleDefaultGroup);
    mainLayout->addWidget(consoleChannelColorGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

//----------------------------------------------------------------------------------------

SettingsConsolePage::~SettingsConsolePage()
{
}

//----------------------------------------------------------------------------------------
	
void SettingsConsolePage::SetConsoleBackgroundColor(const QColor & color)
{
	Editor::GetSettings()->SetConsoleBackgroundColor(color);
}

//----------------------------------------------------------------------------------------
	
void SettingsConsolePage::SetDefaultConsoleBackgroundColor()
{
	mConsoleBackgroundColorPickerBox->SetColor(
                    Editor::GetSettings()->GetConsoleDefaultBackgroundColor());
}

//----------------------------------------------------------------------------------------

void SettingsConsolePage::SetConsoleTextDefaultColor(const QColor & color)
{
	Editor::GetSettings()->SetConsoleTextDefaultColor(color);

    // Refresh the content of the log channel color table view
    mColorTableView->Refresh();
}

//----------------------------------------------------------------------------------------

void SettingsConsolePage::SetDefaultConsoleTextDefaultColor()
{
	mConsoleTextDefaultColorPickerBox->SetColor(
                    Editor::GetSettings()->GetConsoleDefaultTextDefaultColor());
}

//----------------------------------------------------------------------------------------

void SettingsConsolePage::ResetAllConsoleTextColors()
{
    Editor::GetSettings()->SetDefaultConsoleTextColorForAllLogChannels(); 

    // Refresh the content of the log channel color table view
    mColorTableView->Refresh();
}
