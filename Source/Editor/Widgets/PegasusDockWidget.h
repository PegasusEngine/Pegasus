/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusDockWidget.h
//! \author	Kleber Garcia
//! \date	July 20th, 2015
//! \brief	Master widget class with common functions required by dock widgets

#ifndef EDITOR_PEGASUS_DOCK_WIDGET_H
#define EDITOR_PEGASUS_DOCK_WIDGET_H

#include <QDockWidget>
#include "MessageControllers/MsgDefines.h"

// fwd declarations
namespace Pegasus {
    struct PegasusAssetTypeDesc;
    namespace App {
        class IApplicationProxy;
    }
}

class Editor;
class QUndoStack;
class QFocusEvent;
class NodeFileTabBar;

class PegasusDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    
    //! Constructor
    //! \param the parent widget,
    //! \param the master editor instance
    PegasusDockWidget(QWidget* parent, Editor* editor);

    //! Destructor
    virtual ~PegasusDockWidget();

    //! Called on initialization
    void Initialize();

    //! Returns the editor instance owner
    Editor* GetEditor() const { return mEditor; }

    //! Fired when this widget is in focus
    //! \param event the event fired
    virtual void focusInEvent(QFocusEvent * event);

    //! Fired when this widget is out of focus
    //! \param event the event fired
    virtual void focusOutEvent(QFocusEvent * event);

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi() = 0;

    //! Returns the name this widget
    virtual const char* GetName() const = 0;

    //! Returns the title of this widget
    virtual const char* GetTitle() const = 0;

    //! Sends a message to the asset IO controller
    void SendAssetIoMessage(const AssetIOMCMessage& msg);

    //! Sends a message to the property grid IO controller
    void SendPropertyGridIoMessage(const PropertyGridIOMCMessage& msg);

    //! Sends a message to the graph IO controller
    void SendGraphIoMessage(const GraphIOMCMessage& msg);

    //! Sends a message to the timeline IO controller
    void SendTimelineIoMessage(const TimelineIOMCMessage& msg);

    //! Returns the current undo stack in focus for this widget
    //! \return implementation specific, must return the current active undo stack of this widget
    virtual QUndoStack* GetCurrentUndoStack() const { return nullptr; }

    //! Special pegasus forwarder function which asserts if this widget has focus
    virtual bool HasFocus() const { return hasFocus(); }

    //! Callback, implement here functionality that requires saving of current object
    virtual void OnSaveFocusedObject() {}

    //! Callback, implement here functionality that would pursue the deletion of whatever this widget has selected internally
    virtual void OnDeleteFocusedObject() {}

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc*const* GetTargetAssetTypes() const { return nullptr; }

    //! Implement this function with functionality on how to process for edit.
    //! Only objects of type retured in GetTargetAssetTypes will be the ones opened.
    virtual void OnOpenObject(AssetInstanceHandle object, const QString& displayName, const QVariant& initData) { }

    //! Refocus this widget
    void PerformFocus();

    NodeFileTabBar* CreateNodeFileTabBar();

signals:

    //! Fired when this widget is in focus
    void OnFocus(PegasusDockWidget* owner);

    //! Fired when this widget is out of focus
    void OutFocus(PegasusDockWidget* owner);

    //! Fired by the user, when we want to register dirtiness on an object
    void OnRegisterDirtyObject(AssetInstanceHandle object);

    //! Fired by the user, when we want to unregister dirtiness on an object
    void OnUnregisterDirtyObject(AssetInstanceHandle object);

    //! Sends a message to the asset IO controller
    void OnSendAssetIoMessage(PegasusDockWidget* sender, AssetIOMCMessage msg);

    //! Sends a message to the property grid IO controller
    void OnSendPropertyGridIoMessage(PropertyGridIOMCMessage msg);

    //! Sends a message to the graph IO controller
    void OnSendGraphIoMessage(GraphIOMCMessage msg);

    //! Sends a message to the timeline message controller.
    void OnSendTimelineIoMessage(TimelineIOMCMessage msg);

    //! Generic request a frame from the render thread.
    void OnRequestRedrawAllViewports();

public slots:
    
    //! Receives when an application has loaded
    void UpdateUIForAppLoaded();

    //! Receives when an application has been closed
    void UpdateUIForAppClosed();

    //! Receives an IO message
    void ReceiveAssetIoMessage(PegasusDockWidget* sender, AssetIOMCMessage::IoResponseMessage msg) { if (this == sender) { OnReceiveAssetIoMessage(msg); } }

    //! Received when an object has be requested to be opened.
    void ReceiveOpenRequest(AssetInstanceHandle object, const QString& displayName, const QVariant& initData) { OnOpenObject(object, displayName, initData); }

protected:
    // Receive an IO message, to be implemented by the widget
    //! \param msg the message
    virtual void OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage msg) {}

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application) {}

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed() {}


private:

    //! Editor instance
    Editor* mEditor;
    
};

#endif //EDITOR_PEGASUS_DOCK_WIDGET_H 
