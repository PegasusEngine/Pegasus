/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	CodeTextEditorWidget.h
//! \author	Kleber Garcia
//! \date	22nd April 2014
//! \brief	Graphics widget representing a single source code text editor

#ifndef EDITOR_CODETEXTEDITORWIDGET_H
#define EDITOR_CODETEXTEDITORWIDGET_H
#include <QPlainTextEdit>
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include <MessageControllers/MsgDefines.h>

class QEvent;
class QSyntaxHighlighter;
class QPaintEvent;

//! Data structure holding the state of code.
struct SourceState
{
    AssetInstanceHandle handle; //! handle to pegasus asset 
    QTextDocument* document; //! ui qt document holding the ui text
    QSet<int> errorLines;   //! set of lines that have compilation errors
    QMap<int, QString> errorMessages; //! map of line / string error messages.
    Pegasus::Core::ISourceCodeProxy::CompilationPolicy compilationPolicy; //compilation policy

    //! constructor
    SourceState() : document(nullptr), compilationPolicy(Pegasus::Core::ISourceCodeProxy::POLICY_USER_DEFINED) {}
};

//! text editor widget class. Represents the text editor widget
class CodeTextEditorWidget : public QPlainTextEdit
{
    friend class LineNumberArea;
    Q_OBJECT;
public:
    //! constructor
    CodeTextEditorWidget(QWidget * parent);

    //! destructor
    virtual ~CodeTextEditorWidget();

    //! initialization function. Call whenever you want a source code to be opened
    void Initialize(SourceState * code); 

    //! function that clears the state of the widget
    void Uninitialize(); 

    //! gets the source code being used
    //! \return the source code assigned
    SourceState * GetCode() { return mCode; }

    //! updates the syntax of a single line in the document.
    //! \param the line to update. If the line is out of range nothing happens
    void UpdateLineSyntax(int line);

    //! forces a full syntax update on the document
    //! \warning this function is very expensive
    void UpdateAllDocumentSyntax();

    //! \return true if the user has this widget in focus, false otherwise
    bool IsFocus() const { return mIsFocus; }

signals:
    void SignalSelected();

protected:
    //! event callback
    virtual bool event(QEvent * e);

    //! focus in event callbacks
    virtual void focusInEvent(QFocusEvent * e);

    //! focus out event callbacks
    virtual void focusOutEvent(QFocusEvent * e);

    //! resize event callback
    virtual void resizeEvent(QResizeEvent *event);

private slots:
    //! updates the width of the line number area
    void updateLineNumberAreaWidth(int newBlockCount);

    //! highlights the current line of the cursor
    void highlightCurrentLine();
    
    //! updates the line number area
    void updateLineNumberArea(const QRect &, int);

private:

    //! Line number paint event
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    //! gets the width of the line number area
    int lineNumberAreaWidth();

    //! variable that tells if this widget is in focus
    bool mIsFocus;

    //! source code reference
    SourceState* mCode;

    QTextDocument* mNullDocument;

    QSyntaxHighlighter * mSyntaxHighlighter;

    QWidget* mLineNumberArea;
};


#endif
