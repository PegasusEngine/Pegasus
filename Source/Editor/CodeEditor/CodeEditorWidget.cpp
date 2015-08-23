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

static const char * DOCKABLE_DESC = "Dockable: Allow to be docked when hovering over main window.";
static const char * UNDOCKABLE_DESC = "Undockable: Allow to hover over window w/o docking.";
static const char * INSTANT_COMPILATION_DESC_ON = "Turn instant compilation ON\nInstant Compilation Mode: code gets compiled at every key stroke, in an efficient manner. ";
static const char * INSTANT_COMPILATION_DESC_OFF = "Turn instant compilation OFF\nInstant Compilation Mode: code gets compiled at every key stroke, in an efficient manner. ";



static inline QString SrcToQString(const char* src, int srcSize)
{    
    QChar * qchar = new QChar[srcSize];
    for (int i = 0; i < srcSize; ++i)
    {
        qchar[i] = src[i];
    }

    QString srcQString(qchar, srcSize);
    delete[] qchar;            
    return srcQString;

}


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

    mSaveAction = toolBar->addAction(saveIcon, tr("save this source to its file"));

    mCloseViewAction  = toolBar->addAction(singleIcon, tr("close current view"));
    mVerticalAction   = toolBar->addAction(verticalIcon, tr("split views vertically"));
    mHorizontalAction = toolBar->addAction(horizontalIcon, tr("split views horizontally"));
    mCompileAction    = toolBar->addAction(compileIcon, tr("compile current code (Ctrl + F7)"));
    mCompileAction->setShortcut(tr("Ctrl+F7"));

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

    resize(550, 700);
    setWindowTitle(tr("Code Editor"));
    setObjectName("CodeEditor");

    mUi.mTabWidget = new NodeFileTabBar(this);
    
    connect(
        mUi.mTabWidget, SIGNAL(RuntimeObjectRemoved(Pegasus::AssetLib::IRuntimeAssetObjectProxy*, QObject*)),
        this, SLOT(RequestClose(Pegasus::AssetLib::IRuntimeAssetObjectProxy*, QObject*))
    );

    connect(
        mUi.mTabWidget, SIGNAL(DisplayRuntimeObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
        this,   SLOT(SignalViewCode(Pegasus::AssetLib::IRuntimeAssetObjectProxy*))
    );

    connect(
        mUi.mTabWidget, SIGNAL(SaveCurrentRuntimeObject()),
        this,   SLOT(SignalSaveCurrentCode())
    );

    connect(
        mUi.mTabWidget, SIGNAL(SaveCurrentRuntimeObject()),
        this,   SLOT(SignalSaveCurrentCode())
    );

    connect(
        mUi.mTabWidget, SIGNAL(DiscardCurrentObjectChanges()),
        this,   SLOT(SignalDiscardCurrentObjectChanges())
    );

    connect(
        mUi.mTabWidget, SIGNAL(RegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
        this,   SIGNAL(OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*))
    );

    connect(
        mUi.mTabWidget, SIGNAL(UnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
        this,   SIGNAL(OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*))
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

    //setup the status bar
    mUi.mStatusBar = new QStatusBar(mainWidget);
    
    mUi.mMainLayout->addWidget(toolBar);
    mUi.mMainLayout->addWidget(mUi.mTabWidget);
    mUi.mMainLayout->addWidget(mUi.mTreeEditor);    
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
        Pegasus::Core::ISourceCodeProxy* sourceCode = mUi.mTabWidget->GetTabObject<Pegasus::Core::ISourceCodeProxy>(idx);
        CodeUserData * code = static_cast<CodeUserData*>(sourceCode->GetUserData());
        if (code != nullptr)
        {
            //ensure that no compilation is happening while saving this file
            CompileCode(code);
        }
    }
}

void CodeEditorWidget::SignalCompilationError(CodeUserData* userData, int line, QString errorString)
{
    if (userData != nullptr)
    {
        userData->InvalidateLine(line);
        userData->InsertMessage(line, errorString);
        PostStatusBarMessage(errorString);
        CodeTextEditorWidget * editor = mUi.mTreeEditor->FindCodeInEditors(userData);
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

void CodeEditorWidget::BlessUserData(CodeUserData* codeUserData)
{
    ED_ASSERT(codeUserData != nullptr);
    codeUserData->SetDocument(new QTextDocument());
}

void CodeEditorWidget::UnblessUserData(CodeUserData* codeUserData)
{
    ED_ASSERT(codeUserData != nullptr);
    int index = mUi.mTabWidget->FindIndex(codeUserData->GetSourceCode());
    if (index != -1)
    {
        mUi.mTabWidget->Close(index);
    }

    delete codeUserData->GetDocument();
    codeUserData->SetDocument(nullptr);

    //request that user data gets deleted on the app thread.
    emit(RequestSafeDeleteUserData(codeUserData));
}

void CodeEditorWidget::SignalCompilationBegin(CodeUserData* target)
{
    CodeTextEditorWidget * editor = mUi.mTreeEditor->FindCodeInEditors(target);
    if (editor != nullptr)
    {
        PostStatusBarMessage("");
        ED_ASSERT(!target->IsProgram());
        QSet<int> lineSetCopy = target->GetInvalidLineSet();
        target->ClearInvalidLines();
        for (int line : lineSetCopy)
        {
            UpdateSyntaxForLine(editor, line);
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
    PostStatusBarMessage("");
    int idx = mUi.mTabWidget->GetCurrentIndex();
    if (idx >= 0)
    {
        Pegasus::Core::ISourceCodeProxy* sourceCode = mUi.mTabWidget->GetTabObject<Pegasus::Core::ISourceCodeProxy>(idx);

        //dump ui document data to the internal node so we can save
        CodeUserData* code = static_cast<CodeUserData*>(sourceCode->GetUserData());
        QString& plainText = code->GetDocument()->toPlainText();
        QByteArray ba = plainText.toLocal8Bit();
        const char* asciiData = ba.constData();
        sourceCode->SetSource(asciiData, plainText.size());

        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::SAVE_ASSET);
        msg.SetObject(sourceCode);
        SendAssetIoMessage(msg);

    }
}

void CodeEditorWidget::SignalDiscardCurrentObjectChanges()
{
    int idx = mUi.mTabWidget->GetCurrentIndex();
    if (idx >= 0)
    {
        Pegasus::Core::ISourceCodeProxy* sourceCode = mUi.mTabWidget->GetTabObject<Pegasus::Core::ISourceCodeProxy>(idx);
        sourceCode->ReloadFromAsset();
        const char* src = nullptr;
        int srcSize = 0;
        sourceCode->GetSource(&src, srcSize);
        QString qtxt = SrcToQString(src, srcSize);
        CodeUserData* code = static_cast<CodeUserData*>(sourceCode->GetUserData());
        if (code != nullptr)
        {
            mInternalBlockTextUpdated = true;
            code->GetDocument()->setPlainText(qtxt);
            mInternalBlockTextUpdated = false;
            CompileCode(code);
        }
    }
}

void CodeEditorWidget::SignalSavedFileSuccess()
{
    PostStatusBarMessage(tr("File Saved."));
}

void CodeEditorWidget::SignalViewCode(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    if (object != nullptr)
    {
        CodeTextEditorWidget * editor = nullptr;
        Pegasus::Core::ISourceCodeProxy* code = static_cast<Pegasus::Core::ISourceCodeProxy*>(object);
        CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
        
        editor = mUi.mTreeEditor->FindCodeInEditors(userData);

        mInternalBlockTextUpdated = true;
        mUi.mTreeEditor->DisplayCode(userData, editor);
        mInternalBlockTextUpdated = false;

        UpdateInstantCompilationButton(userData);
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


void CodeEditorWidget::RequestClose(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object, QObject* extraData)
{
    Pegasus::Core::ISourceCodeProxy* code = static_cast<Pegasus::Core::ISourceCodeProxy*>(object);
    CodeUserData* codeToClose = static_cast<CodeUserData*>(code->GetUserData()); 
    mInternalBlockTextUpdated = true;
    mUi.mTreeEditor->HideCode(codeToClose);
    mInternalBlockTextUpdated = false;

    //send a message to the render thread to close this code safely
    AssetIOMessageController::Message msg;
    msg.SetMessageType(AssetIOMessageController::Message::CLOSE_ASSET);
    msg.SetObject(codeToClose->GetSourceCode());
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
            CompileCode(textEditor->GetCode());
        }
    }
}

void CodeEditorWidget::OnTextWindowSelected(QWidget * sender)
{
    CodeTextEditorWidget * textEditor = static_cast<CodeTextEditorWidget*>(sender);
    if (textEditor != nullptr)
    {
        CodeUserData * code = textEditor->GetCode();

        if (code != nullptr)
        {
            int i = mUi.mTabWidget->FindIndex(code->GetSourceCode());
            if (i >= 0 && i < mUi.mTabWidget->GetTabCount() && i != mUi.mTabWidget->GetCurrentIndex())
            {
                mUi.mTabWidget->SetCurrentIndex(i);
            }
        }
    }
}

void CodeEditorWidget::CompileCode(CodeUserData* code)
{
    mCompilationRequestMutex->lock(); 
    if (!mCompilationRequestPending)
    {
        mCompilationRequestPending = true;            
        mCompilationRequestMutex->unlock();
        emit(RequestCompilationBegin());
        SourceIOMessageController::Message msg;
        msg.SetMessageType(SourceIOMessageController::Message::SET_SOURCE_AND_COMPILE_SOURCE);
        msg.SetSourceText(code->GetDocument()->toPlainText());
        msg.SetSource(code->GetSourceCode());
        emit (SendSourceIoMessage(msg));
    }
    else
    {
        mCompilationRequestMutex->unlock();
    }
}

void CodeEditorWidget::UpdateInstantCompilationButton(CodeUserData* code)
{
    Pegasus::Core::ISourceCodeProxy* proxy = code->GetSourceCode();
    if (proxy != nullptr && mCompilationPolicy != proxy->GetCompilationPolicy())
    {
        mCompilationPolicy = proxy->GetCompilationPolicy();
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

void CodeEditorWidget::OnOpenObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    show();
    activateWindow();
    Pegasus::Core::ISourceCodeProxy* codeProxy = static_cast<Pegasus::Core::ISourceCodeProxy*>(object);
    //TODO: bigass assert here
    CodeUserData* codeUserData = static_cast<CodeUserData*>(codeProxy->GetUserData());
    if (codeProxy == nullptr || codeUserData->GetDocument() == nullptr)
    {
        return;
    }
    //dump code contents to document text
    //set the text of the current text editor
    const char * srcChar = nullptr;
    int srcSize = 0;
    codeUserData->GetSourceCode()->GetSource(&srcChar, srcSize);
    QString qtxt = SrcToQString(srcChar, srcSize);
    mInternalBlockTextUpdated = true;
    codeUserData->GetDocument()->setPlainText(qtxt);
    mInternalBlockTextUpdated = false;

    mUi.mTabWidget->Open(codeUserData->GetSourceCode());
    CompileCode(codeUserData);
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
