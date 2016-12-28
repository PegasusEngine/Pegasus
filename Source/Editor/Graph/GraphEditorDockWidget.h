/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEditorDockWidget.h
//! \author	Karolyn Boulanger
//! \date	05th June 2014
//! \brief	Dock widget for the graph editor

#ifndef EDITOR_GRAPHEDITORDOCKWIDGET_H
#define EDITOR_GRAPHEDITORDOCKWIDGET_H

#include <QDockWidget>
#include "Widgets/PegasusDockWidget.h"
#include "Widgets/NodeFileTabBar.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"

class ViewportWidget;
class QMdiSubWindow;
class Editor;
class GraphEditorGraphicsView;

namespace Pegasus {
    namespace Texture {
        class ITextureNodeProxy;
    }

    namespace App {
        class IApplicationProxy;
    }
}

namespace Ui
{
    class GraphEditorDockWidget;
}

//! Interface that defines specific visualization patterns
//! for a graph editor widget.
class IGraphEditorViewStrategy
{
public:
    IGraphEditorViewStrategy(){}
    virtual ~IGraphEditorViewStrategy(){}

    //! Returns the title of this widget
    virtual const char* GetWidgetTitle() const = 0;

    //! Returns the name this widget
    virtual const char* GetWidgetName()  const = 0;

    //! Returns the components used for the viewport on this view strategy
    virtual Pegasus::App::ComponentTypeFlags GetViewportComponents() const = 0;

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc** GetTargetAssetTypes() const = 0;
    
};

//! Object that keeps all the state of a graph.
//! KAROLYN-TODO: make this object keep the graph topology. Make sure that this
//! object has subobject (sub graph objects) that keep also sub property grids, which will be viewed when the user
//! clicks on the corresponding node.
class GraphState : public QObject
{
    Q_OBJECT
public:
    //! Main asset object instance handle of this graph.
    AssetInstanceHandle assetHandle;
};


//! Dock widget for the texture editor
class GraphEditorDockWidget : public PegasusDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent of the dock widget
    GraphEditorDockWidget(QWidget *parent, Editor* editor, IGraphEditorViewStrategy* viewStrategy);

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi();

    //! Returns the name this widget
    virtual const char* GetName() const { return mViewStrategy->GetWidgetName(); } 

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return mViewStrategy->GetWidgetTitle(); }

    //! Implement this function with functionality on how to process for edit.
    //! Only objects of type retured in GetTargetAssetTypes will be the ones opened.
    virtual void OnOpenObject(AssetInstanceHandle object, const QString& displayName, const QVariant& initData);

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc*const* GetTargetAssetTypes() const { return mViewStrategy->GetTargetAssetTypes(); }

    //! Callback, implement here functionality that requires saving of current object
    virtual void OnSaveFocusedObject();

    //! Special pegasus forwarder function which asserts if this widget has focus
    virtual bool HasFocus() const; 

    //! Destructor
    ~GraphEditorDockWidget();

    //! Get the viewport widget associated with the dock widget
    //! \return Viewport widget associated with the dock widget
    inline ViewportWidget * GetViewportWidget() const { return mViewportWidget; }


private slots:

    //! When closing this asset has been requested.
    void RequestClose(AssetInstanceHandle objectHandle, QObject* extraData);
    
    //! When viewing the code has been requested
    void SignalViewGraph(AssetInstanceHandle objectHandle);
    
    //! When the current tab has been signaled to save the current asset.
    void SignalSaveTab(int tabId);
    
    //! When the current object changes are marked to be discarded.
    void SignalDiscardObjectChanges(AssetInstanceHandle objectHandle);


protected:
    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* applicationProxy);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

    //------------------------------------------------------------------------------------
    
private:

    //! UI associated with the dock widget
    Ui::GraphEditorDockWidget* mUi;
    
    //! File tab bar
    NodeFileTabBar* mNodeFileTabBar;

    //! Viewport widget embedded in the dock widget
    ViewportWidget * mViewportWidget;

    //! Pointer to view strategy for this widget
    IGraphEditorViewStrategy* mViewStrategy;

    //! Widget that visualizes a graph
    GraphEditorGraphicsView* mGraphViewWidget;
};


#endif // EDITOR_GRAPHEDITORDOCKWIDGET_H
