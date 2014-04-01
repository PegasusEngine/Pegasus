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

private slots:
    //! slot to be called when a shader wants to be closed.
    //! \param index index of shader to close
    void RequestClose(int index);

private:
    //! maximum number of shader tabs to have open
    static const int MAX_TEXT_TABS = 50;

    //! sets the ui. To be used internally
    void SetupUi();

    //! finds the index of a particular shader
    int  FindIndex(Pegasus::Shader::IShaderProxy * target);

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
    
};

#endif
