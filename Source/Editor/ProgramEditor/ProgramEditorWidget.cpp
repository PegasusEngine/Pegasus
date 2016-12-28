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
#include "Pegasus/Shader/Shared/ShaderDefs.h"
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

const char* gShaderStageNames[Pegasus::Shader::SHADER_STAGES_COUNT] = {
    "vertex",
    "fragment",
    "tesselation control",
    "tesselation evaluation",
    "geometry",
    "compute"
};

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

    mAddShaderMapper = new QSignalMapper(mMainWidget);

    mRemoveShaderMapper = new QSignalMapper(mMainWidget);

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

    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        ProgramEditorWidget::ShaderSlots& uiSlot = mShaderSlots[i];

        QHBoxLayout* vertLayout = new QHBoxLayout();
        mainLayout->addItem(vertLayout);

        uiSlot.mSlotName = new QLabel(tr(gShaderStageNames[i]), mMainWidget);
        vertLayout->addWidget(uiSlot.mSlotName);

        uiSlot.mLoadButton = new QToolButton(mMainWidget);
        uiSlot.mLoadButton->setIcon(addIcon);
        uiSlot.mLoadButton->setToolTip(tr("Browse shader asset"));
        vertLayout->addWidget(uiSlot.mLoadButton);

        uiSlot.mDeleteButton = new QToolButton(mMainWidget);
        uiSlot.mDeleteButton->setIcon(delIcon);
        uiSlot.mDeleteButton->setToolTip(tr("Remove current shader"));
        vertLayout->addWidget(uiSlot.mDeleteButton);

        uiSlot.mActive = false;

        uiSlot.mAsset = new QLabel(tr("<none>"), mMainWidget);
        vertLayout->addWidget(uiSlot.mAsset);

        //connect signals
        mAddShaderMapper->setMapping(uiSlot.mLoadButton, i);
        connect(uiSlot.mLoadButton, SIGNAL(clicked(bool)),
                mAddShaderMapper, SLOT(map()));

        mRemoveShaderMapper->setMapping(uiSlot.mDeleteButton, i);
        connect(uiSlot.mDeleteButton, SIGNAL(clicked(bool)),
                mRemoveShaderMapper, SLOT(map()));
    }

    mStatusBar = new QStatusBar(mMainWidget);
    mainLayout->addWidget(mStatusBar);

    connect(mAddShaderMapper, SIGNAL(mapped(int)),
            this, SLOT(OnAddShader(int)));

    connect(mRemoveShaderMapper, SIGNAL(mapped(int)),
            this, SLOT(OnRemoveShader(int)));
    EnableUi(false);
}

QString ProgramEditorWidget::GetCurrentShaderPath(Pegasus::Shader::ShaderType type)
{
    if (mCurrentProgram.IsValid())
    {
        ProgramData& pd = mProgramData[mCurrentProgram];
        ShaderMap::iterator it = pd.shaders.find(type);
        if (it != pd.shaders.end())
        {
            return it.value();
        }
    }
    return tr("");
}

void ProgramEditorWidget::OnAddShader(int id)
{
    Pegasus::App::IApplicationProxy* app = GetEditor()->GetApplicationManager().GetApplication()->GetApplicationProxy();
    if (app != nullptr)
    {
        QString rootFolder = app->GetAssetsRoot();
        QDir qd(rootFolder);
        QString fileExtension = tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[id], Pegasus::Shader::gShaderExtensions[id]);
        QString shaderAsset = QFileDialog::getOpenFileName(this, tr("Open shader."), rootFolder, fileExtension);
        QString filePath = qd.path();
        if (shaderAsset.startsWith(filePath))
        {
            QString path = shaderAsset.right(shaderAsset.size() - filePath.size() - 1);
            Pegasus::Shader::ShaderType shaderType = static_cast<Pegasus::Shader::ShaderType>(id);
            QString previousPath = GetCurrentShaderPath(shaderType);
            ProgramEditorModifyShaderCmd* cmd = new ProgramEditorModifyShaderCmd(
                this,
                path,
                previousPath,
                shaderType
            );
            GetCurrentUndoStack()->push(cmd);
        }
        else if (shaderAsset.length() > 0)
        { 
            QMessageBox::warning(this, tr("Can't load asset."), tr("Asset must be loaded into the root directory only of the project. ")+rootFolder, QMessageBox::Ok);
        }
    }
}

void ProgramEditorWidget::SetShader(const QString& shaderFile, Pegasus::Shader::ShaderType shaderType)
{
    if (mCurrentProgram.IsValid())
    {
        ProgramData& pd = mProgramData[mCurrentProgram];
        mTabBar->MarkCurrentAsDirty();
        if (shaderFile == "")
        {
            
            pd.shaders.remove(shaderType);
            mTabBar->MarkCurrentAsDirty();
            ProgramIOMCMessage msg;
            msg.SetMessageType(ProgramIOMCMessage::REMOVE_SHADER);
            msg.SetShaderType(shaderType);
            msg.SetProgram(mCurrentProgram);
            emit SendProgramIoMessage(msg);
        }
        else
        {
            pd.shaders[shaderType] = shaderFile;
            ProgramIOMCMessage msg;
            msg.SetMessageType(ProgramIOMCMessage::MODIFY_SHADER);
            msg.SetShaderPath(shaderFile);
            msg.SetProgram(mCurrentProgram);
            emit SendProgramIoMessage(msg);
        }
    }
}

void ProgramEditorWidget::OnRemoveShader(int id)
{
    if (mCurrentProgram.IsValid())
    {
        Pegasus::Shader::ShaderType shaderType = static_cast<Pegasus::Shader::ShaderType>(id);
        GetCurrentUndoStack()->push( new ProgramEditorModifyShaderCmd (
                this,
                tr(""),
                GetCurrentShaderPath(shaderType),
                shaderType
            )
        );
    }
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
    QVariantList shaders = map["Shaders"].toList();

    ProgramData pd;

    for (QVariant v : shaders)
    {
        QVariantMap s = v.toMap();
        QString shaderPath = s["Path"].toString();
        Pegasus::Shader::ShaderType shaderType = static_cast<Pegasus::Shader::ShaderType>(s["Type"].toInt());
        pd.shaders.insert(shaderType, shaderPath);
    }

    mProgramData.insert(handle, pd); 

    QUndoStack* programUndoStack = new QUndoStack(this);
    mTabBar->Open(handle, displayName, programUndoStack);
}

void ProgramEditorWidget::SyncUiToProgram()
{
    ClearUi();
    if (mCurrentProgram.IsValid())
    {
        ProgramData pd = mProgramData[mCurrentProgram];

        ShaderMap::iterator it = pd.shaders.begin();
        for (; it != pd.shaders.end(); ++it)
        {
            Pegasus::Shader::ShaderType shaderType = it.key();
            if (shaderType >= Pegasus::Shader::SHADER_STAGES_BEGIN && shaderType < Pegasus::Shader::SHADER_STAGES_COUNT)
            {
                ProgramEditorWidget::ShaderSlots& slot = mShaderSlots[shaderType];
                slot.mAsset->setText(it.value());
                slot.mActive = true;
            }
        }
    }
}

void ProgramEditorWidget::EnableUi(bool enableUi)
{
    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        mShaderSlots[i].mLoadButton->setEnabled(enableUi);
        mShaderSlots[i].mDeleteButton->setEnabled(enableUi);
    }
}

void ProgramEditorWidget::ClearUi()
{
    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        ProgramEditorWidget::ShaderSlots& uiSlot = mShaderSlots[i];
        uiSlot.mFullAssetPath = "";
        uiSlot.mActive = false;
        uiSlot.mAsset->setText(tr("<none>"));
    }
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
