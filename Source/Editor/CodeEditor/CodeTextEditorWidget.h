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
#include <QTextEdit>

namespace Pegasus {
    namespace Core {
        class ISourceCodeProxy;
    }
}
 
class QEvent;
class QSyntaxHighlighter;

//! text editor widget class. Represents the text editor widget
class CodeTextEditorWidget : public QTextEdit
{
    Q_OBJECT;
public:
    //! constructor
    CodeTextEditorWidget(QWidget * parent);

    //! destructor
    virtual ~CodeTextEditorWidget();

    //! initialization function. Call whenever you want a source code to be opened
    void Initialize(Pegasus::Core::ISourceCodeProxy * code); 

    //! function that clears the state of the widget
    void Uninitialize(); 

    //! gets the source code being used
    //! \return the source code assigned
    Pegasus::Core::ISourceCodeProxy * GetCode() { return mCode; }

    //! updates the syntax of a single line in the document.
    //! \param the line to update. If the line is out of range nothing happens
    void UpdateLineSyntax(int line);

    //! forces a full syntax update on the document
    //! \warning this function is very expensive
    void UpdateAllDocumentSyntax();

    //! flushes the text container to the source code internals
    void FlushTextToCode();

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

private:
    //! variable that tells if this widget is in focus
    bool mIsFocus;

    //! source code reference
    Pegasus::Core::ISourceCodeProxy * mCode;


    QSyntaxHighlighter * mSyntaxHighlighter;
};


#endif
