/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportDockWidget.cpp
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the application viewports

#include "Viewport/ViewportDockWidget.h"
#include "Viewport/ViewportWidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>


ViewportDockWidget::ViewportDockWidget(Editor* editor, QWidget * parent, const char* title, const char* objName)
    :   PegasusDockWidget(parent, editor), mTitle(title), mObjName(objName)
{
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::SetupUi()
{
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

    // Create the main widget of the dock, covering the entire child area
    QWidget * mainWidget = new QWidget(this);
    setWidget(mainWidget);

    // Create the layout that will contain the menu bar and the ViewportWidget
    QVBoxLayout * verticalLayout = new QVBoxLayout(mainWidget);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    // Create the menu bar
    QMenuBar * menuBar = CreateMenu(mainWidget);

    // Create the viewport widget that will contain the renderer
    mViewportWidget = new ViewportWidget(mainWidget, Pegasus::App::COMPONENT_FLAG_DEBUG_TEXT | Pegasus::App::COMPONENT_FLAG_WORLD);

    // Set the elements of the layout
    verticalLayout->setMenuBar(menuBar);
    verticalLayout->addWidget(mViewportWidget);
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application)
{
    mViewportWidget->OnAppLoaded();
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::OnUIForAppClosed()
{
    mViewportWidget->OnAppUnloaded();
}

//----------------------------------------------------------------------------------------

ViewportDockWidget::~ViewportDockWidget()
{
    delete mViewportWidget;
}

//----------------------------------------------------------------------------------------

QMenuBar * ViewportDockWidget::CreateMenu(QWidget * mainWidget)
{
    // Create the actions

    QAction * actionModeWireframe = new QAction(tr("Wireframe"), this);
	//actionModeWireframe->setIcon(QIcon(":/Toolbar/File/OpenApp24.png"));
    actionModeWireframe->setStatusTip(tr("Wireframe mode"));
    //connect(actionModeWireframe, SIGNAL(triggered()), this, SLOT(SetModeWireframe()));
    
    QAction * actionModeFilled = new QAction(tr("Filled"), this);
	//actionModeFilled->setIcon(QIcon(":/Toolbar/File/OpenApp24.png"));
	actionModeFilled->setStatusTip(tr("Filled mode"));
    //connect(actionModeFilled, SIGNAL(triggered()), this, SLOT(SetModeFilled()));

    // Create the menu bar and its menus

    QMenuBar * menuBar = new QMenuBar(mainWidget);

    QMenu * modeMenu = menuBar->addMenu(tr("Mode"));
    modeMenu->addAction(actionModeWireframe);
    modeMenu->addAction(actionModeFilled);

    return menuBar;
}


