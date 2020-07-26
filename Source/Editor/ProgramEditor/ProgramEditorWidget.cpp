/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramEditorWidget.cpp
//! \author	Kleber Garcia
//! \date	10rth March 2015
//! \brief	Program editor IDE

#include "Editor.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "ProgramEditor/ProgramEditorWidget.h"
#include "ProgramEditor/ProgramEditorUndoCommands.h"
#include "Widgets/NodeFileTabBar.h"
#include "Application/ApplicationManager.h"
#include "Application/Application.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QToolButton>
#include <QTabBar>
#include <QToolBar>
#include <QDir>
#include <QStatusBar>
#include <QUndoStack>
#include <QSignalMapper>
#include "AssetLibrary/AssetLibraryWidget.h"

ProgramEditorWidget::ProgramEditorWidget(QWidget* parent, Editor* editor)
    : PegasusDockWidget(parent, editor)
{
}

ProgramEditorWidget::~ProgramEditorWidget()
{
}

void ProgramEditorWidget::SetupUi()
{
    mMainWidget = new QWidget(this);
    QVBoxLayout * mainLayout = new QVBoxLayout();
    mMainWidget->setLayout(mainLayout);
    setWidget(mMainWidget);

    QIcon saveIcon(tr(":/CodeEditor/save.png"));
    QToolBar* toolBar = new QToolBar(mMainWidget);
    mainLayout->addWidget(toolBar);
    toolBar->setIconSize(QSize(16,16));
    
    mSaveAction = toolBar->addAction(saveIcon, tr("save this program to its asset file"));
    connect(mSaveAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalSaveCurrentProgram()));

    QIcon addIcon(tr(":/TimelineToolbar/Add16.png"));
    QIcon delIcon(tr(":/TimelineToolbar/Remove16.png"));

    mTabBar = new NodeFileTabBar(this);

    connect(
        mTabBar, SIGNAL(RuntimeObjectRemoved(AssetInstanceHandle, QObject*)),
        this, SLOT(RequestCloseProgram(AssetInstanceHandle, QObject*))
    );

    connect(
        mTabBar, SIGNAL(DisplayRuntimeObject(AssetInstanceHandle)),
        this,   SLOT(OnViewProgram(AssetInstanceHandle))
    );

    connect(
        mTabBar, SIGNAL(SaveRuntimeObject(int)),
        this,   SLOT(SignalSaveTab(int))
    );

    connect(
        mTabBar, SIGNAL(DiscardObjectChanges(AssetInstanceHandle)),
        this,   SLOT(SignalDiscardObjectChanges(AssetInstanceHandle))
    );

    mainLayout->addWidget(mTabBar);

    QHBoxLayout* vertLayout = new QHBoxLayout();
    mainLayout->addItem(vertLayout);
    vertLayout->addWidget(new QLabel(tr("Source:"), mMainWidget));

    mShaderName = new QLabel(tr("<none>"), mMainWidget);
    vertLayout->addWidget(mShaderName);
    
    mLoadShaderButton = new QToolButton(mMainWidget);
    mLoadShaderButton->setIcon(addIcon);
    mLoadShaderButton->setToolTip(tr("Browse shader source asset"));
    vertLayout->addWidget(mLoadShaderButton);

    mRemoveShaderButton = new QToolButton(mMainWidget);
    mRemoveShaderButton->setIcon(delIcon);
    mRemoveShaderButton->setToolTip(tr("Remove current shader asset"));
    vertLayout->addWidget(mRemoveShaderButton);

    mStatusBar = new QStatusBar(mMainWidget);
    mainLayout->addWidget(mStatusBar);

    connect(mLoadShaderButton, SIGNAL(clicked(bool)),
            this, SLOT(OnLoadShader()));

    connect(mRemoveShaderButton, SIGNAL(clicked(bool)),
            this, SLOT(OnRemoveShader()));
    EnableUi(false);
}

void ProgramEditorWidget::OnLoadShader()
{
    Pegasus::App::IApplicationProxy* app = GetEditor()->GetApplicationManager().GetApplication()->GetApplicationProxy();
    if (app == nullptr)
        return;

    QString rootFolder = app->GetAssetsRoot();
    QDir qd(rootFolder);
    QString fileExtension = tr("%1 Shader (*.%2)").arg(tr("Shader File"), tr("hlsl"));
    QString shaderAsset = QFileDialog::getOpenFileName(this, tr("Open shader."), rootFolder, fileExtension);
    QString filePath = qd.path();
    if (shaderAsset.startsWith(filePath))
    {
        QString path = shaderAsset.right(shaderAsset.size() - filePath.size() - 1);
        QString previousPath = GetCurrentShaderPath();
        ProgramEditorModifyShaderCmd* cmd = new ProgramEditorModifyShaderCmd(
            this,
            path,
            previousPath
        );
        GetCurrentUndoStack()->push(cmd);
    }
    else if (shaderAsset.length() > 0)
    { 
        QMessageBox::warning(this, tr("Can't load asset."), tr("Asset must be loaded into the root directory only of the project. ")+rootFolder, QMessageBox::Ok);
    }
    
}

void ProgramEditorWidget::OnRemoveShader()
{
    GetCurrentUndoStack()->push( new ProgramEditorModifyShaderCmd (
            this,
            tr(""),
            GetCurrentShaderPath()
        )
    );
}

void ProgramEditorWidget::PostStatusBarMessage(const QString& msg)
{
    if (msg == "" || mStatusBarMessage == "")
    {
        mStatusBarMessage = msg;
        mStatusBar->showMessage(msg);
    }
}

void ProgramEditorWidget::OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage id)
{
    switch(id)
    {
    case AssetIOMCMessage::IO_SAVE_SUCCESS:
        // This might create a race condition, if the user changes a tab wile the file is saving... to fix this we need
        // to pass the object around... if this becomes a problem then we could fix it later...
        mTabBar->ClearCurrentDirty();
        PostStatusBarMessage(tr("Saved file successfully."));
        break;
    case AssetIOMCMessage::IO_SAVE_ERROR:
        PostStatusBarMessage(tr("IO Error saving file."));
        break;
    case AssetIOMCMessage::IO_NEW_SUCCESS:
        PostStatusBarMessage(tr(""));
        break;
    case AssetIOMCMessage::IO_NEW_ERROR:
        PostStatusBarMessage(tr("IO Error creating new program file."));
        break;
    default:
        ED_FAILSTR("Unkown IO Asset message id %d", id);
    }
}

void ProgramEditorWidget::OnSaveFocusedObject()
{
    SignalSaveCurrentProgram();
}
void ProgramEditorWidget::SignalSaveCurrentProgram()
{
    int idx = mTabBar->GetCurrentIndex();
    SignalSaveTab(idx);
}

void ProgramEditorWidget::SignalSaveTab(int tabId)
{
    if (tabId < 0 || tabId >= mTabBar->GetTabCount()) return;

    AssetInstanceHandle handle = mTabBar->GetTabObject(tabId);
    if (handle.IsValid())
    {
        PostStatusBarMessage(tr("")); //clear the message bar
        AssetIOMCMessage msg;
        msg.SetMessageType(AssetIOMCMessage::SAVE_ASSET);
        msg.SetObject(handle);
        SendAssetIoMessage(msg);
    }
}

void ProgramEditorWidget::SignalDiscardObjectChanges(AssetInstanceHandle object)
{
    if (object.IsValid())
    {
        AssetIOMCMessage msg(AssetIOMCMessage::RELOAD_FROM_ASSET);
        msg.SetObject(object);
        SendAssetIoMessage(msg);
    }
}
    

void ProgramEditorWidget::OnOpenObject(AssetInstanceHandle handle, const QString& displayName, const QVariant& initData)
{
    show();
    activateWindow();
    QVariantMap map = initData.toMap();
    QString name = map["Name"].toString();

    ProgramData pd;

    auto shaderIt = map.find("Shader");
    if (shaderIt != map.end())
    {
        QVariantMap shader = shaderIt.value().toMap();
        pd.sourcePath = shader["Path"].toString();
    }

    mProgramData.insert(handle, pd); 

    QUndoStack* programUndoStack = new QUndoStack(this);
    mTabBar->Open(handle, displayName, programUndoStack);
}

void ProgramEditorWidget::SyncUiToProgram()
{
    ClearUi();
    if (!mCurrentProgram.IsValid())
        return;

    ProgramData pd = mProgramData[mCurrentProgram];
    mShaderName->setText(pd.sourcePath);
}

void ProgramEditorWidget::EnableUi(bool enableUi)
{
    mLoadShaderButton->setEnabled(enableUi);
    mRemoveShaderButton->setEnabled(enableUi);
}

void ProgramEditorWidget::ClearUi()
{
    mShaderName->setText(tr("<none>"));
}

void ProgramEditorWidget::OnViewProgram(AssetInstanceHandle handle)
{
    if (handle.IsValid())
    {
        mCurrentProgram = handle;
        SyncUiToProgram();
        EnableUi(true);
    }
    else
    {
        mCurrentProgram = AssetInstanceHandle();
        EnableUi(false);
        ClearUi();
    }
}


void ProgramEditorWidget::RequestCloseProgram(AssetInstanceHandle handle, QObject* extraData)
{
    ED_ASSERT(extraData != nullptr);
    delete extraData;

    AssetIOMCMessage msg;
    msg.SetMessageType(AssetIOMCMessage::CLOSE_ASSET);
    msg.SetObject(handle);


    if (mTabBar->GetTabCount() == 0)
    {
        EnableUi(false);
    }

    SendAssetIoMessage(msg);
}

QUndoStack* ProgramEditorWidget::GetCurrentUndoStack() const
{
    if (mCurrentProgram.IsValid() && mTabBar->GetCurrentIndex() >= 0)
    {
        int index = mTabBar->GetCurrentIndex();
        return static_cast<QUndoStack*>(mTabBar->GetExtraData(index));
    }
    return nullptr;
}

QString ProgramEditorWidget::GetCurrentShaderPath()
{
    if (!mCurrentProgram.IsValid())
        return tr("");

    ProgramData& pd = mProgramData[mCurrentProgram];
    return pd.sourcePath;
}


void ProgramEditorWidget::SetShader(const QString& shaderPath)
{
    if (!mCurrentProgram.IsValid())
        return;

    ProgramData& pd = mProgramData[mCurrentProgram];
    mTabBar->MarkCurrentAsDirty();
    pd.sourcePath = shaderPath;

    ProgramIOMCMessage msg;
    msg.SetMessageType(shaderPath == "" ? ProgramIOMCMessage::REMOVE_SHADER : ProgramIOMCMessage::MODIFY_SHADER);
    msg.SetProgram(mCurrentProgram);
    msg.SetShaderPath(shaderPath);
    emit SendProgramIoMessage(msg);
}

void ProgramEditorWidget::OnUIForAppClosed()
{
    while (mTabBar->GetTabCount() > 0)
    {
        mTabBar->Close(0);
    }
}

const Pegasus::PegasusAssetTypeDesc*const* ProgramEditorWidget::GetTargetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gTypes[] = {
             &Pegasus::ASSET_TYPE_PROGRAM  
            ,nullptr
    };
    
    return gTypes;
}
