/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	SettingsDialog.cpp
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Preferences dialog box

#include "Settings/SettingsDialog.h"
#include "Settings/SettingsAppearancePage.h"
#include "Settings/SettingsConsolePage.h"

#include <QListWidgetItem>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


SettingsDialog::SettingsDialog(QWidget *parent)
:   QDialog(parent),
    mIcons(nullptr),
    mSettingsPages(nullptr)
{
    // Create the list of icons for the pages
    mIcons = new QListWidget();
    mIcons->setViewMode(QListView::IconMode);
    mIcons->setFlow(QListView::TopToBottom);
    mIcons->setIconSize(QSize(64, 64));
    mIcons->setMovement(QListView::Static);
    mIcons->setMaximumWidth(128);
    mIcons->setSpacing(12);
    CreateIcons();

    // Create the stack of pages
    mSettingsPages = new QStackedWidget();
    mSettingsPages->addWidget(new SettingsAppearancePage(this));
    mSettingsPages->addWidget(new SettingsConsolePage(this));
    mIcons->setCurrentRow(PAGE_APPEARANCE);

    // Close button
    QPushButton * closeButton = new QPushButton(tr("Close"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout * horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(mIcons);
    horizontalLayout->addWidget(mSettingsPages, 1);

    QHBoxLayout * buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addSpacing(8);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Pegasus Editor Preferences"));
}

//----------------------------------------------------------------------------------------

SettingsDialog::~SettingsDialog()
{

}

//----------------------------------------------------------------------------------------

void SettingsDialog::SetCurrentPage(Page page)
{
    if (page < NUM_PAGES)
    {
        mIcons->setCurrentRow(static_cast<int>(page));
    }
    else
    {
        ED_FAILSTRF("Invalid page (%d) for the settings dialog box. It should be < %d.", page, NUM_PAGES);
    }
}

//----------------------------------------------------------------------------------------

void SettingsDialog::ChangePage(QListWidgetItem * current, QListWidgetItem * previous)
{
    if (current == nullptr)
    {
        current = previous;
    }

    mSettingsPages->setCurrentIndex(mIcons->row(current));
}

//----------------------------------------------------------------------------------------

void SettingsDialog::CreateIcons()
{
    QListWidgetItem * appearanceButton = new QListWidgetItem(mIcons);
    appearanceButton->setIcon(QIcon(":/SettingsDialog/Appearance.png"));
    appearanceButton->setText(tr("Appearance"));
    appearanceButton->setTextAlignment(Qt::AlignHCenter);
    appearanceButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem * consoleButton = new QListWidgetItem(mIcons);
    consoleButton->setIcon(QIcon(":/SettingsDialog/Console.png"));
    consoleButton->setText(tr("Console"));
    consoleButton->setTextAlignment(Qt::AlignHCenter);
    consoleButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(mIcons, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(ChangePage(QListWidgetItem *, QListWidgetItem*)));
}
