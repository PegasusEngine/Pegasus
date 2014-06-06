/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEditorDockWidget.cpp
//! \author	Kevin Boulanger
//! \date	05th June 2014
//! \brief	Dock widget for the texture editor

#include "TextureEditorDockWidget.h"
#include "TextureGraphEditorGraphicsView.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>


TextureEditorDockWidget::TextureEditorDockWidget(QWidget *parent)
:   QDockWidget(parent)
{
    ui.setupUi(this);

    // Create the menu bar
    //! \todo Use proper actions
    QMenuBar * menuBar = new QMenuBar(ui.mainWidget);
    ui.mainVerticalLayout->setMenuBar(menuBar);
    QMenu * graphMenu = menuBar->addMenu(tr("Graph"));
    graphMenu->addAction(ui.actionTest1);
    graphMenu->addAction(ui.actionTest1_2);

    // Create the top toolbar, and place it first in the layout
    QToolBar *topToolBar = new QToolBar(this);
    //! \todo Use proper actions
    topToolBar->addAction(ui.actionTest1);
    topToolBar->addAction(ui.actionTest1_2);
    ui.mainVerticalLayout->insertWidget(0, topToolBar);

    // Create the left toolbar, and place it first in the layout
    QToolBar * leftToolBar = new QToolBar(this);
    //! \todo Use proper actions
    leftToolBar->addAction(ui.actionTest1);
    leftToolBar->addAction(ui.actionTest1_2);
    leftToolBar->setOrientation(Qt::Vertical);
    ui.mainHorizontalLayout->insertWidget(0, leftToolBar);

    //! \todo Replace those temporary buttons by graph editors
    TextureGraphEditorGraphicsView * button1 = new TextureGraphEditorGraphicsView(this);
    TextureGraphEditorGraphicsView * button2 = new TextureGraphEditorGraphicsView(this);
    QMdiSubWindow *subWindow1 = new QMdiSubWindow;
    subWindow1->setWidget(button1);
    subWindow1->setAttribute(Qt::WA_DeleteOnClose);
    subWindow1->setWindowTitle("Test1");
    ui.mdiArea->addSubWindow(subWindow1);
    QMdiSubWindow *subWindow2 = new QMdiSubWindow;
    subWindow2->setWidget(button2);
    subWindow2->setAttribute(Qt::WA_DeleteOnClose);
    subWindow2->setWindowTitle("Test2");
    ui.mdiArea->addSubWindow(subWindow2);

    //! \todo Replace the black background of the frame
    //!       by the texture preview
    //! \todo Disable auto fill background in Qt Designer
    QPalette pal(ui.texturePreviewFrame->palette());
    pal.setColor(QPalette::Background, Qt::black);
    ui.texturePreviewFrame->setPalette(pal);
}

//----------------------------------------------------------------------------------------

TextureEditorDockWidget::~TextureEditorDockWidget()
{
}
