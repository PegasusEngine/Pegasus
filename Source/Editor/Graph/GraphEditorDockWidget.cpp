/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEditorDockWidget.cpp
//! \author	Karolyn Boulanger
//! \date	05th June 2014
//! \brief	Dock widget for the graph editor

#include "Graph/GraphEditorDockWidget.h"
#include "Graph/GraphEditorGraphicsView.h"
#include "Graph/Items/GraphNodeGraphicsItem.h"
#include "Viewport/ViewportWidget.h"

//! \todo Temporary
#include "Editor.h"
#include "Application/ApplicationManager.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"
#include "Pegasus/Texture/Shared/ITextureManagerProxy.h"
#include "Pegasus/Texture/Shared/ITextureConfigurationProxy.h"
#include "ui_GraphEditorDockWidget.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>


GraphEditorDockWidget::GraphEditorDockWidget(QWidget *parent, Editor* editor, IGraphEditorViewStrategy* viewStrategy)
:   PegasusDockWidget(parent, editor), mViewStrategy(viewStrategy), mUi(nullptr)
{
    mUi = new Ui::GraphEditorDockWidget();
}

//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::SetupUi()
{
    mUi->setupUi(this);
    setFocusPolicy(Qt::NoFocus);
    mUi->propertyGridWidget->SetMessenger(this);

    // Create the viewport widget that will contain the previewer
    mViewportWidget = new ViewportWidget(mUi->mainWidget, mViewStrategy->GetViewportComponents());
    mViewportWidget->setMinimumSize(128, 128);
    mViewportWidget->setMaximumSize(1080, 1080);
    mUi->viewportLayout->insertWidget(0, mViewportWidget);
    
    // Connect the message that signals that the tab selection has changed
    mNodeFileTabBar = new NodeFileTabBar(this);
    mGraphViewWidget =  new GraphEditorGraphicsView(this);
    mUi->graphEditorLayout->insertWidget(0, mNodeFileTabBar);
    mUi->graphEditorLayout->insertWidget(1, mGraphViewWidget);

    connect(
        mNodeFileTabBar, SIGNAL(RuntimeObjectRemoved(AssetInstanceHandle, QObject*)),
        this, SLOT(RequestClose(AssetInstanceHandle, QObject*))
    );

    connect(
        mNodeFileTabBar, SIGNAL(DisplayRuntimeObject(AssetInstanceHandle)),
        this,   SLOT(SignalViewGraph(AssetInstanceHandle))
    );

    connect(
        mNodeFileTabBar, SIGNAL(SaveRuntimeObject(int)),
        this,   SLOT(SignalSaveTab(int))
    );

    connect(
        mNodeFileTabBar, SIGNAL(DiscardObjectChanges(AssetInstanceHandle)),
        this,   SLOT(SignalDiscardObjectChanges(AssetInstanceHandle))
    );


    /********************/
    //! \todo TEMPORARY graph to see what the editor looks like
    QList<QString> inputList;
    GraphNodeGraphicsItem* gradientItem = mGraphViewWidget->CreateNode("Gradient", inputList);
    GraphNodeGraphicsItem* colorItem = mGraphViewWidget->CreateNode("Color", inputList);

    inputList.clear();
    inputList << "InputA";
    inputList << "InputB";
    inputList << "InputC";
    GraphNodeGraphicsItem* blendItem = mGraphViewWidget->CreateNode("Blend", inputList);

    inputList.clear();
    inputList << "Out";
    GraphNodeGraphicsItem* outputItem = mGraphViewWidget->CreateNode("Output", inputList);

    GraphNodeOutputGraphicsItem* gradientOutputItem = gradientItem->GetOutputItem();
    GraphNodeOutputGraphicsItem* colorOutputItem = colorItem->GetOutputItem();
    GraphNodeInputGraphicsItem* blendInputItem0 = blendItem->GetInputItem(0);
    GraphNodeInputGraphicsItem* blendInputItem1 = blendItem->GetInputItem(1);
    GraphNodeInputGraphicsItem* blendInputItem2 = blendItem->GetInputItem(2);
    GraphNodeOutputGraphicsItem* blendOutputItem = blendItem->GetOutputItem();
    GraphNodeInputGraphicsItem* outputInputItem0 = outputItem->GetInputItem(0);

    mGraphViewWidget->CreateConnection(gradientOutputItem, blendInputItem0);
    mGraphViewWidget->CreateConnection(gradientOutputItem, blendInputItem2);
    mGraphViewWidget->CreateConnection(colorOutputItem, blendInputItem1);
    mGraphViewWidget->CreateConnection(blendOutputItem, outputInputItem0);
    /********************/
}

//----------------------------------------------------------------------------------------

GraphEditorDockWidget::~GraphEditorDockWidget()
{
    delete mUi;
}

//----------------------------------------------------------------------------------------

bool GraphEditorDockWidget::HasFocus() const
{
    return hasFocus() || mViewportWidget->hasFocus() || mNodeFileTabBar->hasFocus() || mGraphViewWidget->hasFocus();
}

//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* applicationProxy)
{
    mViewportWidget->OnAppLoaded();

    //! After all tabs are loaded, update the UI for the current tab
    mUi->propertyGridWidget->SetApplicationProxy(applicationProxy);

    WindowIOMCMessage msg;
    msg.SetMessageType(WindowIOMCMessage::ENABLE_DRAW);
    msg.SetViewportWidget(mViewportWidget);
    msg.SetEnableDraw(true);
    emit mViewportWidget->OnSendWindowIoMessage(msg);
}

//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::OnUIForAppClosed() 
{
    //close all pending apps
    while (mNodeFileTabBar->GetTabCount())
    {
        mNodeFileTabBar->Close(0);
    }
    mViewportWidget->OnAppUnloaded();
    mUi->propertyGridWidget->SetApplicationProxy(nullptr);
}

//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::OnOpenObject(AssetInstanceHandle object, const QString& displayName, const QVariant& initData)
{
    QVariantMap m = initData.toMap();
    //KAROLYN-TODO: 
    // Parse here whatever initData the GraphIOMessageController has translated from the function "TranslateToQt". Store it in the
    // graphState. Read this graph state when SignalViewGraph has been called.
    show();
    activateWindow();
    GraphState* graphState = new GraphState();
    graphState->assetHandle = object;
    mNodeFileTabBar->Open(object, displayName, graphState); //this will also signal viewing of this graph widget.
}

//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::RequestClose(AssetInstanceHandle objectHandle, QObject* extraData)
{
    AssetIOMCMessage msg(AssetIOMCMessage::CLOSE_ASSET);
    msg.SetObject(objectHandle);
    SendAssetIoMessage(msg);
    delete static_cast<GraphState*>(extraData);
}
    
//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::SignalViewGraph(AssetInstanceHandle objectHandle)
{
    //KAROLYN-TODO: 
    // this function gets called when a tab is selected, and we are required to change views.
    // there are multiple ways of doing this, by either switching an entire widget containing the view
    // or by reconstructing the graph that we are requesting completely.
    // Remember, this function only gets called on the tab that gets changed.

    int objectIndex = mNodeFileTabBar->FindIndex(objectHandle);
    if (objectIndex != -1)
    {
        GraphState* graphState = static_cast<GraphState*>(mNodeFileTabBar->GetExtraData(objectIndex));
        if (graphState != nullptr)
        {
            //let the property grid visualize this graph's root:
            mUi->propertyGridWidget->SetCurrentProxy(graphState->assetHandle);

            //let the viewport draw this graph's result:
            GraphIOMCMessage msg(GraphIOMCMessage::VIEW_GRAPH_ON_VIEWPORT);
            msg.SetTargetViewport(mViewportWidget);
            msg.SetGraphHandle(graphState->assetHandle);
            SendGraphIoMessage(msg);
        }
        //KAROLYN-TODO: put here code to visualize the actual graph.
    }
}

//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::OnSaveFocusedObject() 
{
    int index = mNodeFileTabBar->GetCurrentIndex();
    if (index != -1)
    {
        SignalSaveTab(index);
    }
}
    
//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::SignalSaveTab(int tabId)
{
    AssetInstanceHandle objectHandle = mNodeFileTabBar->GetTabObject(tabId);
    AssetIOMCMessage msg(AssetIOMCMessage::SAVE_ASSET);
    msg.SetObject(objectHandle);
    SendAssetIoMessage(msg);
}
    
//----------------------------------------------------------------------------------------

void GraphEditorDockWidget::SignalDiscardObjectChanges(AssetInstanceHandle objectHandle)
{
    //KAROLYN-TODO: this function gets called when these actions occur:
    // - the object handle is marked as dirty, somehow
    // - the user closes the tab
    // - the prompt wether to save or discard changes appears
    // - the user selects discard changes.
    // Make sure that after the reload from asset message has been set, you re-visualize or refresh somehow the view
    // of this graph.
    AssetIOMCMessage msg(AssetIOMCMessage::RELOAD_FROM_ASSET);
    msg.SetObject(objectHandle);
    SendAssetIoMessage(msg);
}
