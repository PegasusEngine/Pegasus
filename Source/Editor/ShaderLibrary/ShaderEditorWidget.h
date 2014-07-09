/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderEditorWidget.h
//! \author	Kleber Garcia
//! \date	30th Match 2014
//! \brief	Shader Editor IDE

#ifndef EDITOR_SHADEREDITORWIDGET_H
#define EDITOR_SHADEREDITORWIDGET_H

#include <QDockWidget>
#include "ShaderLibrary/ShaderTextEditorTreeWidget.h"

namespace Pegasus {
    namespace Shader {
        class IShaderProxy;
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
class ShaderTextEditorTreeWidget;
class ShaderTextEditorWidget;

//! Graphics Widget meant for shader text editing
class ShaderEditorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ShaderEditorWidget(QWidget * parent);
    virtual ~ShaderEditorWidget();

    //! opens a shader proxy in the text editor
    //! \param shaderProxy the shader to open
    void RequestOpen(Pegasus::Shader::IShaderProxy * shaderProxy);

    //! called whenever settings have been changed
    void OnSettingsChanged();

    //! checks if there is a compilation request pending (so we dont resend more)
    bool AsyncHasCompilationRequestPending();
    
    //! Sets a pending compilation request has been sent and is on its way to be cleard
    void AsyncSetCompilationRequestPending();
    
    //! Clears the compilation request and flushes the internal text of a shader tab
    //! into the internal shader source container
    //! \param id of the shader editor to flush
    void FlushShaderTextEditorToShader(int id);

    //! Called when a shader compilation status has been changed
    //! \param shader that has been updated
    void ShaderUIChanged(Pegasus::Shader::IShaderProxy * shader);

    //! Requests a shaders to compile
    //! \param the id of the editor (tab id). If the id is -1, then all programs are queried for compilaton
    void CompileShader(int id);

signals:
    void RequestShaderCompilation(int id);

private slots:
    //! slot to be called when a shader wants to be closed.
    //! \param index index of shader to close
    void RequestClose(int index);

    //! slot called whenever a tab changes its text.
    //! \param the widget that changed its text, should be casted to a QTextEdit object
    void OnTextChanged(QWidget *);

    //! slot called when the user clicks on a text editor window
    //! \param the widget that has been selected.
    void OnTextWindowSelected(QWidget *);

    //! signals a compilation error.
    //! \param the shader reference to set the compilation error
    //! \param the line number that such shader failed at
    //! \param the error message string of the failed compilation
    void SignalCompilationError(void* shader, int line, QString errorString);

    //! signal triggered when a linking event has occured
    //! \param the program that triggered this event
    //! \param message the message of such error
    //! \param eventType the event type
    void SignalLinkingEvent(void* program, QString message, int eventType);

    //! signals the begining of a compilation request. Used to set UI states and clear stuff 
    //! \param the shader pointer
    void SignalCompilationBegin(void* shader);

    //! signals the end of a compilation request. Used to display any compilation error in the status bar
    void SignalCompilationEnd(QString log);

    //! signal triggered when pin icon is pressed in the toolbar
    void SignalPinActionTriggered(); 

    //! signal triggered when the user clicks on the save button
    void SignalSaveCurrentShader();

    //! signal triggered when a file save has ended successfuly
    void SignalSavedFileSuccess();

    //! signal triggered when a file save has ended badly
    void SignalSavedFileIoError(int ioError, QString msg);

    //! signal triggered when the user clicks on a tab in the tab bar.
    //! selects and visualizes a shader for opening
    void SignalViewShader(int tabId);
    

private:

    //! request a syntax highlight update for a particular line
    //! \param the id of the text editor
    //! \param the line number to update
    void UpdateSyntaxForLine(ShaderTextEditorWidget * editor, int line);

    //! sets the ui. To be used internally
    void SetupUi();

    //! finds the index of a particular shader
    int  FindIndex(Pegasus::Shader::IShaderProxy * target);

    //! finds the index of a particular text edit
    int  FindIndex(ShaderTextEditorWidget * target);

    //! Sets the status bar message
    void PostStatusBarMessage(const QString& string);

    //! ui component pool
    struct Ui
    {
        ShaderTextEditorTreeWidget::SignalCombination mTextEditorSignals;
        QString       mStatusBarMessage;
        QStatusBar * mStatusBar; 
        QVBoxLayout * mMainLayout; //! the main layout of the text editor
        QTabBar     * mTabWidget;
        ShaderTextEditorTreeWidget * mTreeEditor;
    } mUi;

    //! maximum number of shader tabs to have open
    static const int MAX_OPENED_SHADERS = 50;
    Pegasus::Shader::IShaderProxy * mOpenedShaders[MAX_OPENED_SHADERS];
    int mOpenShaderCount;

    //! poitns to the id of the previous tab index.
    int mPreviousTabIndex;

    //! toolbar actions
    QIcon mPinIcon;
    QIcon mUnpinIcon;
    QAction * mPinAction;
    QAction * mSaveAction;
    QAction * mCloseViewAction;
    QAction * mHorizontalAction;
    QAction * mVerticalAction;

    //! compilation barrier. Throttles compilation if a signal has been sent
    bool mCompilationRequestPending;
    QMutex * mCompilationRequestMutex;

    bool mInternalBlockTextUpdated;

    
    
};

#endif
