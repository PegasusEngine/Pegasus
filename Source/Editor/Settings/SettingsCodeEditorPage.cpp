/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsCodeEditorPage.cpp
//! \author	Kleber Garcia
//! \date   4rth March 2014	
//! \brief	Page to configure the appearance settings of the Code IDE editor

#include "Settings/SettingsCodeEditorPage.h"
#include "Editor.h"

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSignalMapper>
#include "Widgets/ColorPickerBox.h"

const int gCodeFontSizes[] = {
    8, 9, 10, 11, 12, 13, 14, 18, 22, 24, 32
};

const int gCodeTabSizes[] = {
    2, 3, 4, 5, 6, 8
};

SettingsCodeEditorPage::SettingsCodeEditorPage(QWidget *parent)
:   QWidget(parent)
{
    const Settings * const settings = Editor::GetSettings();
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mColorPickersList = new ColorPickerBox* [Settings::SYNTAX_COUNT];
    mSignalMapper = new QSignalMapper(this);
    mSignalDefaultButtonMapper = new QSignalMapper(this);
    QGroupBox * colorGroup = new QGroupBox(tr("Code Editor Colors")); 
    QGridLayout * colorLayout = new QGridLayout();
    for (unsigned i = 0; i < Settings::SYNTAX_COUNT; ++i)
    {
        mColorPickersList[i] = new ColorPickerBox(settings->GetCodeSyntaxColor(static_cast<Settings::CodeEditorSyntaxStyle>(i)), this);
        mSignalMapper->setMapping(mColorPickersList[i], i);
        
        connect(mColorPickersList[i], SIGNAL(ColorChanged(const QColor &)),
                mSignalMapper, SLOT(map()));    
         
        QLabel * informationLayout = new QLabel(tr(Settings::sCodeEditorSyntaxStyleNames[i]));

        QPushButton * colorDefaultButton = new QPushButton(tr("Set Default"));
        mSignalDefaultButtonMapper->setMapping(colorDefaultButton, i);
        connect(colorDefaultButton, SIGNAL(clicked()),
                mSignalDefaultButtonMapper, SLOT(map()));

        colorLayout->addWidget(informationLayout, i, 0);
        colorLayout->addWidget(mColorPickersList[i], i, 1);
        colorLayout->addWidget(colorDefaultButton, i, 2);
    }

    colorGroup->setLayout(colorLayout); 

    QGroupBox * miscGroup = new QGroupBox(tr("Code Editor Misc"));
    QGridLayout * miscLayout = new QGridLayout();
    QLabel * fontSizeLabel = new QLabel(tr("Font size:"));
    miscLayout->addWidget(fontSizeLabel, 0, 0);
    mFontSizeCombo = new QComboBox(this);
    int currentFontIndex = 0;
    for (int i = 0; i < sizeof(gCodeFontSizes)/sizeof(int); ++i)
    {
        if (gCodeFontSizes[i] == settings->GetCodeEditorFontSize())
        {
            currentFontIndex = i;
        }
        mFontSizeCombo->addItem(QString::number(gCodeFontSizes[i]), i);
    }

    mFontSizeCombo->setCurrentIndex(currentFontIndex);

    connect(mFontSizeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ApplyCodeFontSize(int)));

    miscLayout->addWidget(mFontSizeCombo, 0, 1);

    QLabel * tabSizeLabel = new QLabel(tr("Tab size:"));
    miscLayout->addWidget(tabSizeLabel , 0, 2);
    mTabSizeCombo = new QComboBox(this);

    int currentTabIndex = 0;
    for (int i = 0; i < sizeof(gCodeTabSizes)/sizeof(int); ++i)
    {
        if (gCodeTabSizes[i] == settings->GetCodeEditorTabSize())
        {
            currentTabIndex = i;
        }
        mTabSizeCombo->addItem(QString::number(gCodeTabSizes[i]), i);
    }

    mTabSizeCombo->setCurrentIndex(currentTabIndex);

    connect(mTabSizeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ApplyCodeTabSize(int)));

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

void SettingsCodeEditorPage::ApplyColor(int i)
{
    Settings * settings = Editor::GetSettings();
    settings->SetCodeEditorColor(static_cast<Settings::CodeEditorSyntaxStyle>(i), mColorPickersList[i]->GetColor());
}

//----------------------------------------------------------------------------------------

void SettingsCodeEditorPage::ApplyCodeFontSize(int index)
{
    Settings * settings = Editor::GetSettings();
    settings->SetCodeEditorFontSize(gCodeFontSizes[index]);
}

//----------------------------------------------------------------------------------------

void SettingsCodeEditorPage::ApplyCodeTabSize(int index)
{
    Settings * settings = Editor::GetSettings();
    settings->SetCodeEditorTabSize(gCodeTabSizes[index]);
}

//----------------------------------------------------------------------------------------

void SettingsCodeEditorPage::SetDefaultColor(int i)
{
    Settings * settings = Editor::GetSettings();
    settings->SetCodeEditorColor(static_cast<Settings::CodeEditorSyntaxStyle>(i), Settings::sDefaultSyntaxHighlightColors[i]);
    mColorPickersList[i]->SetColor(Settings::sDefaultSyntaxHighlightColors[i]);
}
//----------------------------------------------------------------------------------------

SettingsCodeEditorPage::~SettingsCodeEditorPage()
{

    delete [] mColorPickersList;
}
