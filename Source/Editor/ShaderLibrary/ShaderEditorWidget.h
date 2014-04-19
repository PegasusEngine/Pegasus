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

#include <QWidget>

namespace Pegasus {
    namespace Shader {
        class IShaderProxy;
    }
}

class QVBoxLayout;
class QTabWidget;
class QTextEdit;
class QSyntaxHighlighter;
class QSignalMapper;
class QMutex;

//! Graphics Widget meant for shader text editing
class ShaderEditorWidget : public QWidget
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

    bool AsyncHasCompilationRequestPending();
    void AsyncSetCompilationRequestPending();
    void FlushShaderTextEditorToShader(int id);

signals:
    void RequestShaderCompilation(int id);

private slots:
    //! slot to be called when a shader wants to be closed.
    //! \param index index of shader to close
    void RequestClose(int index);

    //! slot called whenever a tab changes its text.
    //! \param the widget that changed its text, should be casted to a QTextEdit object
    void OnTextChanged(QWidget *);

private:
    //! maximum number of shader tabs to have open
    static const int MAX_TEXT_TABS = 50;

    //! sets the ui. To be used internally
    void SetupUi();

    //! finds the index of a particular shader
    int  FindIndex(Pegasus::Shader::IShaderProxy * target);

    //! finds the index of a particular text edit
    int  FindIndex(QTextEdit * target);

    //! ui component pool
    struct Ui
    {
        QVBoxLayout * mMainLayout; //! the main layout of the text editor
        QTabWidget  * mTabWidget; //! the tab widget
        QWidget   * mWidgetPool[MAX_TEXT_TABS]; //! pool of widgets for tabs
        QTextEdit * mTextEditPool[MAX_TEXT_TABS]; //! pool of text editor tabs
        QSyntaxHighlighter * mSyntaxHighlighterPool[MAX_TEXT_TABS]; //! pool of syntax highlighers
    } mUi;

    int mTabCount; //! count of tabs

    //! pool of shader proxy pointers
    Pegasus::Shader::IShaderProxy * mShaderProxyPool[MAX_TEXT_TABS];

    //! signal mapper for shader text editors
    QSignalMapper * mShaderEditorSignalMapper;

    //! compilation barrier. Throttles compilation if a signal has been sent
    bool mCompilationRequestPending;
    QMutex * mCompilationRequestMutex;
    
    
};

#endif
