/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEditorDockWidget.cpp
//! \author	Kevin Boulanger
//! \date	05th June 2014
//! \brief	Dock widget for the texture editor

#include "Texture/TextureEditorDockWidget.h"
#include "Texture/TextureGraphEditorGraphicsView.h"
#include "Viewport/ViewportWidget.h"

//! \todo Temporary
#include "Editor.h"
#include "Application/ApplicationManager.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Texture/Shared/ITextureProxy.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>


TextureEditorDockWidget::TextureEditorDockWidget(QWidget *parent)
:   QDockWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("Texture Editor"));
	setObjectName("TextureEditorDockWidget");
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

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

    // Create the viewport widget that will contain the previewer
    mViewportWidget = new ViewportWidget(VIEWPORTTYPE_TEXTURE_EDITOR_PREVIEW, ui.mainWidget);
    mViewportWidget->setMinimumSize(256, 256);
    mViewportWidget->setMaximumSize(256, 256);
    ui.propertiesVerticalLayout->insertWidget(0, mViewportWidget);

    // Connect the Refresh button to the graph changed message
    //! \todo Temporary. Needs to emit the signal when the graph actually changes
    connect(ui.refreshButton, SIGNAL(clicked()),
            this, SIGNAL(GraphChanged()));
}

//----------------------------------------------------------------------------------------

TextureEditorDockWidget::~TextureEditorDockWidget()
{
}

//----------------------------------------------------------------------------------------

void TextureEditorDockWidget::UpdateUIForAppLoaded()
{
    //! \todo Temporary code to load the list of textures and create a tab for each
    static Pegasus::Texture::ITextureProxy * textureProxies[16] = { nullptr };
    const unsigned int numTextures = Editor::GetInstance().GetApplicationManager().GetApplication()->GetApplicationProxy()
                                            ->GetTextures(textureProxies);
    for (unsigned int t = 0; t < numTextures; ++t)
    {
        TextureGraphEditorGraphicsView * graphicsView = new TextureGraphEditorGraphicsView;
        QMdiSubWindow * subWindow = ui.mdiArea->addSubWindow(graphicsView);
        subWindow->setWidget(graphicsView);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        subWindow->setWindowTitle(textureProxies[t]->GetName());
        subWindow->show();
    }
}

//----------------------------------------------------------------------------------------

void TextureEditorDockWidget::UpdateUIForAppClosed()
{

}