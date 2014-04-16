/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsShaderEditorPage.cpp
//! \author	Kleber Garcia
//! \date   4rth March 2014	
//! \brief	Page to configure the appearance settings of the shader IDE editor

#include "Settings/SettingsShaderEditorPage.h"
#include "Editor.h"

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSignalMapper>
#include "Widgets/ColorPickerBox.h"

SettingsShaderEditorPage::SettingsShaderEditorPage(QWidget *parent)
:   QWidget(parent)
{
    const Settings * const settings = Editor::GetSettings();
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mColorPickersList = new ColorPickerBox* [Settings::SYNTAX_COUNT];
    mSignalMapper = new QSignalMapper(this);
    mSignalDefaultButtonMapper = new QSignalMapper(this);
    QGroupBox * colorGroup = new QGroupBox(tr("Shader Editor Colors")); 
    QGridLayout * colorLayout = new QGridLayout();
    for (unsigned i = 0; i < Settings::SYNTAX_COUNT; ++i)
    {
        mColorPickersList[i] = new ColorPickerBox(settings->GetShaderSyntaxColor(static_cast<Settings::ShaderEditorSyntaxStyle>(i)), this);
        mSignalMapper->setMapping(mColorPickersList[i], i);
        
        connect(mColorPickersList[i], SIGNAL(ColorChanged(const QColor &)),
                mSignalMapper, SLOT(map()));    
         
        QLabel * informationLayout = new QLabel(tr(Settings::sShaderEditorSyntaxStyleNames[i]));

        QPushButton * colorDefaultButton = new QPushButton(tr("Set Default"));
        mSignalDefaultButtonMapper->setMapping(colorDefaultButton, i);
        connect(colorDefaultButton, SIGNAL(clicked()),
                mSignalDefaultButtonMapper, SLOT(map()));

        colorLayout->addWidget(informationLayout, i, 0);
        colorLayout->addWidget(mColorPickersList[i], i, 1);
        colorLayout->addWidget(colorDefaultButton, i, 2);
    }

    colorGroup->setLayout(colorLayout); 
    connect(mSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(ApplyColor(int)));

    connect(mSignalDefaultButtonMapper, SIGNAL(mapped(int)),
            this, SLOT(SetDefaultColor(int)));
    
    mainLayout->addWidget(colorGroup);

    setLayout(mainLayout);

}

//----------------------------------------------------------------------------------------

void SettingsShaderEditorPage::ApplyColor(int i)
{
    Settings * settings = Editor::GetSettings();
    settings->SetShaderEditorColor(static_cast<Settings::ShaderEditorSyntaxStyle>(i), mColorPickersList[i]->GetColor());
}

//----------------------------------------------------------------------------------------

void SettingsShaderEditorPage::SetDefaultColor(int i)
{
    Settings * settings = Editor::GetSettings();
    settings->SetShaderEditorColor(static_cast<Settings::ShaderEditorSyntaxStyle>(i), Settings::sDefaultSyntaxHighlightColors[i]);
    mColorPickersList[i]->SetColor(Settings::sDefaultSyntaxHighlightColors[i]);
}
//----------------------------------------------------------------------------------------

SettingsShaderEditorPage::~SettingsShaderEditorPage()
{

    delete [] mColorPickersList;
}
