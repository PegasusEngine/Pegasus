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

const int gShaderFontSizes[] = {
    8, 9, 10, 11, 12, 13, 14, 18, 22, 24, 32
};

const int gShaderTabSizes[] = {
    2, 3, 4, 5, 6, 8
};

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

    QGroupBox * miscGroup = new QGroupBox(tr("Shader Editor Misc"));
    QGridLayout * miscLayout = new QGridLayout();
    QLabel * fontSizeLabel = new QLabel(tr("Font size:"));
    miscLayout->addWidget(fontSizeLabel, 0, 0);
    mFontSizeCombo = new QComboBox(this);
    int currentFontIndex = 0;
    for (int i = 0; i < sizeof(gShaderFontSizes)/sizeof(int); ++i)
    {
        if (gShaderFontSizes[i] == settings->GetShaderEditorFontSize())
        {
            currentFontIndex = i;
        }
        mFontSizeCombo->addItem(QString::number(gShaderFontSizes[i]), i);
    }

    mFontSizeCombo->setCurrentIndex(currentFontIndex);

    connect(mFontSizeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ApplyShaderFontSize(int)));

    miscLayout->addWidget(mFontSizeCombo, 0, 1);

    QLabel * tabSizeLabel = new QLabel(tr("Tab size:"));
    miscLayout->addWidget(tabSizeLabel , 0, 2);
    mTabSizeCombo = new QComboBox(this);

    int currentTabIndex = 0;
    for (int i = 0; i < sizeof(gShaderTabSizes)/sizeof(int); ++i)
    {
        if (gShaderTabSizes[i] == settings->GetShaderEditorTabSize())
        {
            currentTabIndex = i;
        }
        mTabSizeCombo->addItem(QString::number(gShaderTabSizes[i]), i);
    }

    mTabSizeCombo->setCurrentIndex(currentTabIndex);

    connect(mTabSizeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ApplyShaderTabSize(int)));

    miscLayout->addWidget(mTabSizeCombo , 0, 3);
    
    miscGroup->setLayout(miscLayout);

    connect(mSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(ApplyColor(int)));

    connect(mSignalDefaultButtonMapper, SIGNAL(mapped(int)),
            this, SLOT(SetDefaultColor(int)));
    
    mainLayout->addWidget(colorGroup);
    mainLayout->addWidget(miscGroup);

    setLayout(mainLayout);

}

//----------------------------------------------------------------------------------------

void SettingsShaderEditorPage::ApplyColor(int i)
{
    Settings * settings = Editor::GetSettings();
    settings->SetShaderEditorColor(static_cast<Settings::ShaderEditorSyntaxStyle>(i), mColorPickersList[i]->GetColor());
}

//----------------------------------------------------------------------------------------

void SettingsShaderEditorPage::ApplyShaderFontSize(int index)
{
    Settings * settings = Editor::GetSettings();
    settings->SetShaderEditorFontSize(gShaderFontSizes[index]);
}

//----------------------------------------------------------------------------------------

void SettingsShaderEditorPage::ApplyShaderTabSize(int index)
{
    Settings * settings = Editor::GetSettings();
    settings->SetShaderEditorTabSize(gShaderTabSizes[index]);
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
