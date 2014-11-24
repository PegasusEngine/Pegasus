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

namespace Pegasus {
    namespace Core {
        class ISourceCodeProxy;
    }
}

class QVBoxLayout;
class QStatusBar;
class QTabWidget;
class QSyntaxHighlighter;
class QSignalMapper;
class QMutex;
class QAction;
class QTab;
class CodeTextEditorTreeWidget;
class CodeTextEditorWidget;

//! Graphics Widget meant for code text editing
class CodeEditorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CodeEditorWidget(QWidget * parent);
    virtual ~CodeEditorWidget();

    //! opens a ISourceCodeProxy in the text editor
    //! \param ISourceCodeProxy the code to open
    void RequestOpen(Pegasus::Core::ISourceCodeProxy * shaderProxy);

    //! called whenever settings have been changed
    void OnSettingsChanged();

    //! checks if there is a compilation request pending (so we dont resend more)
    bool AsyncHasCompilationRequestPending();
    
    //! Sets a pending compilation request has been sent and is on its way to be cleard
    void AsyncSetCompilationRequestPending();
    
    //! Clears the compilation request and flushes the internal text of a code tab
    //! into the internal code source container
    //! \param id of the code editor to flush
    void FlushTextEditorToCode(int id);

    //! Called when a code compilation status has been changed
    //! \param code that has been updated
    void CodeUIChanged(Pegasus::Core::ISourceCodeProxy * code);

    //! Requests a code to compile
    //! \param the id of the editor (tab id). If the id is -1, then all programs are queried for compilaton
    void CompileCode(int id);

signals:
    void RequestCodeCompilation(int id);

private slots:
    //! slot to be called when a code wants to be closed.
    //! \param index code to close
    void RequestClose(int index);

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
    void SignalCompilationError(void* code, int line, QString errorString);

    //! signal triggered when a linking event has occured
    //! \param the program that triggered this event
    //! \param message the message of such error
    //! \param eventType the event type
    void SignalLinkingEvent(void* program, QString message, int eventType);

    //! signals the begining of a compilation request. Used to set UI states and clear stuff 
    //! \param the code pointer
    void SignalCompilationBegin(void* code);

    //! signals the end of a compilation request. Used to display any compilation error in the status bar
    void SignalCompilationEnd(QString log);

    //! signal triggered when pin icon is pressed in the toolbar
    void SignalPinActionTriggered(); 

    //! signal triggered when pin icon is pressed in the toolbar
    void SignalInstantCompilationActionTriggered(); 

    //! signal triggered when the user clicks on the save button
    void SignalSaveCurrentCode();

    //! signal triggered when a file save has ended successfuly
    void SignalSavedFileSuccess();

    //! Compiles current code
    void SignalCompileCurrentCode();

    //! signal triggered when a file save has ended badly
    void SignalSavedFileIoError(int ioError, QString msg);

    //! signal triggered when the user clicks on a tab in the tab bar.
    //! selects and visualizes a Code for opening
    void SignalViewCode(int tabId);

    //! signal to update the UI for the editor once the app is finished
    void UpdateUIForAppFinished();

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
    void SetupUi();

    void UpdateInstantCompilationButton(Pegasus::Core::ISourceCodeProxy* proxy);

    //! finds the index of a particular Code
    int  FindIndex(Pegasus::Core::ISourceCodeProxy * target);

    //! finds the index of a particular text edit
    int  FindIndex(CodeTextEditorWidget * target);

    //! Sets the status bar message
    void PostStatusBarMessage(const QString& string);

    //! ui component pool
    struct Ui
    {
        CodeTextEditorTreeWidget::SignalCombination mTextEditorSignals;
        QString       mStatusBarMessage;
        QStatusBar * mStatusBar; 
        QVBoxLayout * mMainLayout; //! the main layout of the text editor
        QTabBar     * mTabWidget;
        CodeTextEditorTreeWidget * mTreeEditor;
    } mUi;

    //! maximum number of Code tabs to have open
    static const int MAX_OPENED_CODES = 50;
    Pegasus::Core::ISourceCodeProxy * mOpenedCodes[MAX_OPENED_CODES];
    int mOpenCodeCount;

    //! poitns to the id of the previous tab index.
    int mPreviousTabIndex;

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
