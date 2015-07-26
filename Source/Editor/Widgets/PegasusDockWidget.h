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
#include "MessageControllers/AssetIOMessageController.h"

// fwd declarations
namespace Pegasus {
    namespace App {
        class IApplicationProxy;
    }
}

class Editor;
class QUndoStack;
class QFocusEvent;

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
    void SendAssetIoMessage(const AssetIOMessageController::Message& msg);

    //! Returns the current undo stack in focus for this widget
    //! \return implementation specific, must return the current active undo stack of this widget
    virtual QUndoStack* GetCurrentUndoStack() const { return nullptr; }

    //! Special pegasus forwarder function which asserts if this widget has focus
    virtual bool HasFocus() const { return hasFocus(); }

    //! Callback, implement here functionality that requires saving of current object
    virtual void OnSaveFocusedObject() {}

    //! Refocus this widget
    void PerformFocus();

signals:

    //! Fired when this widget is in focus
    void OnFocus(PegasusDockWidget* owner);

    //! Fired when this widget is out of focus
    void OutFocus(PegasusDockWidget* owner);

    //! Fired by the user, when we want to register dirtyness on an object
    void OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Fired by the user, when we want to unregister dirtyness on an object
    void OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Sends a message to the asset IO controller
    void OnSendAssetIoMessage(PegasusDockWidget* sender, AssetIOMessageController::Message msg);

public slots:
    
    //! Receives when an application has loaded
    void UpdateUIForAppLoaded();

    //! Receives when an application has been cloased
    void UpdateUIForAppClosed();

    //! Receives an io message
    void ReceiveAssetIoMessage(PegasusDockWidget* sender, AssetIOMessageController::Message::IoResponseMessage msg) { if (this == sender) { OnReceiveAssetIoMessage(msg); } }

protected:
    // Receive an io message, to be implemented by the widget
    //! \param msg the message
    virtual void OnReceiveAssetIoMessage(AssetIOMessageController::Message::IoResponseMessage msg) {}

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application) {}

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed() {}


private:

    //! Editor instance
    Editor* mEditor;
    
};

#endif //EDITOR_PEGASUS_DOCK_WIDGET_H 
