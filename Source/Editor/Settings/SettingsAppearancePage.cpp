/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsAppearancePage.cpp
//! \author	Karolyn Boulanger
//! \date	10th July 2013
//! \brief	Page to configure the appearance settings of the editor

#include "Settings/SettingsAppearancePage.h"
#include "Editor.h"

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
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

    // Rendering quality group box
    QGroupBox * renderingQualityGroup = new QGroupBox(tr("Rendering quality"));
    QCheckBox * timelineAntialiasingCheck = new QCheckBox(tr("Timeline &antialiasing"));
    timelineAntialiasingCheck->setChecked(settings->IsUsingTimelineAntialiasing());

    QVBoxLayout * renderingQualityLayout = new QVBoxLayout();
    renderingQualityLayout->addWidget(timelineAntialiasingCheck);
    renderingQualityGroup->setLayout(renderingQualityLayout);

    connect(timelineAntialiasingCheck, SIGNAL(stateChanged(int)),
            this, SLOT(SetUseTimelineAntialiasing(int)));

    // Main layout
    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->addWidget(interfaceStyleGroup);
    mainLayout->addWidget(renderingQualityGroup);
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

void SettingsAppearancePage::SetUseTimelineAntialiasing(int state)
{
	Editor::GetSettings()->SetUseTimelineAntialiasing(state == Qt::Checked);
}
