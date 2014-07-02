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
#include <QToolBar>
#include <QColor>
#include <QSyntaxHighlighter>
#include <QPushButton>
#include <QRegExp>
#include <QTextCharFormat>
#include <QFontMetrics>
#include <QSignalMapper>
#include <QMutex>
#include <QSet>
#include <QAction>
#include <QStatusBar>

static const char * DOCKABLE_DESC = "Dockable: Allow to be docked when hovering over main window.";
static const char * UNDOCKABLE_DESC = "Undockable: Allow to hover over window w/o docking.";

ShaderEditorWidget::ShaderEditorWidget (QWidget * parent)
: 
    QDockWidget(parent),
    mTabCount(0), 
    mCompilationRequestPending(false), 
    mInternalBlockTextUpdated(false),
    mPinAction(nullptr),
    mSaveAction(nullptr),
    mShaderEditorSignalMapper(nullptr),
    mCompilationRequestMutex(nullptr)
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
    QWidget * mainWidget = new QWidget(this);
    setWidget(mainWidget);

	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);

    setAllowedAreas(Qt::NoDockWidgetArea);
    QToolBar * toolBar = new QToolBar(this);

    //TODO hook up these actions
    QIcon saveIcon(tr(":/ShaderEditor/save.png"));
    QIcon singleIcon(tr(":/ShaderEditor/single.png"));
    QIcon verticalIcon(tr(":/ShaderEditor/vertical.png"));
    QIcon horizontalIcon(tr(":/ShaderEditor/horizontal.png"));

    mSaveAction = toolBar->addAction(saveIcon, tr("save shader to its file"));

    toolBar->addAction(singleIcon, tr("COMING SOON: single view"));
    toolBar->addAction(verticalIcon, tr("COMING SOON: split views vertically"));
    toolBar->addAction(horizontalIcon, tr("COMING SOON: split views horizontally"));

    toolBar->addSeparator();

    mPinIcon.addFile(tr(":/ShaderEditor/pin.png"));
    mUnpinIcon.addFile(tr(":/ShaderEditor/unpin.png"));
    mPinAction = toolBar->addAction(mUnpinIcon, tr(UNDOCKABLE_DESC));

    toolBar->setIconSize(QSize(16,16));

    // connect toolbar actions
    connect(mPinAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalPinActionTriggered()));
    

    mSaveAction->setShortcut(tr("Ctrl+S"));
    connect(mSaveAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalSaveCurrentShader()));

    resize(550, 700);
    setWindowTitle(tr("Shader Editor"));
    setObjectName("ShaderEditor");
    mUi.mMainLayout = new QVBoxLayout();
    mUi.mTabWidget = new QTabWidget(mainWidget);
    mUi.mTabWidget->setTabsClosable(true);
    
    connect(
        mUi.mTabWidget, SIGNAL(tabCloseRequested(int)),
        this, SLOT(RequestClose(int))
    );

    //setup the status bar
    mUi.mStatusBar = new QStatusBar(this);
    
    
    mUi.mMainLayout->addWidget(toolBar);
    mUi.mMainLayout->addWidget(mUi.mTabWidget);
    mUi.mMainLayout->addWidget(mUi.mStatusBar);
    
    for (int i = 0; i < MAX_TEXT_TABS; ++i)
    {
        mUi.mWidgetPool[i] = new QWidget();
        mUi.mTextEditPool[i] = new ShaderTextEditorWidget();

        connect(mUi.mTextEditPool[i], SIGNAL(textChanged()),
                mShaderEditorSignalMapper, SLOT(map()));
    
        mShaderEditorSignalMapper->setMapping(mUi.mTextEditPool[i], static_cast<QWidget*>(mUi.mTextEditPool[i]));
    
        QHBoxLayout * horizontalLayout = new QHBoxLayout(mainWidget);
        horizontalLayout->addWidget(mUi.mTextEditPool[i]);
        mUi.mWidgetPool[i]->setLayout(horizontalLayout);

    }
    connect(mShaderEditorSignalMapper, SIGNAL(mapped(QWidget *)),
            this, SLOT(OnTextChanged(QWidget *)));

    mainWidget->setLayout(mUi.mMainLayout);
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
    if (id != -1) //is there a ui element for this shader?
    {
        mUi.mTextEditPool[id]->FlushTextToShader();
    }
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
        PostStatusBarMessage(errorString);
        if (id != -1) //is there a ui element for this shader?
        {
            UpdateSyntaxForLine(id, line);
        }
    }
}

void ShaderEditorWidget::SignalLinkingEvent(void * program, QString message, int eventType)
{
    Pegasus::Shader::IProgramProxy * target = static_cast<Pegasus::Shader::IProgramProxy*>(program);
    ED_ASSERT(target != nullptr);
    ProgramUserData * programUserData = static_cast<ProgramUserData*>(target->GetUserData());
    if (programUserData != nullptr)
    {
        if (static_cast<Pegasus::Shader::LinkingEvent::Type>(eventType) != Pegasus::Shader::LinkingEvent::LINKING_SUCCESS)
        {
            //do not opaque the previous string if this one is empty
            if (mUi.mStatusBarMessage == "" && message != "")
            {
                PostStatusBarMessage(message);
            }
        }
        else
        {
             PostStatusBarMessage("");
        }
    }
}

void ShaderEditorWidget::SignalCompilationBegin(void * shader)
{
    Pegasus::Shader::IShaderProxy* target = static_cast<Pegasus::Shader::IShaderProxy*>(shader);
    int id = FindIndex(target);
    ED_ASSERT(id < mTabCount);
    if (id != -1 && target->GetUserData() != nullptr)
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

void ShaderEditorWidget::SignalCompilationEnd(QString log)
{
    //update the status bar with whichever compilation error.
    PostStatusBarMessage(log);
}

void ShaderEditorWidget::SignalPinActionTriggered()
{
    ED_ASSERT(mPinAction != nullptr);
    if (allowedAreas() == Qt::NoDockWidgetArea)
    {
        setAllowedAreas(Qt::AllDockWidgetAreas);
        mPinAction->setIcon(mPinIcon);
        mPinAction->setText(tr(DOCKABLE_DESC));
    }
    else if (allowedAreas() == Qt::AllDockWidgetAreas)
    {
        setAllowedAreas(Qt::NoDockWidgetArea);
        mPinAction->setIcon(mUnpinIcon);
        mPinAction->setText(tr(UNDOCKABLE_DESC));
    } 
}

void ShaderEditorWidget::SignalSaveCurrentShader()
{
    ED_ASSERT(mUi.mTabWidget != nullptr);
    int idx = mUi.mTabWidget->currentIndex();
    if (idx >= 0)
    {
        ShaderTextEditorWidget * texEd = mUi.mTextEditPool[idx];
        Pegasus::Shader::IShaderProxy * shader = texEd->GetShader();
        if (shader != nullptr)
        {
            //ensure that no compilation is happening while saving this file
            mCompilationRequestMutex->lock();
            shader->SaveSourceToFile();
            mCompilationRequestMutex->unlock();
        }
    }
}

void ShaderEditorWidget::SignalSavedFileSuccess()
{
    PostStatusBarMessage(tr("File Saved."));
}

void ShaderEditorWidget::SignalSavedFileIoError(int ioError, QString msg)
{
    QString barMsg = tr("IO ERROR: file not saved correctly: ") + msg;
    PostStatusBarMessage(barMsg);
}

void ShaderEditorWidget::UpdateSyntaxForLine(int id, int line)
{
    ED_ASSERT(id < mTabCount);
    mInternalBlockTextUpdated = true;
    mUi.mTextEditPool[id]->UpdateLineSyntax(line);
    mInternalBlockTextUpdated = false;
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

void ShaderEditorWidget::PostStatusBarMessage(const QString& message)
{
    mUi.mStatusBarMessage = message;
    mUi.mStatusBar->showMessage(message);
}

void ShaderEditorWidget::SynchronizeTextEditWidgetSyntaxStyle(int i)
{
    QPalette p = mUi.mTextEditPool[i]->palette();
    p.setColor(QPalette::Base, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_BACKGROUND));
    p.setColor(QPalette::Text, Editor::GetInstance().GetSettings()->GetShaderSyntaxColor(Settings::SYNTAX_NORMAL_TEXT));
    int fontSize = Editor::GetInstance().GetSettings()->GetShaderEditorFontSize();
    int tabStop  = Editor::GetInstance().GetSettings()->GetShaderEditorTabSize();
    QFont f("Courier");
    f.setPointSize(fontSize);
    
    QFontMetrics metrics(f);

    mUi.mTextEditPool[i]->setFont(f);
    mUi.mTextEditPool[i]->setPalette(p);
    mUi.mTextEditPool[i]->setTabStopWidth(tabStop * metrics.width(' '));
    mUi.mTextEditPool[i]->UpdateAllDocumentSyntax();
}

void ShaderEditorWidget::OnSettingsChanged()
{
    for (int i = 0; i < mTabCount; ++i)
    {
        SynchronizeTextEditWidgetSyntaxStyle(i);
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

        CompileShader(id);
        //we are reducing here the number of accesses to the mutex
      
    }
}

void ShaderEditorWidget::CompileShader(int id)
{
    // The following snippet is meant to optimize this set of functions:
    //if (!AsyncHasCompilationRequestPending()) 
    //{
    //    AsyncSetCompilationRequestPending();
    //    emit(RequestShaderCompilation(id));
    //}
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
            SynchronizeTextEditWidgetSyntaxStyle(currentTabIndex); 
            mTabCount++;
        }
        mUi.mTabWidget->setCurrentIndex(currentTabIndex); 
    }
}

