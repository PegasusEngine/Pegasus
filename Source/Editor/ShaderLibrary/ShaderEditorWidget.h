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

namespace Pegasus {
    namespace Shader {
        class IShaderProxy;
    }
}

class QVBoxLayout;
class QTabWidget;
class QSyntaxHighlighter;
class QSignalMapper;
class QMutex;
class QAction;
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


signals:
    void RequestShaderCompilation(int id);

private slots:
    //! slot to be called when a shader wants to be closed.
    //! \param index index of shader to close
    void RequestClose(int index);

    //! slot called whenever a tab changes its text.
    //! \param the widget that changed its text, should be casted to a QTextEdit object
    void OnTextChanged(QWidget *);

    //! signals a compilation error.
    //! \param the shader reference to set the compilation error
    //! \param the line number that such shader failed at
    //! \param the error message string of the failed compilation
    void SignalCompilationError(void* shader, int line, QString errorString);

    //! signals the begining of a compilation request. Used to set UI states and clear stuff 
    //! \param the shader pointer
    void SignalCompilationBegin(void* shader);

    //! signal triggered when pin icon is pressed in the toolbar
    void SignalPinActionTriggered(); 

private:

    //! request a syntax highlight update for a particular line
    //! \param the id of the text editor
    //! \param the line number to update
    void UpdateSyntaxForLine(int id, int line);

    //! maximum number of shader tabs to have open
    static const int MAX_TEXT_TABS = 50;

    //! sets the ui. To be used internally
    void SetupUi();

    //! finds the index of a particular shader
    int  FindIndex(Pegasus::Shader::IShaderProxy * target);

    //! finds the index of a particular text edit
    int  FindIndex(ShaderTextEditorWidget * target);

    //! Updates the syntax and style of a specific textedit widget
    void SynchronizeTextEditWidgetSyntaxStyle(int i);

    //! ui component pool
    struct Ui
    {
        QVBoxLayout * mMainLayout; //! the main layout of the text editor
        QTabWidget  * mTabWidget; //! the tab widget
        QWidget   * mWidgetPool[MAX_TEXT_TABS]; //! pool of widgets for tabs
        ShaderTextEditorWidget * mTextEditPool[MAX_TEXT_TABS]; //! pool of text editor tabs
    } mUi;
    //! toolbar actions
    QIcon mPinIcon;
    QIcon mUnpinIcon;
    QAction * mPinAction;

    int mTabCount; //! count of tabs

    //! signal mapper for shader text editors
    QSignalMapper * mShaderEditorSignalMapper;

    //! compilation barrier. Throttles compilation if a signal has been sent
    bool mCompilationRequestPending;
    QMutex * mCompilationRequestMutex;

    bool mInternalBlockTextUpdated;

    
    
};

#endif
