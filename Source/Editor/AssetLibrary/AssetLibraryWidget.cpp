/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetLibraryWidget.cpp
//! \author	Kleber Garcia
//! \date	16 Match 2014
//! \brief	Graphics widget representing the shader library form.

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Application/ApplicationManager.h"
#include "AssetLibrary/AssetLibraryWidget.h"
#include "AssetLibrary/ProgramTreeModel.h"
#include "AssetLibrary/SourceCodeListModel.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/Core/Shared/IoErrors.h"
#include <QMenu>
#include <QGroupBox>
#include <QItemSelectionModel>
#include <QTextDocument>
#include <QFileSystemModel>
#include <QFileDialog>
#include <qfilesystemwatcher.h>
#include <qmessagebox.h>

#include "Widgets/PegasusDockWidget.h"



AssetLibraryWidget::AssetLibraryWidget(
        QWidget * parent,
        Editor* editor
)
: PegasusDockWidget(parent, editor)
{
}

void AssetLibraryWidget::SetupUi()
{
    mProgramTreeModel = new ProgramTreeModel(this);
    mProgramSelectionModel = new QItemSelectionModel(mProgramTreeModel);

    mShaderListModel = new SourceCodeListModel(this);
    mShaderListSelectionModel = new QItemSelectionModel(mShaderListModel);

    mBlockScriptListModel = new SourceCodeListModel(this);
    mBlockScriptListSelectionModel = new QItemSelectionModel(mBlockScriptListModel);

    mAssetTreeFileSystemModel = new QFileSystemModel(this);
    mAssetTreeSelectionModel = new QItemSelectionModel(mAssetTreeFileSystemModel);

    //TODO: use this file watcher to resolve external changes in real time to files
    mFileSystemWatcher = new QFileSystemWatcher();

    ui.setupUi(this);

    connect(ui.AssetTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchAsset(QModelIndex)));

    connect(ui.ShaderTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchTextEditorThroughShaderView(QModelIndex)));

    connect(ui.ProgramTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchTextEditorThroughProgramView(QModelIndex)));

    connect(ui.BlockScriptTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchTextEditorThroughBlockScriptView(QModelIndex)));



    connect(mFileSystemWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(OnFileChanged(QString)));

    ui.AssetTreeView->setModel(mAssetTreeFileSystemModel);
    ui.AssetTreeView->setSelectionModel(mAssetTreeSelectionModel);

    ui.ProgramTreeView->setModel(mProgramTreeModel);
    ui.ProgramTreeView->setSelectionModel(mProgramSelectionModel);

    ui.ShaderTreeView->setModel(mShaderListModel);
    ui.ShaderTreeView->setSelectionModel(mShaderListSelectionModel);

    ui.BlockScriptTreeView->setModel(mBlockScriptListModel);
    ui.BlockScriptTreeView->setSelectionModel(mBlockScriptListSelectionModel);

    ui.AddAssetButton->setMenu(CreateNewAssetMenu(tr(""), this));
    ui.AddAssetButton->setPopupMode(QToolButton::InstantPopup);
    
}

//----------------------------------------------------------------------------------------

QMenu* AssetLibraryWidget::CreateNewAssetMenu(const QString& name, QWidget* parent)
{
    QMenu* assetLibMenuBar = new QMenu(name, this);    
    QAction* a = nullptr;
    assetLibMenuBar->addAction(a = new QAction("New &Mesh", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewMesh(bool)));

    assetLibMenuBar->addAction(a = new QAction("New &Program", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewProgram(bool)));

    QMenu* shaderCreationMenu = new QMenu("New &Shader", assetLibMenuBar);
    shaderCreationMenu->addAction(a = new QAction("&Vertex Shader", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewVS(bool)));
    shaderCreationMenu->addAction(a = new QAction("&Pixel Shader", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewPS(bool)));
    shaderCreationMenu->addAction(a = new QAction("&Tesselation Control Shader", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewTCS(bool)));
    shaderCreationMenu->addAction(a = new QAction("Tesselation &Evaluation Shader", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewTES(bool)));
    shaderCreationMenu->addAction(a = new QAction("&Geometry Shader", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewGS(bool)));
    shaderCreationMenu->addAction(a = new QAction("&Compute Shader", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewCS(bool)));
    assetLibMenuBar->addMenu(shaderCreationMenu);

    assetLibMenuBar->addAction(a = new QAction("New &Texture", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewTexture(bool)));

    assetLibMenuBar->addAction(a = new QAction("New Timeline Sc&ript", this));
    connect(a, SIGNAL(triggered(bool)),
            this, SLOT(OnNewTimelineScript(bool)));

    return assetLibMenuBar;

}

//----------------------------------------------------------------------------------------
QString AssetLibraryWidget::AskFilePath(const QString& filter)
{
    QString rootPath = mAssetTreeFileSystemModel->rootPath();
    QString selectedFile = QFileDialog::getSaveFileName(this, tr("Save Asset."), rootPath, filter);
    
    if (selectedFile.size() == 0 || selectedFile.startsWith(rootPath))
    {
        QString relativePath = selectedFile.right(selectedFile.size() - rootPath.size() - 1);
        return relativePath;
    }
    else
    {
        QMessageBox::warning(this, tr("Cant save asset."), tr("Asset must be saved into the root directory only of the project. ")+rootPath, QMessageBox::Ok);
        return QString();
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::SaveAsFile(const QString& filter, AssetIOMessageController::Message::MessageType newAssetMessageType)
{
    if (Editor::GetInstance().GetApplicationManager().IsApplicationOpened()) 
    {
        QString selectedFile = AskFilePath(filter);
        if (selectedFile.size() != 0)
        {
            AssetIOMessageController::Message msg;
            msg.SetMessageType(newAssetMessageType);
            msg.SetString(selectedFile);
            SendAssetIoMessage(msg);
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnNewMesh(bool enabled)
{
    
    SaveAsFile(tr("Mesh (*.pas)"), AssetIOMessageController::Message::NEW_MESH);
}

void AssetLibraryWidget::OnNewProgram(bool enabled)
{
    SaveAsFile(tr("Program (*.pas)"), AssetIOMessageController::Message::NEW_PROGRAM);
}


void AssetLibraryWidget::OnNewVS(bool enabled)
{
    const Pegasus::Shader::ShaderType st = Pegasus::Shader::VERTEX;
    SaveAsFile(tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[st], Pegasus::Shader::gShaderExtensions[st]), AssetIOMessageController::Message::NEW_SHADER );
}

void AssetLibraryWidget::OnNewPS(bool enabled)
{
    const Pegasus::Shader::ShaderType st = Pegasus::Shader::FRAGMENT;
    SaveAsFile(tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[st], Pegasus::Shader::gShaderExtensions[st]), AssetIOMessageController::Message::NEW_SHADER );
}

void AssetLibraryWidget::OnNewTCS(bool enabled)
{
    const Pegasus::Shader::ShaderType st = Pegasus::Shader::TESSELATION_CONTROL;
    SaveAsFile(tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[st], Pegasus::Shader::gShaderExtensions[st]), AssetIOMessageController::Message::NEW_SHADER);
}

void AssetLibraryWidget::OnNewTES(bool enabled)
{
    const Pegasus::Shader::ShaderType st = Pegasus::Shader::TESSELATION_EVALUATION;
    SaveAsFile(tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[st], Pegasus::Shader::gShaderExtensions[st]), AssetIOMessageController::Message::NEW_SHADER);
}

void AssetLibraryWidget::OnNewGS(bool enabled)
{
    const Pegasus::Shader::ShaderType st = Pegasus::Shader::GEOMETRY;
    SaveAsFile(tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[st], Pegasus::Shader::gShaderExtensions[st]), AssetIOMessageController::Message::NEW_SHADER);
}

void AssetLibraryWidget::OnNewCS(bool enabled)
{
    const Pegasus::Shader::ShaderType st = Pegasus::Shader::COMPUTE;
    SaveAsFile(tr("%1 Shader (*.%2)").arg(Pegasus::Shader::gShaderTypeNames[st], Pegasus::Shader::gShaderExtensions[st]), AssetIOMessageController::Message::NEW_SHADER);
}

void AssetLibraryWidget::OnNewTexture(bool enabled)
{
    //SaveAsFile(tr("Texture (*.pas)"), TEXTURE);
}

void AssetLibraryWidget::OnNewTimelineScript(bool enabled)
{
    SaveAsFile(tr("Timeline Block Script (*.bs)"), AssetIOMessageController::Message::NEW_TIMELINESCRIPT);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughShaderView(const QModelIndex& index)
{
    Pegasus::Core::ISourceCodeProxy * code = mShaderListModel->Translate(index);
    Pegasus::AssetLib::IAssetProxy* asset = code->GetOwnerAsset();
    if (asset != nullptr)
    {
        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::OPEN_ASSET);
        msg.SetString(asset->GetPath());
        SendAssetIoMessage(msg);
    }
    else
    {
        //TODO: Cannot open node not represneted by asset
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchAsset(const QModelIndex& assetIdx)
{
    QFileInfo fileInfo = mAssetTreeFileSystemModel->fileInfo(assetIdx);
    QString rootPath = mAssetTreeFileSystemModel->rootPath();
    if (fileInfo.isFile())
    {
        QString path = fileInfo.filePath();
        QString relativePath = path.right(path.size() - rootPath.size() - 1);

        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::OPEN_ASSET);
        msg.SetString(relativePath);
        SendAssetIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughBlockScriptView(const QModelIndex& index)
{
    Pegasus::Core::ISourceCodeProxy * code = mBlockScriptListModel->Translate(index);
    Pegasus::AssetLib::IAssetProxy* asset = code->GetOwnerAsset();
    if (asset != nullptr)
    {
        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::OPEN_ASSET);
        msg.SetString(asset->GetPath());;
        SendAssetIoMessage(msg);
    }
    else
    {
        //TODO: Cannot open shader not represented by an asset
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughProgramView(const QModelIndex& index)
{
    if (mProgramTreeModel->IsProgramIndex(index))
    {
    }
    else if (mProgramTreeModel->IsShaderIndex(index))
    {
        Pegasus::Core::ISourceCodeProxy * code = static_cast<Pegasus::Core::ISourceCodeProxy*>(mProgramTreeModel->TranslateShaderIndex(index));
        Pegasus::AssetLib::IAssetProxy* asset = code->GetOwnerAsset();
        if (asset != nullptr)
        {
            AssetIOMessageController::Message msg;
            msg.SetMessageType(AssetIOMessageController::Message::OPEN_ASSET);
            msg.SetString(asset->GetPath());
            SendAssetIoMessage(msg);
        }
        else
        {
            //TODO: Cannot open shader not represented by an asset
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::SetEnabledProgramShaderViews(bool enabled)
{
    ui.ProgramTreeView->setEnabled(enabled);
    ui.ShaderTreeView->setEnabled(enabled);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::EnableProgramShaderViews()
{
    SetEnabledProgramShaderViews(true);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DisableProgramShaderViews()
{
    SetEnabledProgramShaderViews(false);
}

//----------------------------------------------------------------------------------------

AssetLibraryWidget::~AssetLibraryWidget()
{
    delete mProgramTreeModel;
    delete mShaderListModel;
}

// SLOTS
//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* appProxy)
{
    ED_ASSERTSTR(appProxy != nullptr, "App proxy can't be null");
    if (appProxy != nullptr)
    {
        Pegasus::Shader::IShaderManagerProxy* shaderMgrProxy = appProxy->GetShaderManagerProxy();
        ED_ASSERTSTR(shaderMgrProxy != nullptr, "Shader Manager not extracted from app");
        
        if (shaderMgrProxy != nullptr)
        {
            mShaderListModel->OnAppLoaded(shaderMgrProxy);
            ui.ShaderTreeView->doItemsLayout();
            
            mProgramTreeModel->OnAppLoaded();
            ui.ProgramTreeView->doItemsLayout();
        }

        Pegasus::Timeline::ITimelineManagerProxy* timelineProxy = appProxy->GetTimelineManagerProxy();
        if (timelineProxy != nullptr)
        {
            mBlockScriptListModel->OnAppLoaded(timelineProxy);
            ui.BlockScriptTreeView->doItemsLayout();
        }

        // set the assets root folder
        QString assetRoot = appProxy->GetAssetsRoot();
        mAssetTreeFileSystemModel->setRootPath(assetRoot);
        QModelIndex idx = mAssetTreeFileSystemModel->index(assetRoot);
        ui.AssetTreeView->setRootIndex(idx);
        ui.AssetTreeView->show();
    }
    
}

void AssetLibraryWidget::UpdateUIItemsLayout()
{
    ui.ProgramTreeView->doItemsLayout();
    ui.ShaderTreeView->doItemsLayout();
    ui.BlockScriptTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnUIForAppClosed()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();

    mBlockScriptListModel->OnAppDestroyed();
    ui.BlockScriptTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnFileChanged(const QString& path)
{

}
