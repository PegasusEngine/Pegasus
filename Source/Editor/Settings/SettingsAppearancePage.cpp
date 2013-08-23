/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsAppearancePage.cpp
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Page to configure the appearance settings of the editor

#include "Settings/SettingsAppearancePage.h"
#include "Widgets/ColorPickerBox.h"
#include "Console/ConsoleChannelColorTable/ConsoleChannelColorTableView.h"
#include "Editor.h"

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>


SettingsAppearancePage::SettingsAppearancePage(QWidget *parent)
:   QWidget(parent)
{
    const Settings * const settings = Editor::GetSettings();

    // Interface style group box
    QGroupBox * interfaceStyleGroup = new QGroupBox(tr("Interface style"));
    QLabel * styleLabel = new QLabel(tr("&Style:"));
    QComboBox * styleCombo = new QComboBox();
    foreach (QString styleName, settings->GetWidgetStyleNameList())
    {
        styleCombo->addItem(styleName);
    }
    int styleIndex = styleCombo->findText(settings->GetWidgetStyleName());
    if (styleIndex >= 0)
    {
        styleCombo->setCurrentIndex(styleIndex);
    }
    styleLabel->setBuddy(styleCombo);
    QCheckBox * useStylePaletteCheck = new QCheckBox(tr("Use style &palette"));
    useStylePaletteCheck->setChecked(settings->IsUsingWidgetStylePalette());

    QGridLayout * interfaceStyleLayout = new QGridLayout();
    interfaceStyleLayout->addWidget(styleLabel, 0, 0);
    interfaceStyleLayout->addWidget(styleCombo, 0, 1);
    interfaceStyleLayout->addWidget(useStylePaletteCheck, 1, 0, 1, -1);
    interfaceStyleGroup->setLayout(interfaceStyleLayout);

    connect(styleCombo, SIGNAL(currentIndexChanged(const QString &)),
            settings, SLOT(SetWidgetStyleName(const QString &)));
    connect(useStylePaletteCheck, SIGNAL(stateChanged(int)),
            this, SLOT(SetUseWidgetStylePalette(int)));

    // Console group box
    QGroupBox * consoleGroup = new QGroupBox(tr("Console"));
    QLabel * backgroundColorLabel = new QLabel(tr("&Background color:"));
    mConsoleBackgroundColorPickerBox = new ColorPickerBox;
    mConsoleBackgroundColorPickerBox->SetColor(settings->GetConsoleBackgroundColor());
    backgroundColorLabel->setBuddy(mConsoleBackgroundColorPickerBox);
    QPushButton * backgroundColorDefaultButton = new QPushButton(tr("Set default"));
    QLabel * textColorLabel = new QLabel(tr("&Text default color:"));
    mConsoleTextDefaultColorPickerBox = new ColorPickerBox();
    mConsoleTextDefaultColorPickerBox->SetColor(settings->GetConsoleTextDefaultColor());
    textColorLabel->setBuddy(mConsoleTextDefaultColorPickerBox);
    QPushButton * textColorDefaultButton = new QPushButton(tr("Set default"));
    mColorTableView = new ConsoleChannelColorTableView;
    QPushButton * colorTableResetAllButton = new QPushButton(tr("Set all to default"));
    
    QGridLayout * consoleLayout = new QGridLayout();
    consoleLayout->addWidget(backgroundColorLabel, 0, 0);
    consoleLayout->addWidget(mConsoleBackgroundColorPickerBox, 0, 1);
    consoleLayout->addWidget(backgroundColorDefaultButton, 0, 2);
    consoleLayout->addWidget(textColorLabel, 1, 0);
    consoleLayout->addWidget(mConsoleTextDefaultColorPickerBox, 1, 1);
    consoleLayout->addWidget(textColorDefaultButton, 1, 2);
    consoleLayout->addWidget(mColorTableView, 2, 0);
    consoleLayout->addWidget(colorTableResetAllButton, 2, 2);
    consoleGroup->setLayout(consoleLayout);

    connect(mConsoleBackgroundColorPickerBox, SIGNAL(ColorChanged(const QColor &)),
            this, SLOT(SetConsoleBackgroundColor(const QColor &)));
    connect(backgroundColorDefaultButton, SIGNAL(clicked()),
            this, SLOT(SetDefaultConsoleBackgroundColor()));

    connect(mConsoleTextDefaultColorPickerBox, SIGNAL(ColorChanged(const QColor &)),
            this, SLOT(SetConsoleTextDefaultColor(const QColor &)));
    connect(textColorDefaultButton, SIGNAL(clicked()),
            this, SLOT(SetDefaultConsoleTextDefaultColor()));

    connect(colorTableResetAllButton, SIGNAL(clicked()),
            this, SLOT(ResetAllConsoleTextColors()));

    // Main layout
    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->addWidget(interfaceStyleGroup);
    mainLayout->addWidget(consoleGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

//----------------------------------------------------------------------------------------

SettingsAppearancePage::~SettingsAppearancePage()
{
}

//----------------------------------------------------------------------------------------

void SettingsAppearancePage::SetUseWidgetStylePalette(int state)
{
	Editor::GetSettings()->SetUseWidgetStylePalette(state == Qt::Checked);
}

//----------------------------------------------------------------------------------------
	
void SettingsAppearancePage::SetConsoleBackgroundColor(const QColor & color)
{
	Editor::GetSettings()->SetConsoleBackgroundColor(color);
}

//----------------------------------------------------------------------------------------
	
void SettingsAppearancePage::SetDefaultConsoleBackgroundColor()
{
	mConsoleBackgroundColorPickerBox->SetColor(
                    Editor::GetSettings()->GetConsoleDefaultBackgroundColor());
}

//----------------------------------------------------------------------------------------

void SettingsAppearancePage::SetConsoleTextDefaultColor(const QColor & color)
{
	Editor::GetSettings()->SetConsoleTextDefaultColor(color);

    // Refresh the content of the log channel color table view
    mColorTableView->Refresh();
}

//----------------------------------------------------------------------------------------

void SettingsAppearancePage::SetDefaultConsoleTextDefaultColor()
{
	mConsoleTextDefaultColorPickerBox->SetColor(
                    Editor::GetSettings()->GetConsoleDefaultTextDefaultColor());
}

//----------------------------------------------------------------------------------------

void SettingsAppearancePage::ResetAllConsoleTextColors()
{
    Editor::GetSettings()->SetDefaultConsoleTextColorForAllLogChannels(); 

    // Refresh the content of the log channel color table view
    mColorTableView->Refresh();
}
