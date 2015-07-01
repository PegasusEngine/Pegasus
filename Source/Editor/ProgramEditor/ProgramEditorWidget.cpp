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
#include "ProgramEditor/ProgramEditorWidget.h"
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
#include "AssetLibrary/AssetLibraryWidget.h"

const char* gShaderStageNames[Pegasus::Shader::SHADER_STAGES_COUNT] = {
    "vertex",
    "fragment",
    "tesselation control",
    "tesselation evaluation",
    "geometry",
    "compute"
};

ProgramEditorWidget::ProgramEditorWidget(QWidget* parent)
    : mCurrentProgram(nullptr)
{
    SetupUi();
}

ProgramEditorWidget::~ProgramEditorWidget()
{
}

void ProgramEditorWidget::SetupUi()
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("Program Editor"));
    setObjectName("ProgramEditor");
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);

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

    mTabBar = new QTabBar(this);
    mTabBar->setTabsClosable(true);

    connect(
        mTabBar, SIGNAL(tabCloseRequested(int)),
        this, SLOT(RequestCloseProgram(int))
    );

    connect(
        mTabBar, SIGNAL(currentChanged(int)),
        this,   SLOT(OnViewProgram(int))
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
            ProgramIOMessageController::Message msg;
            msg.SetMessageType(ProgramIOMessageController::Message::MODIFY_SHADER);
            msg.SetShaderPath(shaderAsset.right(shaderAsset.size() - filePath.size() - 1));
            msg.SetProgram(mCurrentProgram);
            emit SendProgramIoMessage(msg);
        }
        else if (shaderAsset.length() > 0)
        { 
            QMessageBox::warning(this, tr("Can't load asset."), tr("Asset must be loaded into the root directory only of the project. ")+rootFolder, QMessageBox::Ok);
        }
    }
}

void ProgramEditorWidget::OnRemoveShader(int id)
{
    ProgramIOMessageController::Message msg;
    msg.SetMessageType(ProgramIOMessageController::Message::REMOVE_SHADER);
    msg.SetShaderType(static_cast<Pegasus::Shader::ShaderType>(id));
    msg.SetProgram(mCurrentProgram);
    emit SendProgramIoMessage(msg);
}

void ProgramEditorWidget::PostStatusBarMessage(const QString& msg)
{
    if (msg == "" || mStatusBarMessage == "")
    {
        mStatusBarMessage = msg;
        mStatusBar->showMessage(msg);
    }
}

void ProgramEditorWidget::SignalSaveCurrentProgram()
{
    PostStatusBarMessage(tr("")); //clear the message bar
    AssetIOMessageController::Message msg;
    msg.SetMessageType(AssetIOMessageController::Message::SAVE_PROGRAM);
    msg.GetAssetNode().mProgram = mCurrentProgram;
    emit SendAssetIoMessage(msg);
}

void ProgramEditorWidget::RequestOpenProgram(Pegasus::Shader::IProgramProxy* program)
{
    show();
    activateWindow();
    //find if the program is open.
    for (int i = 0; i < mTabBar->count(); ++i)
    {
        if (program == mPrograms[i])
        {
            mTabBar->setCurrentIndex(i);
            return;
        }
    }
    
    if (mTabBar->count() < MAX_PROGRAMS_OPEN - 1)
    {
        mPrograms[mTabBar->count()] = program;
        int targetIndex = mTabBar->count();
        mTabBar->addTab((program->GetName()));
        mTabBar->setCurrentIndex(targetIndex);
    }
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

void ProgramEditorWidget::OnViewProgram(int programid)
{
    if (programid == -1)
    {
        ClearUi();
        mCurrentProgram = nullptr;
    }
    else if (programid < MAX_PROGRAMS_OPEN)
    {
        mCurrentProgram = mPrograms[programid];
        SyncUiToProgram();
        EnableUi(true);
    }
}


void ProgramEditorWidget::RequestCloseProgram(int id)
{
    ED_ASSERT(id >= 0 && id < mTabBar->count());
    
    AssetIOMessageController::Message msg;
    msg.SetMessageType(AssetIOMessageController::Message::CLOSE_PROGRAM);
    msg.GetAssetNode().mProgram = mPrograms[id];

    if (id < MAX_PROGRAMS_OPEN - 1)
    {
        for (int c = id; c < mTabBar->count() - 1; ++c)
        {
            mPrograms[c] = mPrograms[c + 1];
        }

        int currIdx = mTabBar->currentIndex();
        mTabBar->removeTab(id);        
    } 

    if (mTabBar->count() == 0)
    {
        EnableUi(false);
    }

    emit (SendAssetIoMessage(msg));
}

void ProgramEditorWidget::UpdateUIForAppFinished()
{
    while (mTabBar->count() > 0)
    {
        RequestCloseProgram(0);
    }
}
