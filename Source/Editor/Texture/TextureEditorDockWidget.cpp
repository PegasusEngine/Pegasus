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
#include "Pegasus/Texture/Shared/ITextureManagerProxy.h"
#include "Pegasus/Texture/Shared/ITextureConfigurationProxy.h"

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
    mViewportWidget->setMinimumSize(512, 512);
    mViewportWidget->setMaximumSize(512, 512);
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
    Pegasus::Texture::ITextureManagerProxy * textureManagerProxy =
                        Editor::GetInstance().GetApplicationManager().GetApplication()->GetTextureManagerProxy();
    const unsigned int numTextures = textureManagerProxy->GetNumTextures();
    for (unsigned int t = 0; t < numTextures; ++t)
    {
        Pegasus::Texture::ITextureProxy * textureProxy = textureManagerProxy->GetTexture(t);
        OpenTabForTexture(textureProxy);
    }

    //! \todo Temporary, use proper Qt signals to make it happen
    //UpdateTextureProperties();
}

//----------------------------------------------------------------------------------------

void TextureEditorDockWidget::UpdateUIForAppClosed()
{

}

//----------------------------------------------------------------------------------------

void TextureEditorDockWidget::OpenTabForTexture(Pegasus::Texture::ITextureProxy * textureProxy)
{
    if (textureProxy == nullptr)
    {
        ED_FAILSTR("Trying to open a new tab in the texture editor with a null texture");
        return;
    }

    //! \todo Check that the tab has not been opened yet

    const char * textureName = textureProxy->GetName();
    ED_LOG("Opening tab for texture \"%s\" in the texture editor", textureName)

    // Create a graph editor view associated with the texture
    TextureGraphEditorGraphicsView * graphicsView = new TextureGraphEditorGraphicsView(textureProxy);

    // Create a subwindow (tab) and associate the graph editor graphics view with it
    QMdiSubWindow * subWindow = ui.mdiArea->addSubWindow(graphicsView);
    //subWindow->setWidget(graphicsView);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->setWindowTitle(textureName);

    // Show the new tab
    subWindow->show();

    //! \todo **** Temporary, to test UpdateTextureProperties
    UpdateTextureProperties(subWindow);
}

//----------------------------------------------------------------------------------------

void TextureEditorDockWidget::UpdateTextureProperties(QMdiSubWindow * subWindow)
{
    ED_ASSERTSTR(subWindow != nullptr, "Trying to update the texture properties with an invalid current subwindow");
    const TextureGraphEditorGraphicsView * graphicsView = static_cast<TextureGraphEditorGraphicsView *>(subWindow->widget());
    ED_ASSERTSTR(graphicsView != nullptr, "Trying to update the texture properties with an invalid current graphics view");
    const Pegasus::Texture::ITextureProxy * textureProxy = graphicsView->GetTextureProxy();
    ED_ASSERTSTR(textureProxy != nullptr, "Trying to update the texture properties with an invalid current texture");
    const Pegasus::Texture::ITextureConfigurationProxy * textureConfigurationProxy = textureProxy->GetConfiguration();
    ED_ASSERTSTR(textureConfigurationProxy != nullptr, "Trying to update the texture properties with an invalid current texture configuration");
    
    ui.resolutionValueLabel->setText(QString("%1 x %2 x %3").arg(textureConfigurationProxy->GetWidth())
                                                            .arg(textureConfigurationProxy->GetHeight())
                                                            .arg(textureConfigurationProxy->GetDepth()));
    ui.layersValueLabel->setText(QString("%1").arg(textureConfigurationProxy->GetNumLayers()));
}
