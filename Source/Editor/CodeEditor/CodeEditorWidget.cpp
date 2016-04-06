/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   CodeEditorWidget.cpp	
//! \author	Kleber Garcia
//! \date	30th Match 2014
//! \brief	Code Editor IDE

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Settings/Settings.h"
#include "CodeEditor/CodeEditorWidget.h"
#include "CodeEditor/CodeTextEditorWidget.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "CodeEditor/CodeTextEditorTreeWidget.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "Widgets/NodeFileTabBar.h"
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
#include <QCheckBox>
#include <QLineEdit>

static const char * DOCKABLE_DESC = "Dockable: Allow to be docked when hovering over main window.";
static const char * UNDOCKABLE_DESC = "Undockable: Allow to hover over window w/o docking.";
static const char * INSTANT_COMPILATION_DESC_ON = "Turn instant compilation ON\nInstant Compilation Mode: code gets compiled at every key stroke, in an efficient manner. ";
static const char * INSTANT_COMPILATION_DESC_OFF = "Turn instant compilation OFF\nInstant Compilation Mode: code gets compiled at every key stroke, in an efficient manner. ";



CodeEditorWidget::CodeEditorWidget (QWidget * parent, Editor* editor)
: 
    PegasusDockWidget(parent, editor),
    mCompilationRequestPending(false), 
    mInternalBlockTextUpdated(false),
    mCloseViewAction(nullptr),
    mPinAction(nullptr),
    mSaveAction(nullptr),
    mHorizontalAction(nullptr),
    mVerticalAction(nullptr),
    mCompilationRequestMutex(nullptr),
    mInstantCompilationFlag(true), //on by default
    mSavedInstantCompilationFlag(true),
    mCompilationPolicy(Pegasus::Core::ISourceCodeProxy::POLICY_USER_DEFINED)
{
    mCompilationRequestMutex = new QMutex();
}

CodeEditorWidget::~CodeEditorWidget()
{
    delete mCompilationRequestMutex;
}

void CodeEditorWidget::SetupUi()
{
    setFocusPolicy(Qt::NoFocus);
    QWidget * mainWidget = new QWidget(this);
    mUi.mMainLayout = new QVBoxLayout();
    mainWidget->setLayout(mUi.mMainLayout);
    setWidget(mainWidget);

    setAllowedAreas(Qt::NoDockWidgetArea);

    QToolBar * toolBar = new QToolBar(mainWidget);
    //TODO hook up these actions
    QIcon saveIcon(tr(":/CodeEditor/save.png"));
    QIcon singleIcon(tr(":/CodeEditor/single.png"));
    QIcon verticalIcon(tr(":/CodeEditor/vertical.png"));
    QIcon horizontalIcon(tr(":/CodeEditor/horizontal.png"));
    QIcon compileIcon(tr(":/CodeEditor/compile.png"));
    QIcon searchIcon(tr(":/CodeEditor/search.png"));
    QIcon closeIcon(tr(":/TimelineToolbar/Remove16.png"));

    mSaveAction = toolBar->addAction(saveIcon, tr("save this source to its file"));

    mCloseViewAction  = toolBar->addAction(singleIcon, tr("close current view"));
    mVerticalAction   = toolBar->addAction(verticalIcon, tr("split views vertically"));
    mHorizontalAction = toolBar->addAction(horizontalIcon, tr("split views horizontally"));
    mCompileAction    = toolBar->addAction(compileIcon, tr("compile current code (Ctrl + F7)"));
    mCompileAction->setShortcut(tr("Ctrl+F7"));

    mSearchAction     = toolBar->addAction(searchIcon, tr("search text in current window."));
    mSearchAction->setShortcut(tr("Ctrl+f"));

    toolBar->addSeparator();

    mPinIcon.addFile(tr(":/CodeEditor/pin.png"));
    mUnpinIcon.addFile(tr(":/CodeEditor/unpin.png"));
    mPinAction = toolBar->addAction(mUnpinIcon, tr(UNDOCKABLE_DESC));   

    mOnInstantCompilationIcon.addFile(tr(":/Toolbar/Edit/switchon.png"));
    mOffInstantCompilationIcon.addFile(tr(":/Toolbar/Edit/switchoff.png"));

    mInstantCompilationAction = toolBar->addAction(mOnInstantCompilationIcon, INSTANT_COMPILATION_DESC_OFF);
    toolBar->setIconSize(QSize(16,16));

    // connect toolbar actions
    connect(mPinAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalPinActionTriggered()));

    connect(mInstantCompilationAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalInstantCompilationActionTriggered()));
    
    connect(mSaveAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalSaveCurrentCode()));

    connect(mSearchAction, SIGNAL(triggered(bool)),
            this, SLOT(FocusSearch()));

    resize(550, 700);
    setWindowTitle(tr("Code Editor"));
    setObjectName("CodeEditor");

    mUi.mTabWidget = new NodeFileTabBar(this);
    
    connect(
        mUi.mTabWidget, SIGNAL(RuntimeObjectRemoved(AssetInstanceHandle, QObject*)),
        this, SLOT(RequestClose(AssetInstanceHandle, QObject*))
    );

    connect(
        mUi.mTabWidget, SIGNAL(DisplayRuntimeObject(AssetInstanceHandle)),
        this,   SLOT(SignalViewCode(AssetInstanceHandle))
    );

    connect(
        mUi.mTabWidget, SIGNAL(SaveRuntimeObject(int)),
        this,   SLOT(SignalSaveTab(int))
    );

    connect(
        mUi.mTabWidget, SIGNAL(DiscardObjectChanges(AssetInstanceHandle)),
        this,   SLOT(SignalDiscardObjectChanges(AssetInstanceHandle))
    );

    //setup the tree editor
    QSignalMapper * textChangedMapper = new QSignalMapper(this);
    QSignalMapper * selectedSignalMapper = new QSignalMapper(this);
    mUi.mTextEditorSignals.Initialize(
        textChangedMapper,
        selectedSignalMapper
    );

    mUi.mTreeEditor = new CodeTextEditorTreeWidget(mUi.mTextEditorSignals, mainWidget);

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
        mCompileAction, SIGNAL(triggered(bool)),
        this, SLOT(SignalCompileCurrentCode())
    );


    mUi.mFindTextWidget = new QWidget(this);
    QLabel* findTextLabel = new QLabel(tr("Find:"),mUi.mFindTextWidget);
    QLineEdit* lineEdit = new QLineEdit(mUi.mFindTextWidget);
    mUi.mSearchWindowLineEdit = lineEdit;
    QHBoxLayout* findBoxLayout = new QHBoxLayout(mUi.mFindTextWidget);
    QToolButton* findNext = new QToolButton(mUi.mFindTextWidget);
    findNext->setText(tr("Next"));
    QToolButton* findPrev = new QToolButton(mUi.mFindTextWidget);
    findPrev->setText(tr("Prev"));
    QToolButton* closeSearch = new QToolButton(mUi.mFindTextWidget);
    closeSearch->setIcon(closeIcon);
    connect(closeSearch, SIGNAL(clicked()),
            this, SLOT(CloseSearch()));
    QCheckBox* caseSense = new QCheckBox(mUi.mFindTextWidget); 
    caseSense->setText(tr("Case Sensitive"));

    mUi.mFindTextWidget->setLayout(findBoxLayout);
    findBoxLayout->addWidget(findTextLabel);
    findBoxLayout->addWidget(lineEdit);
    findBoxLayout->addWidget(findNext);
    findBoxLayout->addWidget(findPrev);
    findBoxLayout->addWidget(caseSense);
    findBoxLayout->addWidget(closeSearch);
    mUi.mFindTextWidget->hide();


    //setup the status bar
    mUi.mStatusBar = new QStatusBar(mainWidget);
    
    mUi.mMainLayout->addWidget(toolBar);
    mUi.mMainLayout->addWidget(mUi.mTabWidget);
    mUi.mMainLayout->addWidget(mUi.mTreeEditor);    
    mUi.mMainLayout->addWidget(mUi.mFindTextWidget);
    mUi.mMainLayout->addWidget(mUi.mStatusBar);

}

bool CodeEditorWidget::AsyncHasCompilationRequestPending()
{
    bool tmp;
    mCompilationRequestMutex->lock();
    tmp = mCompilationRequestPending;
    mCompilationRequestMutex->unlock();
    return tmp;
}


void CodeEditorWidget::AsyncSetCompilationRequestPending()
{
    mCompilationRequestMutex->lock();
    mCompilationRequestPending = true;
    mCompilationRequestMutex->unlock();
}

void CodeEditorWidget::SignalCompileCurrentCode()
{
    int idx = mUi.mTabWidget->GetCurrentIndex();
    if (idx >= 0)
    {
        AssetInstanceHandle handle = mUi.mTabWidget->GetTabObject(idx);
        CompileCode(handle);
    }
}

void CodeEditorWidget::FocusSearch()
{
    mUi.mFindTextWidget->show();
    mUi.mSearchWindowLineEdit->setFocus();
}

void CodeEditorWidget::CloseSearch()
{
    mUi.mFindTextWidget->hide();
}

void CodeEditorWidget::SignalCompilationError(AssetInstanceHandle handle, int line, QString errorString)
{
    if (handle.IsValid())
    {
        SourceState* ss = mHandleMap[handle];
        ss->errorLines.insert(line);
        ss->errorMessages.insert(line, errorString);
        PostStatusBarMessage(QString("(%1): ").arg(line) + errorString);
        CodeTextEditorWidget * editor = mUi.mTreeEditor->FindCodeInEditors(ss);
        if (editor != nullptr)
        {
            UpdateSyntaxForLine(editor, line);
        }
    }
}

void CodeEditorWidget::SignalLinkingEvent(QString message, int eventType)
{
    if (static_cast<Pegasus::Core::CompilerEvents::LinkingEvent::Type>(eventType) != Pegasus::Core::CompilerEvents::LinkingEvent::LINKING_SUCCESS)
    {
        PostStatusBarMessage(message);
    }
}

void CodeEditorWidget::SignalCompilationBegin(AssetInstanceHandle handle)
{
    SourceStateMap::iterator it = mHandleMap.find(handle);
    if (it != mHandleMap.end())
    {
        SourceState* ss = it.value();
        CodeTextEditorWidget * editor = mUi.mTreeEditor->FindCodeInEditors(ss);
        if (editor != nullptr)
        {
            PostStatusBarMessage("");            
            QSet<int> lineSetCopy = ss->errorLines;
            ss->errorLines.clear();
            ss->errorMessages.clear();
            for (int line : lineSetCopy)
            {
                UpdateSyntaxForLine(editor, line);
            }
        }
    }
}

void CodeEditorWidget::CompilationRequestReceived()
{
    mCompilationRequestMutex->lock();
    mCompilationRequestPending = false;
    mCompilationRequestMutex->unlock();
}

void CodeEditorWidget::SignalCompilationEnd(QString log)
{
    //update the status bar with whichever compilation error.
    if (log != "")
    {
        PostStatusBarMessage(log);
    }
    else
    {
         PostStatusBarMessage("File Compiled Successfuly.");
    }
}

void CodeEditorWidget::SignalPinActionTriggered()
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

void CodeEditorWidget::SetInstantCompilationState(bool state)
{
    if (state)
    {
        mInstantCompilationAction->setIcon(mOnInstantCompilationIcon);
        mInstantCompilationAction->setToolTip(tr(INSTANT_COMPILATION_DESC_OFF));
    }
    else
    {   
        mInstantCompilationAction->setIcon(mOffInstantCompilationIcon);
        mInstantCompilationAction->setToolTip(tr(INSTANT_COMPILATION_DESC_ON));
    }
    mInstantCompilationFlag = state;
}

void CodeEditorWidget::EnableModeInstantCompilationButton(bool enableMode)
{
    mInstantCompilationAction->setDisabled(!enableMode);
}

void CodeEditorWidget::SignalInstantCompilationActionTriggered()
{
    SetInstantCompilationState(!mInstantCompilationFlag);
}

void CodeEditorWidget::OnSaveFocusedObject()
{
    SignalSaveCurrentCode();
}

void CodeEditorWidget::SignalSaveCurrentCode()
{
    int idx = mUi.mTabWidget->GetCurrentIndex();
    SignalSaveTab(idx);
}

void CodeEditorWidget::SignalSaveTab(int idx)
{
    PostStatusBarMessage("");    
    if (idx >= 0)
    {
        AssetInstanceHandle handle = mUi.mTabWidget->GetTabObject(idx);
        SourceState* ss = mHandleMap[handle];

        //dump ui document data to the internal node so we can save
        QString plainText = ss->document->toPlainText();
        SourceIOMessageController::Message smsg;
        smsg.SetMessageType(SourceIOMessageController::Message::SET_SOURCE);
        smsg.SetHandle(handle);
        smsg.SetSourceText(plainText);
        SendSourceIoMessage(smsg);

        //Save the document
        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::SAVE_ASSET);
        msg.SetObject(handle);
        SendAssetIoMessage(msg);

    }
}

void CodeEditorWidget::SignalDiscardObjectChanges(AssetInstanceHandle handle)
{
    //Internally reload the asset.
    AssetIOMessageController::Message msg(AssetIOMessageController::Message::RELOAD_FROM_ASSET);
    msg.SetObject(handle);
    SendAssetIoMessage(msg);
    
    //Send a compilation request
    SourceIOMessageController::Message smsg;
    smsg.SetMessageType(SourceIOMessageController::Message::COMPILE_SOURCE);
    smsg.SetHandle(handle);
    SendSourceIoMessage(smsg);
}

void CodeEditorWidget::SignalSavedFileSuccess()
{
    PostStatusBarMessage(tr("File Saved."));
}

void CodeEditorWidget::SignalViewCode(AssetInstanceHandle handle)
{
    if (handle.IsValid())
    {
        SourceState* ss = mHandleMap[handle];
        mInternalBlockTextUpdated = true;
        mUi.mTreeEditor->DisplayCode(ss);
        mInternalBlockTextUpdated = false;

        UpdateInstantCompilationButton(ss);
    }
}

void CodeEditorWidget::SignalSavedFileIoError(int ioError, QString msg)
{
    QString barMsg = tr("IO ERROR: file not saved correctly: ") + msg;
    PostStatusBarMessage(barMsg);
}

void CodeEditorWidget::UpdateSyntaxForLine(CodeTextEditorWidget * editor, int line)
{
    mInternalBlockTextUpdated = true;
    editor->UpdateLineSyntax(line);
    mInternalBlockTextUpdated = false;
}

void CodeEditorWidget::PostStatusBarMessage(const QString& message)
{
    // do not opaque the previous error
    if (message == "" || mUi.mStatusBarMessage == "")
    {
        mUi.mStatusBarMessage = message;
        mUi.mStatusBar->showMessage(message);
    }
}

void CodeEditorWidget::OnReceiveAssetIoMessage(AssetIOMessageController::Message::IoResponseMessage id)
{
    switch(id)
    {
    case AssetIOMessageController::Message::IO_SAVE_SUCCESS:
        // This might create a race condition, if the user changes a tab wile the file is saving... to fix this we need
        // to pass the object around... if this becomes a problem then we could fix it later...
        mUi.mTabWidget->ClearCurrentDirty();
        PostStatusBarMessage(tr("Saved file successfully."));
        break;
    case AssetIOMessageController::Message::IO_SAVE_ERROR:
        PostStatusBarMessage(tr("IO Error saving file."));
        break;
    case AssetIOMessageController::Message::IO_NEW_SUCCESS:
        PostStatusBarMessage(tr(""));
        break;
    case AssetIOMessageController::Message::IO_NEW_ERROR:
        PostStatusBarMessage(tr("IO Error creating new code file."));
        break;
    default:
        ED_FAILSTR("Unkown IO Asset message id %d", id);
    }
}

void CodeEditorWidget::OnSettingsChanged()
{
    mUi.mTreeEditor->ForceUpdateAllStyles();
}


void CodeEditorWidget::RequestClose(AssetInstanceHandle handle, QObject* extraData)
{

    SourceState* ss = mHandleMap[handle];

    mInternalBlockTextUpdated = true;
    mUi.mTreeEditor->HideCode(ss);
    mInternalBlockTextUpdated = false;

    mHandleMap.remove(handle);

    delete ss->document;
    ss->document = nullptr;
    delete ss;


    //send a message to the render thread to close this code safely
    AssetIOMessageController::Message msg;
    msg.SetMessageType(AssetIOMessageController::Message::CLOSE_ASSET);
    msg.SetObject(handle);
    SendAssetIoMessage(msg);
}

void CodeEditorWidget::OnTextChanged(QWidget * sender)
{
    //do not trigger a text update event if the syntax highlighter rehighlights
    if (mInternalBlockTextUpdated)
        return;

    mUi.mTabWidget->MarkCurrentAsDirty();
   
    if (mInstantCompilationFlag)
    {
        CodeTextEditorWidget * textEditor = static_cast<CodeTextEditorWidget*>(sender);
        if (textEditor->GetCode() != nullptr)
        {
            CompileCode(textEditor->GetCode()->handle);
        }
    }
}

void CodeEditorWidget::OnTextWindowSelected(QWidget * sender)
{
    CodeTextEditorWidget * textEditor = static_cast<CodeTextEditorWidget*>(sender);
    if (textEditor != nullptr)
    {
        SourceState * code = textEditor->GetCode();
        if (code != nullptr)
        {
            int i = mUi.mTabWidget->FindIndex(code->handle);
            if (i >= 0 && i < mUi.mTabWidget->GetTabCount() && i != mUi.mTabWidget->GetCurrentIndex())
            {
                mUi.mTabWidget->SetCurrentIndex(i);
            }
        }
    }
}

void CodeEditorWidget::CompileCode(AssetInstanceHandle handle)
{
    if (!handle.IsValid() ) return;
    mCompilationRequestMutex->lock(); 
    if (!mCompilationRequestPending)
    {
        mCompilationRequestPending = true;            
        mCompilationRequestMutex->unlock();
        emit(RequestCompilationBegin());
        SourceIOMessageController::Message msg;
        msg.SetMessageType(SourceIOMessageController::Message::SET_SOURCE_AND_COMPILE_SOURCE);
        msg.SetSourceText(mHandleMap[handle]->document->toPlainText());
        msg.SetHandle(handle);
        emit (SendSourceIoMessage(msg));
    }
    else
    {
        mCompilationRequestMutex->unlock();
    }
}

void CodeEditorWidget::UpdateInstantCompilationButton(SourceState* ss)
{
    if (ss != nullptr && mCompilationPolicy != ss->compilationPolicy)
    {
        mCompilationPolicy = ss->compilationPolicy;
        if (mCompilationPolicy == Pegasus::Core::ISourceCodeProxy::POLICY_USER_DEFINED)
        {
            if (!mInstantCompilationAction->isEnabled())
            {
                EnableModeInstantCompilationButton(true);
                SetInstantCompilationState(mSavedInstantCompilationFlag);
            }
        }
        else
        {
            ED_ASSERT(mCompilationPolicy == Pegasus::Core::ISourceCodeProxy::POLICY_FORCE_ON_SAVE);
            mSavedInstantCompilationFlag = mInstantCompilationFlag;
            SetInstantCompilationState(false);
            EnableModeInstantCompilationButton(false);
        }
    }

}

void CodeEditorWidget::OnOpenObject(AssetInstanceHandle objectHandle, const QString& displayName, const QVariant& initData)
{
    ED_ASSERT (objectHandle.IsValid());
    show();
    activateWindow();

    SourceStateMap::iterator it = mHandleMap.find(objectHandle);
    //opened for the first time?
    if (it == mHandleMap.end())
    {
        QVariantMap arguments = initData.toMap();
        Pegasus::Core::ISourceCodeProxy::CompilationPolicy compilationPolicy =
           static_cast<Pegasus::Core::ISourceCodeProxy::CompilationPolicy>(arguments["CompilationPolicy"].toInt());

        QString plainText = arguments["Source"].toString();
        
        SourceState* ss = new SourceState();
        ss->handle = objectHandle;
        ss->document = new QTextDocument();
        ss->document->setDocumentLayout(new QPlainTextDocumentLayout(ss->document));
        ss->compilationPolicy = compilationPolicy;
        //dump code contents to document text
        //set the text of the current text editor
        mInternalBlockTextUpdated = true;
        ss->document->setPlainText(plainText);
        mInternalBlockTextUpdated = false;
        mHandleMap.insert(objectHandle, ss);
    }

    mUi.mTabWidget->Open(objectHandle, displayName);
    CompileCode(objectHandle);
}

bool CodeEditorWidget::HasAnyChildFocus() const
{
    return mUi.mTreeEditor->HasAnyChildFocus();
}

void CodeEditorWidget::OnUIForAppClosed()
{
    //compress the opened code list
    while (mUi.mTabWidget->GetTabCount())
    {
        mUi.mTabWidget->Close(0);
    }
    mCompilationRequestPending = false; //kill any compilation request
}

const Pegasus::PegasusAssetTypeDesc*const* CodeEditorWidget::GetTargetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gTypes[] = {
             &Pegasus::ASSET_TYPE_H_SHADER    
            ,&Pegasus::ASSET_TYPE_VS_SHADER   
            ,&Pegasus::ASSET_TYPE_PS_SHADER   
            ,&Pegasus::ASSET_TYPE_GS_SHADER   
            ,&Pegasus::ASSET_TYPE_TCS_SHADER  
            ,&Pegasus::ASSET_TYPE_TES_SHADER  
            ,&Pegasus::ASSET_TYPE_CS_SHADER   
            ,&Pegasus::ASSET_TYPE_BLOCKSCRIPT  
            ,&Pegasus::ASSET_TYPE_H_BLOCKSCRIPT
            ,nullptr
    };
    
    return gTypes;
}
