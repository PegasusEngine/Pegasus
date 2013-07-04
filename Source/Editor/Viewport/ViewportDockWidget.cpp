/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportDockWidget.cpp
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the application viewports

#include "Viewport/ViewportDockWidget.h"
#include "Viewport/ViewportWidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>


ViewportDockWidget::ViewportDockWidget(QWidget *parent)
    : QDockWidget(parent)
{
    // Set the dock widget parameters
    setWindowTitle(tr("Viewport"));
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

    // Create the viewport widget that will contain the renderer and set its size policy
    mViewportWidget = new ViewportWidget(mainWidget);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mViewportWidget->sizePolicy().hasHeightForWidth());
    mViewportWidget->setSizePolicy(sizePolicy);
    mViewportWidget->setMinimumSize(QSize(128, 128));

    // Set the elements of the layout
    verticalLayout->setMenuBar(menuBar);
    verticalLayout->addWidget(mViewportWidget);
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