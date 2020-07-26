/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramEditorWidget.h
//! \author	Kleber Garcia
//! \date	10rth March 2015
//! \brief	Program editor IDE

#ifndef EDITOR_PROGRAM_EDITOR_WIDGET_H
#define EDITOR_PROGRAM_EDITOR_WIDGET_H

#include "Widgets/PegasusDockWidget.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "MessageControllers/MsgDefines.h"

#include <QDockWidget>

#define MAX_PROGRAMS_OPEN 50

class QVBoxLayout;
class QToolButton;
class QSignalMapper;
class QToolBar;
class QTabBar;
class QFocusEvent;
class QStatusBar;
class QUndoStack;
class NodeFileTabBar;
class QLabel;

namespace Pegasus
{
    namespace Shader
    {
        class IProgramProxy;
    }

    namespace AssetLib
    {
        class IRuntimeAssetObjectProxy;
    }
}

class ProgramEditorWidget : public PegasusDockWidget
{
    Q_OBJECT;

    friend class ProgramEditorModifyShaderCmd;

public:
    //! Constructor
    ProgramEditorWidget(QWidget * parent, Editor* editor);

    //! Destructor
    virtual ~ProgramEditorWidget();

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi();

    //! Returns the current undo stack in focus for this widget
    //! \return implementation specific, must return the current active undo stack of this widget
    virtual QUndoStack* GetCurrentUndoStack() const; 

    //! Returns the name this widget
    virtual const char* GetName() const { return "ProgramEditorWidget"; }

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "Program Editor"; }

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

    //! Callback, implement here functionality that requires saving of current object
    virtual void OnSaveFocusedObject();

    //! Implement this function with functionality on how to process for edit.
    //! Only objects of type retured in GetTargetAssetTypes will be the ones opened.
    virtual void OnOpenObject(AssetInstanceHandle handle, const QString& displayName, const QVariant& initData);

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc*const* GetTargetAssetTypes() const ; 

    void SetShader(const QString& shaderPath);
    QString ProgramEditorWidget::GetCurrentShaderPath();

public slots:

    //! Closes a program.
    //! \param the tab index to request close
    void RequestCloseProgram(AssetInstanceHandle handle, QObject* extraData);

    //! Saves current program.
    void SignalSaveCurrentProgram();


    //! Saves current program tab.
    void SignalSaveTab(int tab);

    //! signal triggered right before closing an asset and discarding its internal changes
    void SignalDiscardObjectChanges(AssetInstanceHandle object);

    //! Synchronizes the current program to the UI
    void SyncUiToProgram();

    //! Posts a message to the status bar
    void PostStatusBarMessage(const QString& message);

    //! Receives an IO message response
    virtual void OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage id);

signals:
    //! Sends a job to the render thread. This is connected in the application interface
    void SendProgramIoMessage(ProgramIOMCMessage msg);

private:
    //! Enables/Disables the UI
    //! \param enable if true activates the UI, if false deactivates it. Use this to figure out
    //!        looks of UI when there is no program opened
    void EnableUi(bool enable);

    //! Clears the ui to a default state.
    void ClearUi();

    QWidget*       mMainWidget;
    QAction*       mSaveAction;
    QStatusBar*    mStatusBar;
    QString        mStatusBarMessage;
    QToolButton* mLoadShaderButton;
    QToolButton* mRemoveShaderButton;
    QLabel*      mShaderName;
    NodeFileTabBar* mTabBar;

protected slots:
    //! Triggered when a shader has been added / modified
    void OnLoadShader();

    //! Triggered when a shader has been removed
    void OnRemoveShader();

    //! Triggered when a program has been viewd
    void OnViewProgram(AssetInstanceHandle);

private:
    //! Pointer to the current program opened
    AssetInstanceHandle mCurrentProgram;

    struct ProgramData {
        QString name;
        QString sourcePath;
    };
    
    QMap<AssetInstanceHandle, ProgramData> mProgramData;

};


#endif
