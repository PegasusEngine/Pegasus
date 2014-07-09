/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderTextEditorWidget.h
//! \author	Kleber Garcia
//! \date	22nd April 2014
//! \brief	Graphics widget representing a single shader text editor
#include "ShaderLibrary/ShaderTextEditorWidget.h"
#include "ShaderLibrary/ShaderManagerEventListener.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include <QEvent>
#include <QHelpEvent>
#include <QString>
#include <Qtooltip.h>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QVector>
#include <QSet>


//! internal class serving as the syntax highlighter for shade code
class ShaderSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    ShaderSyntaxHighlighter(QTextDocument * parent)
    : QSyntaxHighlighter(parent), mSignalSyntaxError(false), mShaderUserData(nullptr)
    {
        static const char  * keywords[] = {
            "\\b[0-9]*\\.?[0-9]+f?\\b",
            "\\bsin\\b",
            "\\bcos\\b",
            "\\bmix\\b",
            "\\bdot\\b",
            "\\bnormalize\\b",
            "\\bdistance\\b",
            "\\bvoid\\b",
            "\\bfloat\\b",
            "\\b[biud]?vec[1-4]?\\b",
            "\\bbool\\b",
            "\\bint\\b",
            "\\buint\\b",
            "\\bfloat\\b",
            "\\bdouble\\b",
            "\\bmat[1-4]x?[1-4]?\\b",
            "\\buniform\\b",
            "\\bvarying\\b",
            "\\battribute\\b",
            "\\bstruct\\b",
            "\\bout\\b",
            "\\blayout\\b",
            "\\bin\\b",
            "\\bpow\\b",
            nullptr
        };

        const char * ptr = keywords[0];
        int i = 0;
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(QColor(130,225,50,255)); 
        while (ptr != nullptr)
        {
            QRegExp keywordExp(ptr);
            mRules << keywordExp;
            ptr = keywords[++i]; 
        }
    }

    virtual ~ShaderSyntaxHighlighter() {}

    void SetShaderUserData(const ShaderUserData * shaderUserData) { mShaderUserData = shaderUserData;}
    
    
protected:

    QVector<QRegExp> mRules;
    
    bool mWrongLines;
    bool mSignalSyntaxError;
    const ShaderUserData * mShaderUserData;

    //! sets the formats for comments
    void SetCCommentStyle(int start, int end, Settings * settings)
    {
        QTextCharFormat f;
        f.setForeground(settings->GetShaderSyntaxColor(Settings::SYNTAX_C_COMMENT));
        if (mSignalSyntaxError) f.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        setFormat(start, end, f);
    }

    //! sets the formats for CPP coments
    void SetCPPCommentStyle(int start, int end, Settings * settings)
    {
        QTextCharFormat f;
        f.setForeground(settings->GetShaderSyntaxColor(Settings::SYNTAX_CPP_COMMENT));
        if (mSignalSyntaxError) f.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        setFormat(start, end, f);
    }

    //! sets the formats for regular style
    void SetNormalStyle(int start, int end, Settings * settings)
    {
        QTextCharFormat f;
        f.setForeground(settings->GetShaderSyntaxColor(Settings::SYNTAX_NORMAL_TEXT));
        if (mSignalSyntaxError) f.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        setFormat(start, end, f);
    }

    //! qt callback that processes highlighting in a line of text
    virtual void highlightBlock(const QString& text)
    {
        //TODO: Because the settings has a direct dependency on the widget dock, 
        //      we must request the settings singleton every time we highlight a line.
        //      this is a lot of memory dereference :/ we could optimize this if we cache
        //      the settings as a member of this class
        Settings * settings = Editor::GetInstance().GetSettings();
        // state machine definitions
        const int NormalState = -1;
        const int CommentState = 0;
        int state = previousBlockState();
        int start = 0;
        bool isCommentLine = false;

        mSignalSyntaxError = mShaderUserData == nullptr ? false : mShaderUserData->IsInvalidLine(currentBlock().firstLineNumber()+1);

        // for every character
        for (int i = 0; i < text.length(); ++i)
        {
            if (state == CommentState)
            {
                //close the comment
                if (text.mid(i, 2) == "*/")
                {
                    state = NormalState;
                    SetCPPCommentStyle(start, i - start + 2, settings);
                    start = i + 2;
                }
            }
            else
            {
                // trigger a full line comment
                if (text.mid(i, 2) == "//")
                {
                    isCommentLine = true;
                    SetCCommentStyle(i, text.length() - i, settings);
                    start = text.length();
                    break;
                }
                // trigger a comment block
                else if (text.mid(i, 2) == "/*")
                {
                    start = i;
                    state = CommentState; 
                }
            }
        }

        if (state == CommentState && start < text.length())
        {
            SetCPPCommentStyle(start, text.length() - start, settings);
            start =  text.length();
        }

        if (start < text.length())
        {
            SetNormalStyle(start, text.length(), settings);
        }

        if (state != CommentState && !isCommentLine)
        {
            for (int i = 0; i < mRules.size(); ++i)
            {
                QRegExp& pattern = mRules[i];
                int index = pattern.indexIn(text);
                while (index >= 0)
                {
                    int length = pattern.matchedLength();
                    if (start < index + length)
                    {
                        start = index + length;
                    }
                    Settings::ShaderEditorSyntaxStyle style = i == 0 ? Settings::SYNTAX_NUMBER_VALUE : Settings::SYNTAX_KEYWORD;
                    QTextCharFormat f;
                    f.setForeground(settings->GetShaderSyntaxColor(style));
                    if (mSignalSyntaxError) f.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                    setFormat(index, length, f);
                    index = pattern.indexIn(text, index + length);
               
                }
            }
            if (start < text.length())
            {
                SetNormalStyle(start, text.length(), settings);
            }
        }
        setCurrentBlockState(state);
    }
    
};

ShaderTextEditorWidget::ShaderTextEditorWidget(QWidget * parent)
: QTextEdit(parent), mShader(nullptr), mIsFocus(false)
{
    mSyntaxHighlighter = new ShaderSyntaxHighlighter(document());
}


ShaderTextEditorWidget::~ShaderTextEditorWidget()
{
    delete mSyntaxHighlighter;
}

void ShaderTextEditorWidget::Initialize(Pegasus::Shader::IShaderProxy * shader)
{
    mShader = shader;
    if (shader != nullptr)
    {
        static_cast<ShaderSyntaxHighlighter*>(mSyntaxHighlighter)->SetShaderUserData(static_cast<ShaderUserData*>(shader->GetUserData()));

        //set the text of the current text editor
        const char * srcChar = nullptr;
        int srcSize = 0;
        shader->GetSource(&srcChar, srcSize);
        QChar * qchar = new QChar[srcSize];
        for (int i = 0; i < srcSize; ++i)
        {
            qchar[i] = srcChar[i];
        }

        QString srcQString(qchar, srcSize);
        setText(srcQString);
        delete[] qchar;            
    }
    else
    {
        static_cast<ShaderSyntaxHighlighter*>(mSyntaxHighlighter)->SetShaderUserData(nullptr);
        setText(tr(""));
    }
}

void ShaderTextEditorWidget::Uninitialize()
{
    mShader = nullptr;
    static_cast<ShaderSyntaxHighlighter*>(mSyntaxHighlighter)->SetShaderUserData(nullptr);
    
}

void ShaderTextEditorWidget::UpdateLineSyntax(int line)
{
    QTextDocument * doc = document();
    int lineId = line - 1;
    if (lineId > 0 && lineId < doc->lineCount())
    {
        QTextBlock block = doc->findBlockByLineNumber(lineId);
        mSyntaxHighlighter->rehighlightBlock(block);
    }
}

void ShaderTextEditorWidget::UpdateAllDocumentSyntax()
{
    
    QPalette p = palette();
    p.setColor(QPalette::Base, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_BACKGROUND));
    p.setColor(QPalette::Text, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_NORMAL_TEXT));
    int fontSize = Editor::GetInstance().GetSettings()->GetShaderEditorFontSize();
    int tabStop  = Editor::GetInstance().GetSettings()->GetShaderEditorTabSize();

    QFont f("Courier");
    f.setPointSize(fontSize);
    
    QFontMetrics metrics(f);

    setFont(f);
    setPalette(p);
    setTabStopWidth(tabStop * metrics.width(' '));

    mSyntaxHighlighter->rehighlight();
}

bool ShaderTextEditorWidget::event(QEvent * e)
{
    if (e->type() == QEvent::ToolTip)
    {
        if (mShader != nullptr)
        {
            ShaderUserData * shaderUserData = static_cast<ShaderUserData*>(mShader->GetUserData()); 
            if (shaderUserData != nullptr)
            {
                QMap<int, QString>& messageMap = shaderUserData->GetMessageMap();
                QHelpEvent * helpEvent = static_cast<QHelpEvent*>(e);
                const QPoint& pos = helpEvent->pos();
                QTextCursor cursor = cursorForPosition(pos);
                int line = cursor.block().firstLineNumber() + 1;
                QMap<int, QString>::iterator it = messageMap.find(line);
                if (it != messageMap.end())
                {
                    QToolTip::showText(helpEvent->globalPos(), QString(it.value()));
                }
            }
        }
    }
    return QTextEdit::event(e);
}

void ShaderTextEditorWidget::focusInEvent(QFocusEvent * e)
{
    mIsFocus = true;
    QTextEdit::focusInEvent(e);
    emit(SignalSelected());
}

void ShaderTextEditorWidget::focusOutEvent(QFocusEvent * e)
{
    mIsFocus = false;
    QTextEdit::focusOutEvent(e);
}

void ShaderTextEditorWidget::FlushTextToShader()
{
    if (mShader != nullptr)
    {
        QString qs = toPlainText();
        QByteArray arr = qs.toLocal8Bit();
        const char * source = arr.data();
        int sourceSize = qs.size();
        mShader->SetSource(source, sourceSize);
    }
}
