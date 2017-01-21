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

#include <QToolBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSignalMapper>


ViewportDockWidget::ViewportDockWidget(Editor* editor, QWidget * parent, const char* title, const char* objName)
    :   PegasusDockWidget(parent, editor), mTitle(title), mObjName(objName), mUpdaterThread(nullptr), mSpinRenderEnabled(false)
{
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::SetupUi()
{
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);
    
    mButtonMapper = new QSignalMapper(this);
    connect(mButtonMapper, SIGNAL(mapped(int)), this, SLOT(OnSetButtonProperty(int)));

    // Create the main widget of the dock, covering the entire child area
    QWidget * mainWidget = new QWidget(this);
    setWidget(mainWidget);

    // Create the layout that will contain the menu bar and the ViewportWidget
    QVBoxLayout * verticalLayout = new QVBoxLayout(mainWidget);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    // Create the menu bar
    QToolBar * menuBar = CreateMenu(mainWidget);

    // Create the viewport widget that will contain the renderer
    mViewportWidget = new ViewportWidget(mainWidget, Pegasus::App::COMPONENT_FLAG_GRID | Pegasus::App::COMPONENT_FLAG_WORLD | Pegasus::App::COMPONENT_FLAG_DEBUG_CAMERA | Pegasus::App::COMPONENT_FLAG_TERRAIN3D);
    connect(mViewportWidget, SIGNAL(OnWindowProxyReady()), this, SLOT(OnWindowProxyReady()));

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

void ViewportDockWidget::OnWindowProxyReady()
{
    for (int i = 0; i < mButtons.size(); ++i)
    {
        OnSetButtonProperty(i);
    }

    //! Once everything is done, lets send manually a signal saying, do render things:
    WindowIOMCMessage msg;
    msg.SetMessageType(WindowIOMCMessage::ENABLE_DRAW); 
    msg.SetViewportWidget(mViewportWidget);
    msg.SetEnableDraw(true);
    emit mViewportWidget->OnSendWindowIoMessage(msg);

    if (mUpdaterThread == nullptr)
    {
        mUpdaterThread = new ViewportUpdaterThread(this);
        FlushUiStateToThreadUpdater();
    }
}


//----------------------------------------------------------------------------------------

void ViewportDockWidget::OnUIForAppClosed()
{
    mViewportWidget->OnAppUnloaded();

    delete mUpdaterThread;
    mUpdaterThread = nullptr;
}

//----------------------------------------------------------------------------------------

ViewportDockWidget::~ViewportDockWidget()
{
    delete mViewportWidget;
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::RegisterPropertyButton(QPushButton* button, const char* propertyName, Pegasus::App::ComponentType component)
{
    PropertyButton pb;
    pb.button = button;
    pb.name = propertyName;
    pb.component = component;
    int buttonIdx = mButtons.size();
    mButtons.push_back(pb);

    mButtonMapper->setMapping(button, buttonIdx);

    if (button->isCheckable())
    {
        connect(button, SIGNAL(toggled(bool)), mButtonMapper, SLOT(map()));
    }
    else
    {
        connect(button, SIGNAL(clicked()), mButtonMapper, SLOT(map()));
    }
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::OnSetButtonProperty(int buttonIdx)
{
    if (mViewportWidget != nullptr)
    {
        ViewportDockWidget::PropertyButton& pb = mButtons[buttonIdx];
        bool propertyValue = !pb.button->isCheckable() || pb.button->isChecked();
        mViewportWidget->SetBoolProperty(pb.component, pb.name, propertyValue);
    }
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::GetButtonStatuses(QVariantList& outputList)
{
    for (int i = 0; i < mButtons.size(); ++i)
    {
        const ViewportDockWidget::PropertyButton& pb = mButtons[i];
        outputList.push_back(QVariant(pb.button->isChecked()));
    }
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::SetButtonStatuses(const QVariantList& inputList)
{
    if (inputList.size() != mButtons.size()) return;
    for (int i = 0; i < mButtons.size(); ++i)
    {
        ViewportDockWidget::PropertyButton& pb = mButtons[i];
        bool val = inputList[i].toBool();
        pb.button->setChecked(val);
    }
}

//----------------------------------------------------------------------------------------

QToolBar * ViewportDockWidget::CreateMenu(QWidget * mainWidget)
{
    
    QIcon freeCamIcon(":MiscViews/camIcon.png");
    QPushButton* freeCamera = new QPushButton(freeCamIcon,tr(""),mainWidget);
    freeCamera->setCheckable(true);
    freeCamera->setChecked(true);
    freeCamera->setToolTip(tr("Toggle free cam on/off"));    
    RegisterPropertyButton(freeCamera, "EnableFreeCam", Pegasus::App::COMPONENT_WORLD);

    QIcon resetCamIcon(":MiscViews/resetCamIcon.png");
    QPushButton* resetCamera = new QPushButton(resetCamIcon,tr(""),mainWidget);
    resetCamera->setToolTip(tr("Reset free camera to origin."));    
    RegisterPropertyButton(resetCamera, "ResetFreeCam", Pegasus::App::COMPONENT_WORLD);

    QIcon debugCamIcon(":MiscViews/debugCamIcon.png");
    QPushButton* debugCamsButton = new QPushButton(debugCamIcon,tr(""),mainWidget);
    debugCamsButton->setCheckable(true);
    debugCamsButton->setChecked(false);
    debugCamsButton->setToolTip(tr("Toggle debug cam on/off"));    
    RegisterPropertyButton(debugCamsButton, "EnableDebug", Pegasus::App::COMPONENT_DEBUG_CAMERA);

    QIcon wireframeIcon(":TypeIcons/mesh.png");
    QPushButton* wireframeButton = new QPushButton(wireframeIcon,tr(""),mainWidget);
    wireframeButton->setCheckable(true);
    wireframeButton->setChecked(false);
    wireframeButton->setToolTip(tr("Toggle wire frame mode on/off"));
    RegisterPropertyButton(wireframeButton, "IsWireframe", Pegasus::App::COMPONENT_WORLD);

    QIcon gridIcon(":MiscViews/gridIcon.png");
    QPushButton* gridButton = new QPushButton(gridIcon,tr(""),mainWidget);
    gridButton->setCheckable(true);
    gridButton->setChecked(true);
    gridButton->setToolTip(tr("Toggle grid on/off"));
    RegisterPropertyButton(gridButton, "EnableGrid", Pegasus::App::COMPONENT_GRID);

    QIcon reticleIcon(":MiscViews/xyz.png");
    QPushButton* reticleButton = new QPushButton(reticleIcon,tr(""),mainWidget);
    reticleButton->setCheckable(true);
    reticleButton->setChecked(true);
    reticleButton->setToolTip(tr("Toggle xyz axis on/off"));
    RegisterPropertyButton(reticleButton, "EnableReticle", Pegasus::App::COMPONENT_GRID);

    QIcon terrainDebugIcon(":MiscViews/terrainIcon.png");
    QPushButton* terrainButton = new QPushButton(terrainDebugIcon,tr(""),mainWidget);
    terrainButton->setCheckable(true);
    terrainButton->setChecked(false);
    terrainButton->setToolTip(tr("3d terrain: debug geometry of terrain. Visualizes marching cube grid and density as dots in the corners."));
    RegisterPropertyButton(terrainButton, "EnableDebugGeometry", Pegasus::App::COMPONENT_TERRAIN3D);

    QIcon terrainCamCullDebugIcon(":MiscViews/terrain3dCamCullDebug.png");
    QPushButton* terrainCamCullButton = new QPushButton(terrainCamCullDebugIcon,tr(""),mainWidget);
    terrainCamCullButton->setCheckable(true);
    terrainCamCullButton->setChecked(false);
    terrainCamCullButton->setToolTip(tr("3d terrain: freeze camera cull of 3d terrain voxels."));
    RegisterPropertyButton(terrainCamCullButton, "EnableDebugCameraCull", Pegasus::App::COMPONENT_TERRAIN3D);

    //special buttons (buttons that deal with systems exclusively on the editor side)
    QIcon spinButtonIcon(":MiscViews/SpinIcon.png");
    QPushButton* spinButton = new QPushButton(spinButtonIcon, tr(""), mainWidget);
    spinButton->setCheckable(true);
    spinButton->setChecked(false);
    spinButton->setToolTip(tr("Enable spin render: call render on the timeline to force it to redraw. Warning: gpu and cpu usage will increase."));
    connect(spinButton, SIGNAL(toggled(bool)), this, SLOT(OnToggleSpinRender(bool)));

    QIcon spinOnceButtonIcon(":MiscViews/SpinOnceIcon.png");
    QPushButton* spinOnceButton = new QPushButton(spinOnceButtonIcon, tr(""), mainWidget);
    spinOnceButton->setCheckable(false);
    spinOnceButton->setChecked(false);
    spinOnceButton->setToolTip(tr("Render once: call render only once on this window, when this button is pressed."));
    connect(spinOnceButton, SIGNAL(clicked()), this, SLOT(OnRenderOnceButton()));



    QToolBar* tb = new QToolBar(this);
    tb->addWidget(freeCamera);
    tb->addWidget(resetCamera);
    tb->addWidget(debugCamsButton);
    tb->addSeparator();
    tb->addWidget(spinButton);
    tb->addWidget(spinOnceButton);
    tb->addSeparator();
    tb->addWidget(wireframeButton);
    tb->addWidget(gridButton);
    tb->addWidget(reticleButton);
    tb->addSeparator();
    tb->addWidget(terrainButton);
    tb->addWidget(terrainCamCullButton);

    return tb;
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::OnToggleSpinRender(bool enabled)
{
    mSpinRenderEnabled = enabled;
    FlushUiStateToThreadUpdater();
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::FlushUiStateToThreadUpdater()
{
    if (mUpdaterThread != nullptr)
    {
        emit ToggleSpinRender(mSpinRenderEnabled);
    }
}

//----------------------------------------------------------------------------------------


void ViewportDockWidget::ReceiveDrawRequest()
{
    if (mViewportWidget != nullptr)
    {
        mViewportWidget->DrawPegasusWindow();
    }
}

//----------------------------------------------------------------------------------------

void ViewportDockWidget::OnRenderOnceButton()
{
    if (mUpdaterThread != nullptr)
    {
        emit RenderOnce();
    }
}

//----------------------------------------------------------------------------------------

ViewportUpdaterThread::ViewportUpdaterThread(ViewportDockWidget* parent)
: QThread(parent), mDockWidget(parent), mInitializeSemaphore(0)
{
    start(QThread::LowestPriority);
    mInitializeSemaphore.acquire(1); //freeze constructor until Run has initialized events
}

//----------------------------------------------------------------------------------------

ViewportUpdaterThread::~ViewportUpdaterThread()
{
   quit(); 
   wait();
}

//----------------------------------------------------------------------------------------

void ViewportUpdaterThread::run()
{
    mInterface = new ViewportUpdaterInterface(mDockWidget);
    mInitializeSemaphore.release(1); //unfreeze the UI thread, let the constructor finish.
    exec();
    delete mInterface;
    mInterface = nullptr;
}

//----------------------------------------------------------------------------------------

ViewportUpdaterInterface::ViewportUpdaterInterface(ViewportDockWidget* dockWidget) : QObject(nullptr), mIsSpinning(false), mDockWidget(dockWidget)
{
    connect(dockWidget, SIGNAL(ToggleSpinRender(bool)), this, SLOT(ToggleSpinRenderSlot(bool)), Qt::QueuedConnection);
    connect(dockWidget, SIGNAL(RenderOnce()), this, SLOT(RenderOnceSlot()), Qt::QueuedConnection);
    connect(this, SIGNAL(RenderLoopSignal()), this, SLOT(RenderLoopSlot()), Qt::QueuedConnection);
    connect(this, SIGNAL(DrawPegasusWindowSignal()), dockWidget, SLOT(ReceiveDrawRequest()), Qt::QueuedConnection);

}

//----------------------------------------------------------------------------------------

void ViewportUpdaterInterface::ToggleSpinRenderSlot(bool enabled)
{
    mIsSpinning = enabled;
    if (mIsSpinning)
    {
        emit(RenderLoopSignal());
    }
}

//----------------------------------------------------------------------------------------

void ViewportUpdaterInterface::RenderOnceSlot()
{
    emit(DrawPegasusWindowSignal());
}

//----------------------------------------------------------------------------------------

void ViewportUpdaterInterface::RenderLoopSlot()
{
    QThread::msleep(64);
    if (mIsSpinning)
    {
        emit(DrawPegasusWindowSignal());
        emit(RenderLoopSignal());
    }
}
