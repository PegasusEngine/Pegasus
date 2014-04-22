/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderEditorWidget.cpp	
//! \author	Kleber Garcia
//! \date	30th Match 2014
//! \brief	Shader Editor IDE

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Settings/Settings.h"
#include "ShaderLibrary/ShaderEditorWidget.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "ShaderLibrary/ShaderManagerEventListener.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTextEdit>
#include <QColor>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTextCharFormat>
#include <QSignalMapper>
#include <QMutex>
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

ShaderEditorWidget::ShaderEditorWidget (QWidget * parent)
: QWidget(parent), mTabCount(0), mCompilationRequestPending(false), mInternalBlockTextUpdated(false)
{
    mCompilationRequestMutex = new QMutex();
    SetupUi();
}

ShaderEditorWidget::~ShaderEditorWidget()
{
    delete mCompilationRequestMutex;
}

void ShaderEditorWidget::SetupUi()
{
    mShaderEditorSignalMapper = new QSignalMapper(this);

    resize(550, 700);
    setWindowTitle(QString("Shader Editor"));
    mUi.mMainLayout = new QVBoxLayout();
    mUi.mTabWidget = new QTabWidget(this);
    mUi.mTabWidget->setTabsClosable(true);
    
    connect(
        mUi.mTabWidget, SIGNAL(tabCloseRequested(int)),
        this, SLOT(RequestClose(int))
    );
    
    mUi.mMainLayout->addWidget(mUi.mTabWidget);
    
    for (int i = 0; i < MAX_TEXT_TABS; ++i)
    {
        mUi.mWidgetPool[i] = new QWidget();
        mUi.mTextEditPool[i] = new QTextEdit();

        mUi.mTextEditPool[i]->setFontFamily(QString("Courier"));
        mUi.mSyntaxHighlighterPool[i] = new ShaderSyntaxHighlighter(mUi.mTextEditPool[i]->document());

        connect(mUi.mTextEditPool[i], SIGNAL(textChanged()),
                mShaderEditorSignalMapper, SLOT(map()));
    
        mShaderEditorSignalMapper->setMapping(mUi.mTextEditPool[i], static_cast<QWidget*>(mUi.mTextEditPool[i]));
    
        QHBoxLayout * horizontalLayout = new QHBoxLayout(this);
        horizontalLayout->addWidget(mUi.mTextEditPool[i]);
        mUi.mWidgetPool[i]->setLayout(horizontalLayout);

    }

    connect(mShaderEditorSignalMapper, SIGNAL(mapped(QWidget *)),
            this, SLOT(OnTextChanged(QWidget *)));

    setLayout(mUi.mMainLayout);
}

bool ShaderEditorWidget::AsyncHasCompilationRequestPending()
{
    bool tmp;
    mCompilationRequestMutex->lock();
    tmp = mCompilationRequestPending;
    mCompilationRequestMutex->unlock();
    return tmp;
}


void ShaderEditorWidget::AsyncSetCompilationRequestPending()
{
    mCompilationRequestMutex->lock();
    mCompilationRequestPending = true;
    mCompilationRequestMutex->unlock();
}

void ShaderEditorWidget::FlushShaderTextEditorToShader(int id)
{
    mCompilationRequestMutex->lock();
    QTextEdit * textEditor = mUi.mTextEditPool[id];
    Pegasus::Shader::IShaderProxy * shaderProxy = mShaderProxyPool[id];
    QString qs = textEditor->toPlainText();
    QByteArray arr = qs.toLocal8Bit();
    const char * source = arr.data();
    int sourceSize = qs.size();
    shaderProxy->SetSource(source, sourceSize);

    //todo, inject here property of shader
    mCompilationRequestPending = false;
    mCompilationRequestMutex->unlock();
}

void ShaderEditorWidget::SignalCompilationError(void * shaderPtr, int line, QString errorString)
{
    Pegasus::Shader::IShaderProxy* target = static_cast<Pegasus::Shader::IShaderProxy*>(shaderPtr);
    ShaderUserData * userData = static_cast<ShaderUserData*>(target->GetUserData());
    if (userData != nullptr)
    {
        userData->InvalidateLine(line);
        int id = FindIndex(target);
        ED_ASSERT(id < mTabCount);
        UpdateSyntaxForLine(id, line);
    }
}

void ShaderEditorWidget::SignalCompilationBegin(void * shader)
{
    Pegasus::Shader::IShaderProxy* target = static_cast<Pegasus::Shader::IShaderProxy*>(shader);
    int id = FindIndex(target);
    ED_ASSERT(id < mTabCount);
    if (target->GetUserData() != nullptr)
    {
        ShaderUserData * shaderUserData = static_cast<ShaderUserData*>(target->GetUserData());
        QSet<int> lineSetCopy = shaderUserData->GetInvalidLineSet();
        shaderUserData->ClearInvalidLines();
        for (int line : lineSetCopy)
        {
            UpdateSyntaxForLine(id, line);
        }
    }
}

void ShaderEditorWidget::UpdateSyntaxForLine(int id, int line)
{
    ED_ASSERT(id < mTabCount);
    QTextDocument * doc = mUi.mTextEditPool[id]->document();
    int lineId = line - 1;
    if (lineId > 0 && lineId < doc->lineCount())
    {
        //prevent a circular event loop by disabling text changed event
        mInternalBlockTextUpdated = true;
        QTextBlock block = doc->findBlockByLineNumber(lineId);
        mUi.mSyntaxHighlighterPool[id]->rehighlightBlock(block);
        mInternalBlockTextUpdated = false;
    }
}

void ShaderEditorWidget::ShaderUIChanged(Pegasus::Shader::IShaderProxy * target)
{
    //TODO any event that requires the shader editor to reupdate
}

int ShaderEditorWidget::FindIndex(Pegasus::Shader::IShaderProxy * target)
{
    for (int i = 0; i < mTabCount; ++i)
    {
        if (target->GetGuid() == mShaderProxyPool[i]->GetGuid())
        {
            return i;
        }
    }
    return -1;
}

int ShaderEditorWidget::FindIndex(QTextEdit * target)
{
    for (int i = 0; i < mTabCount; ++i)
    {
        if (target == mUi.mTextEditPool[i])
        {
            return i;
        }
    }
    return -1;
}

void ShaderEditorWidget::OnSettingsChanged()
{
    for (int i = 0; i < mTabCount; ++i)
    {
        mUi.mSyntaxHighlighterPool[i]->rehighlight();
        QPalette p = mUi.mTextEditPool[i]->palette();
        p.setColor(QPalette::Base, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_BACKGROUND));
        p.setColor(QPalette::Text, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_NORMAL_TEXT));
        mUi.mTextEditPool[i]->setPalette(p);
    }
}

void ShaderEditorWidget::RequestClose(int index)
{
    //remove tab
    mUi.mTabWidget->removeTab(index);

    //update tab indices
    QWidget * tempW = mUi.mWidgetPool[index];
    QTextEdit * tempTextEdit = mUi.mTextEditPool[index];
    QSyntaxHighlighter * mtempSyntax = mUi.mSyntaxHighlighterPool[index];
    static_cast<ShaderSyntaxHighlighter*>(mtempSyntax)->SetShaderUserData(nullptr);

    //copy the pointers back
    for (int i = index; i < MAX_TEXT_TABS - 1; ++i)
    {
        mUi.mWidgetPool[i]  = mUi.mWidgetPool[i + 1];
        mUi.mTextEditPool[i]  = mUi.mTextEditPool[i + 1];
        mUi.mSyntaxHighlighterPool[i]  = mUi.mSyntaxHighlighterPool[i + 1];
        mShaderProxyPool[i] = mShaderProxyPool[i + 1];
    }

    //send deleted pointer to the back of the buffer
    mUi.mWidgetPool[MAX_TEXT_TABS - 1] = tempW;
    mUi.mTextEditPool[MAX_TEXT_TABS - 1] = tempTextEdit;
    mUi.mSyntaxHighlighterPool[MAX_TEXT_TABS - 1] = mtempSyntax;

    --mTabCount;
}

void ShaderEditorWidget::OnTextChanged(QWidget * sender)
{
    //HACK: do not trigger a text update event if the syntax highlighter rehighlights
    if (mInternalBlockTextUpdated)
        return;

    QTextEdit * textEditor = static_cast<QTextEdit*>(sender);
    int id = FindIndex(textEditor);
    if (id != -1)
    {
        // The following snippet is meant to optimize this set of functions:
        //if (!AsyncHasCompilationRequestPending()) 
        //{
        //    AsyncSetCompilationRequestPending();
        //    emit(RequestShaderCompilation(id));
        //}

        //we are reducing here the number of accesses to the mutex
        mCompilationRequestMutex->lock(); 
        if (!mCompilationRequestPending)
        {
            mCompilationRequestPending = true;            
            mCompilationRequestMutex->unlock();
            emit(RequestShaderCompilation(id));
        }
        else
        {
            mCompilationRequestMutex->unlock();
        }
    }
}

void ShaderEditorWidget::RequestOpen(Pegasus::Shader::IShaderProxy * shaderProxy)
{
    if (mTabCount >= MAX_TEXT_TABS)
    {
        //TODO - error, too many text tabs open, throw a signal or something
    }
    else
    {
        int currentTabIndex = FindIndex(shaderProxy);
        if (currentTabIndex == -1) //is this opened already in the pool?
        {
            mShaderProxyPool[mTabCount] = shaderProxy;
            mUi.mTabWidget->addTab(mUi.mWidgetPool[mTabCount], QString(shaderProxy->GetName()));
            currentTabIndex = mUi.mTabWidget->count() - 1;            
            const char * srcChar = nullptr;
            int srcSize = 0;
            shaderProxy->GetSource(&srcChar, srcSize);
            QChar * qchar = new QChar[srcSize];
            for (int i = 0; i < srcSize; ++i)
            {
                qchar[i] = srcChar[i];
            }
            
            QSyntaxHighlighter * mtempSyntax = mUi.mSyntaxHighlighterPool[currentTabIndex];
            static_cast<ShaderSyntaxHighlighter*>(mtempSyntax)->SetShaderUserData(static_cast<ShaderUserData*>(shaderProxy->GetUserData()));

            QString srcQString(qchar, srcSize);
            mUi.mTextEditPool[currentTabIndex]->setText(srcQString);
    
            QPalette p = mUi.mTextEditPool[currentTabIndex]->palette();
            p.setColor(QPalette::Base, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_BACKGROUND));
            p.setColor(QPalette::Text, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_NORMAL_TEXT));
            mUi.mTextEditPool[currentTabIndex]->setPalette(p);
        
            delete[] qchar;            
            mTabCount++;
        }
        mUi.mTabWidget->setCurrentIndex(currentTabIndex); 
    }
}

