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
#include "ShaderLibrary/ShaderTextEditorWidget.h"
#include "ShaderLibrary/ShaderManagerEventListener.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QColor>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTextCharFormat>
#include <QSignalMapper>
#include <QMutex>
#include <QSet>

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
        mUi.mTextEditPool[i] = new ShaderTextEditorWidget();
        mUi.mTextEditPool[i]->setFontFamily(QString("Courier"));

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
    mUi.mTextEditPool[id]->FlushTextToShader();
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
        userData->InsertMessage(line, errorString);
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
    mInternalBlockTextUpdated = true;
    mUi.mTextEditPool[id]->UpdateLineSyntax(line);
    mInternalBlockTextUpdated = false;
}

void ShaderEditorWidget::ShaderUIChanged(Pegasus::Shader::IShaderProxy * target)
{
    //TODO any event that requires the shader editor to reupdate
}

int ShaderEditorWidget::FindIndex(Pegasus::Shader::IShaderProxy * target)
{
    for (int i = 0; i < mTabCount; ++i)
    {
        if (target->GetGuid() == mUi.mTextEditPool[i]->GetShader()->GetGuid())
        {
            return i;
        }
    }
    return -1;
}

int ShaderEditorWidget::FindIndex(ShaderTextEditorWidget * target)
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
        mUi.mTextEditPool[i]->UpdateAllDocumentSyntax();
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
    ShaderTextEditorWidget * tempTextEdit = mUi.mTextEditPool[index];
    tempTextEdit->Uninitialize();
    //copy the pointers back
    for (int i = index; i < MAX_TEXT_TABS - 1; ++i)
    {
        mUi.mWidgetPool[i]  = mUi.mWidgetPool[i + 1];
        mUi.mTextEditPool[i]  = mUi.mTextEditPool[i + 1];
    }

    //send deleted pointer to the back of the buffer
    mUi.mWidgetPool[MAX_TEXT_TABS - 1] = tempW;
    mUi.mTextEditPool[MAX_TEXT_TABS - 1] = tempTextEdit;

    --mTabCount;
}

void ShaderEditorWidget::OnTextChanged(QWidget * sender)
{
    //HACK: do not trigger a text update event if the syntax highlighter rehighlights
    if (mInternalBlockTextUpdated)
        return;

    ShaderTextEditorWidget * textEditor = static_cast<ShaderTextEditorWidget*>(sender);
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
            mUi.mTabWidget->addTab(mUi.mWidgetPool[mTabCount], QString(shaderProxy->GetName()));
            currentTabIndex = mUi.mTabWidget->count() - 1;            

            mUi.mTextEditPool[currentTabIndex]->Initialize(shaderProxy);
    
            QPalette p = mUi.mTextEditPool[currentTabIndex]->palette();
            p.setColor(QPalette::Base, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_BACKGROUND));
            p.setColor(QPalette::Text, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_NORMAL_TEXT));
            mUi.mTextEditPool[currentTabIndex]->setPalette(p);
            mTabCount++;
        }
        mUi.mTabWidget->setCurrentIndex(currentTabIndex); 
    }
}

