/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	CodeEditorWidget.h
//! \author	Kleber Garcia
//! \date	30th Match 2014
//! \brief	Code Editor IDE

#ifndef EDITOR_CODEEDITORWIDGET_H
#define EDITOR_CODEEDITORWIDGET_H

#include <QDockWidget>
#include "CodeEditor/CodeTextEditorTreeWidget.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/SourceIOMessageController.h"
#include "Widgets/PegasusDockWidget.h"


class Editor;
class CodeUserData;
class QVBoxLayout;
class QStatusBar;
class QTabWidget;
class QSyntaxHighlighter;
class QSignalMapper;
class QMutex;
class QAction;
class QTab;
class QFocusEvent;
class CodeTextEditorTreeWidget;
class CodeTextEditorWidget;
class CodeUserData;
class NodeFileTabBar;
class QUndoStack;

namespace Pegasus
{
    namespace AssetLib  {
        class IRuntimeAssetObjectProxy;
    }
}

//! Graphics Widget meant for code text editing
class CodeEditorWidget : public PegasusDockWidget
{
    Q_OBJECT

public:
    explicit CodeEditorWidget(QWidget * parent, Editor* editor);
    virtual ~CodeEditorWidget();

    //! checks if there is a compilation request pending (so we dont resend more)
    bool AsyncHasCompilationRequestPending();
    
    //! Sets a pending compilation request has been sent and is on its way to be cleard
    void AsyncSetCompilationRequestPending();
    
    //! Called when a code compilation status has been changed
    //! \param code that has been updated
    void CodeUIChanged(CodeUserData * code);

    //! Requests a code to compile
    //! \param the code user data to compile
    void CompileCode(CodeUserData* code);

    //! true if any child has focus, false otherwise
    bool HasAnyChildFocus() const;

    //! Returns the name this widget
    virtual const char* GetName() const { return "CodeEditor";}

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "Code Editor";}

    //! Returns the current undo stack in focus for this widget
    //! \return implementation specific, must return the current active undo stack of this widget
    virtual QUndoStack* GetCurrentUndoStack() const { return nullptr; }

    //! Special pegasus forwarder function which asserts if this widget has focus
    virtual bool HasFocus() const { return HasAnyChildFocus(); }

    //! Callback, implement here functionality that requires saving of current object
    virtual void OnSaveFocusedObject();

    //! Implement this function with functionality on how to process for edit.
    //! Only objects of type retured in GetTargetAssetTypes will be the ones opened.
    virtual void OnOpenObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc*const* GetTargetAssetTypes() const;

signals:

    //! called when the editor needs the asset library to freeze the ui
    void RequestCompilationBegin();

    //! called when the editor feels like deleting the user data from the render thread
    void RequestSafeDeleteUserData(CodeUserData* userData);

    //! Sends a message to the source IO controller
    void SendSourceIoMessage(SourceIOMessageController::Message msg);

public slots:

    //! slot to be called when a code wants to be closed.
    //! \param code asset object to close
    void RequestClose(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object, QObject* extraData);

    //! bless user data with any UI specific data required
    void BlessUserData(CodeUserData* codeUserData);

    //! unbless user data with any UI specific data that was blessed
    void UnblessUserData(CodeUserData* codeUserData);

    //! called whenever settings have been changed
    void OnSettingsChanged();

    //! Sets the status bar message
    void PostStatusBarMessage(const QString& string);

    //! signal triggered when the user clicks on the save button
    void SignalSaveCurrentCode();

    //! signal triggered right before closing an asset and discarding its internal changes
    void SignalDiscardCurrentObjectChanges();

    //! Triggered when compilation has been received and finished from the UI thread.
    void CompilationRequestReceived();


private slots:
    //! slot called whenever a tab changes its text.
    //! \param the widget that changed its text, should be casted to a QTextEdit object
    void OnTextChanged(QWidget *);

    //! slot called when the user clicks on a text editor window
    //! \param the widget that has been selected.
    void OnTextWindowSelected(QWidget *);

    //! signals a compilation error.
    //! \param the code reference to set the compilation error
    //! \param the line number that such code failed at
    //! \param the error message string of the failed compilation
    void SignalCompilationError(CodeUserData* code, int line, QString errorString);

    //! signal triggered when a linking event has occured
    //! \param the program that triggered this event
    //! \param message the message of such error
    //! \param eventType the event type
    void SignalLinkingEvent(QString message, int eventType);

    //! signals the begining of a compilation request. Used to set UI states and clear stuff 
    //! \param the code pointer
    void SignalCompilationBegin(CodeUserData* code);

    //! signals the end of a compilation request. Used to display any compilation error in the status bar
    void SignalCompilationEnd(QString log);

    //! signal triggered when pin icon is pressed in the toolbar
    void SignalPinActionTriggered(); 

    //! signal triggered when pin icon is pressed in the toolbar
    void SignalInstantCompilationActionTriggered(); 

    //! signal triggered when a file save has ended successfuly
    void SignalSavedFileSuccess();

    //! Compiles current code
    void SignalCompileCurrentCode();

    //! signal triggered when a file save has ended badly
    void SignalSavedFileIoError(int ioError, QString msg);

    //! signal triggered when the user clicks on a tab in the tab bar.
    //! selects and visualizes a Code for opening
    void SignalViewCode(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! function that disables or enables the instant compilation button.
    //! \param true to enable the button, false otherwise
    void EnableModeInstantCompilationButton(bool enableValue);

private:

    //! request a syntax highlight update for a particular line
    //! \param the id of the text editor
    //! \param the line number to update
    void UpdateSyntaxForLine(CodeTextEditorWidget * editor, int line);

    //! Sets the state and updates the UI of the instant compilation switch
    //! \param state, if true the ui updates the instant compilation switch on, otherwise off
    void SetInstantCompilationState(bool state);

    //! sets the ui. To be used internally
    virtual void SetupUi();

    void UpdateInstantCompilationButton(CodeUserData* code);

    //! signal to update the UI for the editor once the app is finished
    virtual void OnUIForAppClosed();

    // Receive an io message, to be implemented by the widget
    //! \param msg the message
    virtual void OnReceiveAssetIoMessage(AssetIOMessageController::Message::IoResponseMessage msg);

    //! ui component pool
    struct Ui
    {
        CodeTextEditorTreeWidget::SignalCombination mTextEditorSignals;
        QString       mStatusBarMessage;
        QStatusBar  * mStatusBar; 
        QVBoxLayout * mMainLayout; //! the main layout of the text editor
        NodeFileTabBar           * mTabWidget;
        CodeTextEditorTreeWidget * mTreeEditor;
    } mUi;

    //! toolbar actions
    QIcon mPinIcon;
    QIcon mUnpinIcon;
    QIcon mOnInstantCompilationIcon;
    QIcon mOffInstantCompilationIcon;

    QAction * mPinAction;
    QAction * mSaveAction;
    QAction * mCloseViewAction;
    QAction * mHorizontalAction;
    QAction * mVerticalAction;
    QAction * mInstantCompilationAction;
    QAction * mCompileAction;

    //! compilation barrier. Throttles compilation if a signal has been sent
    bool mCompilationRequestPending;
    QMutex * mCompilationRequestMutex;

    bool mInternalBlockTextUpdated;

    //flags for autocompilation
    bool mInstantCompilationFlag;
    
    // flag saved when it is automatically forced
    bool mSavedInstantCompilationFlag;

    Pegasus::Core::ISourceCodeProxy::CompilationPolicy mCompilationPolicy;
    
};

#endif
