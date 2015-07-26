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
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
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
    : PegasusDockWidget(parent, editor), mCurrentProgram(nullptr)
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
        mTabBar, SIGNAL(RuntimeObjectRemoved(Pegasus::AssetLib::IRuntimeAssetObjectProxy*, QObject*)),
        this, SLOT(RequestCloseProgram(Pegasus::AssetLib::IRuntimeAssetObjectProxy*, QObject*))
    );

    connect(
        mTabBar, SIGNAL(DisplayRuntimeObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
        this,   SLOT(OnViewProgram(Pegasus::AssetLib::IRuntimeAssetObjectProxy*))
    );

    connect(
        mTabBar, SIGNAL(SaveCurrentRuntimeObject()),
        this,   SLOT(SignalSaveCurrentProgram())
    );

    connect(
        mTabBar, SIGNAL(DiscardCurrentObjectChanges()),
        this,   SLOT(SignalDiscardCurrentObjectChanges())
    );

    connect(
        mTabBar, SIGNAL(RegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
        this,   SIGNAL(OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*))
    );

    connect(
        mTabBar, SIGNAL(UnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
        this,   SIGNAL(OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*))
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
    if (mCurrentProgram != nullptr)
    {
        int shaderCounts = mCurrentProgram->GetShaderCount();
        for (int i = 0; i < shaderCounts; ++i)
        {
            Pegasus::Shader::IShaderProxy* shaderStage = mCurrentProgram->GetShader((unsigned)i);
            Pegasus::Shader::ShaderType shaderType = shaderStage->GetStageType();
            if (shaderType == type)
            {
                Pegasus::AssetLib::IAssetProxy* ownerAsset = shaderStage->GetOwnerAsset();
                if (ownerAsset != nullptr)
                {
                    return tr(ownerAsset->GetPath());
                }
            }
        }
    }
    return tr("");
}

void ProgramEditorWidget::OnAddShader(int id)
{
    Pegasus::App::IApplicationProxy* app = Editor::GetInstance().GetApplicationManager().GetApplication()->GetApplicationProxy();
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
    if (mCurrentProgram != nullptr)
    {
        mTabBar->MarkCurrentAsDirty();
        if (shaderFile == "")
        {
            mTabBar->MarkCurrentAsDirty();
            ProgramIOMessageController::Message msg;
            msg.SetMessageType(ProgramIOMessageController::Message::REMOVE_SHADER);
            msg.SetShaderType(shaderType);
            msg.SetProgram(mCurrentProgram);
            emit SendProgramIoMessage(msg);
        }
        else
        {
            ProgramIOMessageController::Message msg;
            msg.SetMessageType(ProgramIOMessageController::Message::MODIFY_SHADER);
            msg.SetShaderPath(shaderFile);
            msg.SetProgram(mCurrentProgram);
            emit SendProgramIoMessage(msg);
        }
    }
}

void ProgramEditorWidget::OnRemoveShader(int id)
{
    if (mCurrentProgram != nullptr)
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

void ProgramEditorWidget::OnReceiveAssetIoMessage(AssetIOMessageController::Message::IoResponseMessage id)
{
    switch(id)
    {
    case AssetIOMessageController::Message::IO_SAVE_SUCCESS:
        // This might create a race condition, if the user changes a tab wile the file is saving... to fix this we need
        // to pass the object around... if this becomes a problem then we could fix it later...
        mTabBar->ClearCurrentDirty();
        PostStatusBarMessage(tr("Saved file successfully."));
        break;
    case AssetIOMessageController::Message::IO_SAVE_ERROR:
        PostStatusBarMessage(tr("IO Error saving file."));
        break;
    case AssetIOMessageController::Message::IO_NEW_SUCCESS:
        PostStatusBarMessage(tr(""));
        break;
    case AssetIOMessageController::Message::IO_NEW_ERROR:
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
    if (mCurrentProgram != nullptr)
    {
        PostStatusBarMessage(tr("")); //clear the message bar
        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::SAVE_PROGRAM);
        msg.GetAssetNode().mProgram = mCurrentProgram;
        SendAssetIoMessage(msg);
    }
}

void ProgramEditorWidget::SignalDiscardCurrentObjectChanges()
{
    mCurrentProgram->ReloadFromAsset();
}

void ProgramEditorWidget::RequestOpenProgram(Pegasus::Shader::IProgramProxy* program)
{
    show();
    activateWindow();
    QUndoStack* programUndoStack = new QUndoStack(this);
    mTabBar->Open(program, programUndoStack);
}

void ProgramEditorWidget::SyncUiToProgram()
{
    ClearUi();
    if (mCurrentProgram != nullptr)
    {
        int shaderCounts = mCurrentProgram->GetShaderCount();
        for (int i = 0; i < shaderCounts; ++i)
        {
            Pegasus::Shader::IShaderProxy* shaderStage = mCurrentProgram->GetShader((unsigned)i);
            Pegasus::Shader::ShaderType shaderType = shaderStage->GetStageType();
            if (shaderType >= Pegasus::Shader::SHADER_STAGES_BEGIN && shaderType < Pegasus::Shader::SHADER_STAGES_COUNT)
            {
                ProgramEditorWidget::ShaderSlots& slot = mShaderSlots[shaderType];
                slot.mAsset->setText(tr(shaderStage->GetName()));
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

void ProgramEditorWidget::OnViewProgram(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    if (object != nullptr)
    {
        mCurrentProgram = static_cast<Pegasus::Shader::IProgramProxy*>(object);
        SyncUiToProgram();
        EnableUi(true);
    }
    else
    {
        mCurrentProgram = nullptr;
        EnableUi(false);
        ClearUi();
    }
}


void ProgramEditorWidget::RequestCloseProgram(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object, QObject* extraData)
{
    ED_ASSERT(extraData != nullptr);
    delete extraData;

    Pegasus::Shader::IProgramProxy* program = static_cast<Pegasus::Shader::IProgramProxy*>(object);
    AssetIOMessageController::Message msg;
    msg.SetMessageType(AssetIOMessageController::Message::CLOSE_PROGRAM);
    msg.GetAssetNode().mProgram = program;


    if (mTabBar->GetTabCount() == 0)
    {
        EnableUi(false);
    }

    SendAssetIoMessage(msg);
}

QUndoStack* ProgramEditorWidget::GetCurrentUndoStack() const
{
    if (mCurrentProgram != nullptr && mTabBar->GetCurrentIndex() >= 0)
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
