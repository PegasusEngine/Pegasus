/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsAppearancePage.cpp
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Page to configure the appearance settings of the editor

#include "SettingsAppearancePage.h"
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
    Settings * settings = Editor::GetSettings();

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
    mBackgroundColorPickerBox = new ColorPickerBox;
    //mBackgroundColorPickerBox->SetColor(settings->GetConsoleBackgroundColor());
        //! \todo Implement ColorPickerBox
    backgroundColorLabel->setBuddy(mBackgroundColorPickerBox);
    QPushButton * backgroundColorDefaultButton = new QPushButton(tr("Set default"));
    QLabel * textColorLabel = new QLabel(tr("&Text color:"));
    mTextColorPickerBox = new ColorPickerBox();
    //mTextColorPickerBox->SetColor(settings->GetConsoleTextColor());
        //! \todo Implement ColorPickerBox
    textColorLabel->setBuddy(mTextColorPickerBox);
    QPushButton * textColorDefaultButton = new QPushButton(tr("Set default"));

    QGridLayout * consoleLayout = new QGridLayout();
    consoleLayout->addWidget(backgroundColorLabel, 0, 0);
    consoleLayout->addWidget(mBackgroundColorPickerBox, 0, 1);
    consoleLayout->addWidget(backgroundColorDefaultButton, 0, 2);
    consoleLayout->addWidget(textColorLabel, 1, 0);
    consoleLayout->addWidget(mTextColorPickerBox, 1, 1);
    consoleLayout->addWidget(textColorDefaultButton, 1, 2);
    consoleGroup->setLayout(consoleLayout);

    connect(mBackgroundColorPickerBox, SIGNAL(ColorChanged(const QColor &)),
            settings, SLOT(SetConsoleBackgroundColor(const QColor &)));
    connect(backgroundColorDefaultButton, SIGNAL(clicked()),
            this, SLOT(SetConsoleDefaultBackgroundColor()));

    connect(mTextColorPickerBox, SIGNAL(ColorChanged(const QColor &)),
            settings, SLOT(SetConsoleTextColor(const QColor &)));
    connect(textColorDefaultButton, SIGNAL(clicked()),
            this, SLOT(SetConsoleDefaultTextColor()));

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
	
void SettingsAppearancePage::SetConsoleDefaultBackgroundColor()
{
    //! \todo Implement ColorPickerBox
	//mBackgroundColorPickerBox->SetColor(
	//		Editor::GetSettings()->GetConsoleDefaultBackgroundColor());
}

//----------------------------------------------------------------------------------------

void SettingsAppearancePage::SetConsoleDefaultTextColor()
{
    //! \todo Implement ColorPickerBox
	//mTextColorPickerBox->SetColor(
	//		Editor::GetSettings()->GetConsoleDefaultTextColor());
}
