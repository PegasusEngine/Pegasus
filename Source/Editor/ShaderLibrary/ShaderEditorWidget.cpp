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
#include "ShaderLibrary/ShaderTextEditorTreeWidget.h"
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
    mCompilationRequestPending(false), 
    mInternalBlockTextUpdated(false),
    mOpenShaderCount(0),
    mPreviousTabIndex(-1),
    mCloseViewAction(nullptr),
    mPinAction(nullptr),
    mSaveAction(nullptr),
    mHorizontalAction(nullptr),
    mVerticalAction(nullptr),
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
    QWidget * mainWidget = new QWidget(this);
    mUi.mMainLayout = new QVBoxLayout();
    mainWidget->setLayout(mUi.mMainLayout);
    setWidget(mainWidget);

	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);

    setAllowedAreas(Qt::NoDockWidgetArea);

    QToolBar * toolBar = new QToolBar(mainWidget);
    //TODO hook up these actions
    QIcon saveIcon(tr(":/ShaderEditor/save.png"));
    QIcon singleIcon(tr(":/ShaderEditor/single.png"));
    QIcon verticalIcon(tr(":/ShaderEditor/vertical.png"));
    QIcon horizontalIcon(tr(":/ShaderEditor/horizontal.png"));

    mSaveAction = toolBar->addAction(saveIcon, tr("save shader to its file"));

    mCloseViewAction  = toolBar->addAction(singleIcon, tr("close current view"));
    mVerticalAction   = toolBar->addAction(verticalIcon, tr("split views vertically"));
    mHorizontalAction = toolBar->addAction(horizontalIcon, tr("split views horizontally"));

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

    mUi.mTabWidget = new QTabBar(mainWidget);
    mUi.mTabWidget->setTabsClosable(true);
    
    connect(
        mUi.mTabWidget, SIGNAL(tabCloseRequested(int)),
        this, SLOT(RequestClose(int))
    );
 
    //setup the tree editor
    QSignalMapper * textChangedMapper = new QSignalMapper(this);
    QSignalMapper * selectedSignalMapper = new QSignalMapper(this);
    mUi.mTextEditorSignals.Initialize(
        textChangedMapper,
        selectedSignalMapper
    );

    mUi.mTreeEditor = new ShaderTextEditorTreeWidget(mUi.mTextEditorSignals, mainWidget);

    connect(
        textChangedMapper, SIGNAL(mapped(QWidget *)),
        this, SLOT(OnTextChanged(QWidget *))
    );

    connect(
        selectedSignalMapper, SIGNAL(mapped(QWidget *)),
        this, SLOT(OnTextWindowSelected(QWidget*))
    );

    
    // QT's definition of horizontal is different from the user... thats why we flip the
    // the callbacks
    connect(
        mCloseViewAction, SIGNAL(triggered(bool)),
        mUi.mTreeEditor, SLOT(CloseSplit())
    );

    connect(
        mVerticalAction, SIGNAL(triggered(bool)),
        mUi.mTreeEditor, SLOT(HorizontalSplit())
    );

    connect(
        mHorizontalAction, SIGNAL(triggered(bool)),
        mUi.mTreeEditor, SLOT(VerticalSplit())
    );

    connect(
        mUi.mTabWidget, SIGNAL(currentChanged(int)),
        this,   SLOT(SignalViewShader(int))
    );
    //setup the status bar
    mUi.mStatusBar = new QStatusBar(mainWidget);
    
    mUi.mMainLayout->addWidget(toolBar);
    mUi.mMainLayout->addWidget(mUi.mTabWidget);
    mUi.mMainLayout->addWidget(mUi.mTreeEditor);    
    mUi.mMainLayout->addWidget(mUi.mStatusBar);

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
    if (id >= 0 && id < mOpenShaderCount) //is there a ui element for this shader?
    {
        ShaderTextEditorWidget * editor = mUi.mTreeEditor->FindShadersInEditors(mOpenedShaders[id]);
        if (editor != nullptr)
        {
            editor->FlushTextToShader();
        }
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
        PostStatusBarMessage(errorString);
        ShaderTextEditorWidget * editor = mUi.mTreeEditor->FindShadersInEditors(target);
        if (editor != nullptr)
        {
            UpdateSyntaxForLine(editor, line);
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
    ShaderTextEditorWidget * editor = mUi.mTreeEditor->FindShadersInEditors(target);
    if (editor != nullptr && target->GetUserData() != nullptr)
    {
        ShaderUserData * shaderUserData = static_cast<ShaderUserData*>(target->GetUserData());
        QSet<int> lineSetCopy = shaderUserData->GetInvalidLineSet();
        shaderUserData->ClearInvalidLines();
        for (int line : lineSetCopy)
        {
            UpdateSyntaxForLine(editor, line);
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
    int idx = mUi.mTabWidget->currentIndex();
    if (idx >= 0 && idx < mOpenShaderCount)
    {
        Pegasus::Shader::IShaderProxy * shader = mOpenedShaders[idx];
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

void ShaderEditorWidget::SignalViewShader(int tabId)
{
    if (tabId != -1)
    {
        ED_ASSERT(tabId >= 0 && tabId < mOpenShaderCount);

        ShaderTextEditorWidget * editor = nullptr;

        if (mPreviousTabIndex >= 0 && mPreviousTabIndex < mOpenShaderCount)
        {
            editor = mUi.mTreeEditor->FindShadersInEditors(mOpenedShaders[mPreviousTabIndex]);
        }

        mUi.mTreeEditor->DisplayShader(mOpenedShaders[tabId], editor);

        mPreviousTabIndex = tabId;
    }
}

void ShaderEditorWidget::SignalSavedFileIoError(int ioError, QString msg)
{
    QString barMsg = tr("IO ERROR: file not saved correctly: ") + msg;
    PostStatusBarMessage(barMsg);
}

void ShaderEditorWidget::UpdateSyntaxForLine(ShaderTextEditorWidget * editor, int line)
{
    mInternalBlockTextUpdated = true;
    editor->UpdateLineSyntax(line);
    mInternalBlockTextUpdated = false;
}

int ShaderEditorWidget::FindIndex(Pegasus::Shader::IShaderProxy * target)
{
    for (int i = 0; i < mOpenShaderCount; ++i)
    {
        if (target->GetGuid() == mOpenedShaders[i]->GetGuid())
        {
            return i;
        }
    }
    return -1;
}

int ShaderEditorWidget::FindIndex(ShaderTextEditorWidget * target)
{
    Pegasus::Shader::IShaderProxy * shader = target->GetShader();
    if (shader != nullptr)
    {
        return FindIndex(shader);
    }
    else
    {
        return -1;
    }
}

void ShaderEditorWidget::PostStatusBarMessage(const QString& message)
{
    mUi.mStatusBarMessage = message;
    mUi.mStatusBar->showMessage(message);
}

void ShaderEditorWidget::OnSettingsChanged()
{
    mUi.mTreeEditor->ForceUpdateAllStyles();
}

void ShaderEditorWidget::RequestClose(int index)
{
    ED_ASSERT(index >= 0 && index < mOpenShaderCount);
    //remove tab
    mUi.mTreeEditor->HideShader(mOpenedShaders[index]);

    //compress the opened shader list
    for (int i = index; i < mOpenShaderCount - 1; ++i)
    {
        ED_ASSERT(i < MAX_OPENED_SHADERS);
        mOpenedShaders[i] = mOpenedShaders[i + 1];
        
    }
    --mOpenShaderCount;
    mUi.mTabWidget->removeTab(index);
}

void ShaderEditorWidget::OnTextChanged(QWidget * sender)
{
    //do not trigger a text update event if the syntax highlighter rehighlights
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

void ShaderEditorWidget::OnTextWindowSelected(QWidget * sender)
{
    ShaderTextEditorWidget * textEditor = static_cast<ShaderTextEditorWidget*>(sender);
    if (textEditor != nullptr)
    {
        Pegasus::Shader::IShaderProxy * shader = textEditor->GetShader();
        if (shader != nullptr)
        {
            int i = FindIndex(shader);
            if (i >= 0 && i < mUi.mTabWidget->count() && i != mUi.mTabWidget->currentIndex())
            {
                mUi.mTabWidget->setCurrentIndex(i);
            }
        }
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
    int currentTabIndex = FindIndex(shaderProxy);
    if (currentTabIndex == -1)
    {
        if (mOpenShaderCount >= MAX_OPENED_SHADERS)
        {
            ED_LOG("Maximum number of opened shaders reached!");
        }
        else
        {
            //try to find and focus the previous shader selected.
            //this will make the shader selected appear in the screen.
            currentTabIndex = mUi.mTabWidget->count();

            //  pattern must be appending to the end
            ED_ASSERT(currentTabIndex == mOpenShaderCount);
            mOpenedShaders[currentTabIndex] = shaderProxy;
            ++mOpenShaderCount;

            //is this opened already in the pool?
            mUi.mTabWidget->addTab(QString(shaderProxy->GetName()));

        }
    }
    mUi.mTabWidget->setCurrentIndex(currentTabIndex); 
    if (currentTabIndex == mUi.mTabWidget->currentIndex())
    {
        SignalViewShader(currentTabIndex);
    }
}

void ShaderEditorWidget::UpdateUIForAppFinished()
{
    //compress the opened shader list
    while (mOpenShaderCount > 0)
    {
        RequestClose(0);
    }
}

