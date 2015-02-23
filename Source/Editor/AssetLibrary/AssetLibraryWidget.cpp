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
#include "CodeEditor/CodeEditorWidget.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
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


//dispatch types for source code
static const int SHADER = 1;
static const int PROGRAM = 2;
static const int TEXTURE = 3;
static const int MESH = 4;
static const int BLOCKSCRIPT = 5;


AssetLibraryWidget::AssetLibraryWidget(QWidget * parent, CodeEditorWidget * editorWidget)
: QDockWidget(parent), mCodeEditorWidget(editorWidget)
{

    mProgramTreeModel = new ProgramTreeModel(this);
    mProgramSelectionModel = new QItemSelectionModel(mProgramTreeModel);

    mShaderListModel = new SourceCodeListModel(this);
    mShaderListSelectionModel = new QItemSelectionModel(mShaderListModel);

    mBlockScriptListModel = new SourceCodeListModel(this);
    mBlockScriptListSelectionModel = new QItemSelectionModel(mBlockScriptListModel);

    mSourceCodeManagerEventListener = new SourceCodeManagerEventListener(this);

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


    //Queued connections, from events than come directly from the app
    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationError(CodeUserData*,int,QString)),
        mCodeEditorWidget, SLOT(SignalCompilationError(CodeUserData*,int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnLinkingEvent(QString,int)),
        mCodeEditorWidget, SLOT(SignalLinkingEvent(QString,int)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationBegin(CodeUserData*)),
        mCodeEditorWidget, SLOT(SignalCompilationBegin(CodeUserData*)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationEnd(QString)),
        mCodeEditorWidget, SLOT(SignalCompilationEnd(QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnBlessUserData(CodeUserData*)),
        mCodeEditorWidget, SLOT(BlessUserData(CodeUserData*)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnUnblessUserData(CodeUserData*)),
        mCodeEditorWidget, SLOT(UnblessUserData(CodeUserData*)), Qt::QueuedConnection);

    connect(mCodeEditorWidget, SIGNAL(RequestDisableAssetLibraryUi()),
        this, SLOT(OnCompilationRedrawBegin()), Qt::DirectConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalSaveSuccess()),
        mCodeEditorWidget, SLOT(SignalSavedFileSuccess()), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalSavedFileError(int, QString)),
        mCodeEditorWidget, SLOT(SignalSavedFileIoError(int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalUpdateUIViews()),
        this, SLOT(OnCompilationRedrawEnd()), Qt::QueuedConnection);
    
    connect(mCodeEditorWidget, SIGNAL(RequestSafeDeleteUserData(CodeUserData*)),
        mSourceCodeManagerEventListener, SLOT(SafeDestroyUserData(CodeUserData*)), Qt::QueuedConnection);

    connect(this, SIGNAL(RequestOpenCode(Pegasus::Core::ISourceCodeProxy*)),
            this, SLOT(ReceiveOpenCodeSignal(Pegasus::Core::ISourceCodeProxy*)), Qt::QueuedConnection);

    connect(this, SIGNAL(RequestUpdateUIItemsLayout()),
            this, SLOT(UpdateUIItemsLayout()), Qt::QueuedConnection);

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

        emit(DispatchAssetCreation(relativePath));

        return relativePath;
    }
    else
    {
        QMessageBox::warning(this, tr("Cant save asset."), tr("Asset must be saved into the root directory only of the project. ")+rootPath, QMessageBox::Ok);
        return QString();
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::SaveAsFile(const QString& filter, int type)
{
    if (Editor::GetInstance().GetApplicationManager().IsApplicationOpened()) 
    {
        QString selectedFile = AskFilePath(filter);
        if (selectedFile.size() != 0)
        {
            emit(RequestNewAsset(selectedFile, type));
        }
    }
}

void AssetLibraryWidget::OnNewMesh(bool enabled)
{
    SaveAsFile(tr("Mesh (*.pas)"), MESH);
}

void AssetLibraryWidget::OnNewProgram(bool enabled)
{
    SaveAsFile(tr("Program (*.pas)"), PROGRAM);
}


void AssetLibraryWidget::OnNewVS(bool enabled)
{
    SaveAsFile(tr("Vertex Shader (*.vs)"), SHADER);
}

void AssetLibraryWidget::OnNewPS(bool enabled)
{
    SaveAsFile(tr("Pixel Shader (*.ps)"), SHADER);
}

void AssetLibraryWidget::OnNewTCS(bool enabled)
{
    SaveAsFile(tr("Tesselation Control Shader (*.tcs)"), SHADER);
}

void AssetLibraryWidget::OnNewTES(bool enabled)
{
    SaveAsFile(tr("Tesselation Evaluation Shader (*.tes)"), SHADER);
}

void AssetLibraryWidget::OnNewGS(bool enabled)
{
    SaveAsFile(tr("Geometry Shader (*.gs)"), SHADER);
}

void AssetLibraryWidget::OnNewCS(bool enabled)
{
    SaveAsFile(tr("Compute Shader (*.cs)"), SHADER);
}

void AssetLibraryWidget::OnNewTexture(bool enabled)
{
    //SaveAsFile(tr("Texture (*.pas)"), TEXTURE);
}

void AssetLibraryWidget::OnNewTimelineScript(bool enabled)
{
    //SaveAsFile(tr("Timeline Block Script (*.bs)"), BLOCKSCRIPT);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughShaderView(const QModelIndex& index)
{
    mCodeEditorWidget->show();
    mCodeEditorWidget->activateWindow();
    Pegasus::Core::ISourceCodeProxy * code = mShaderListModel->Translate(index);
    CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
    userData->SetDispatchType(SHADER);
    mCodeEditorWidget->RequestOpen(userData);
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

        emit(DispatchAssetCreation(relativePath));

    }
}


//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnRenderThreadOpenAsset(const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Application* app = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy* appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy can't be null");
        Pegasus::AssetLib::IAssetLibProxy* assetLib = appProxy->GetAssetLibProxy();
        
        Pegasus::AssetLib::IAssetProxy* asset = nullptr;
        if (Pegasus::Io::ERR_NONE == assetLib->LoadAsset(asciiPath, &asset))
        {

            ED_ASSERT(asset != nullptr);
            //factories:
            Pegasus::Shader::IShaderManagerProxy*  shaderManagerProxy  = appProxy->GetShaderManagerProxy();
            Pegasus::Timeline::ITimelineProxy* timelineProxy = appProxy->GetTimelineProxy();
            //Pegasus::Shader::ITextureManagerProxy* textureManagerProxy = appProxy->GetTextureManagerProxy();
            //Pegasus::Shader::IMeshManagerProxy*    meshManagerProxy    = appProxy->GetMeshManagerProxy();

            //select which asset factory to use
            if (shaderManagerProxy->IsShader(asset))
            {
                Pegasus::Shader::IShaderProxy* openedShader = shaderManagerProxy->OpenShader(asset);
                if (openedShader != nullptr)
                {
                    static_cast<CodeUserData*>(openedShader->GetUserData())->SetDispatchType(SHADER);
                    emit(RequestOpenCode(openedShader));
                }
            } 
            else if (shaderManagerProxy->IsProgram(asset))
            {
                //todo: implement a program editor widget?
            }
            else if (timelineProxy->IsTimelineScript(asset))
            {
                Pegasus::Core::ISourceCodeProxy* openedTimelineScript = timelineProxy->OpenScript(asset);
                if (openedTimelineScript != nullptr)
                {
                    static_cast<CodeUserData*>(openedTimelineScript->GetUserData())->SetDispatchType(BLOCKSCRIPT);
                    emit (RequestOpenCode(openedTimelineScript));
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnRenderThreadNewAsset(const QString path, int assetType)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Application* app = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy* appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy can't be null");
        Pegasus::AssetLib::IAssetLibProxy* assetLib = appProxy->GetAssetLibProxy();
        Pegasus::AssetLib::IAssetProxy* asset = nullptr;
        Pegasus::Io::IoError errCode = assetLib->CreateBlankAsset(asciiPath, &asset);
        if (errCode == Pegasus::Io::ERR_NONE)
        {
            //we have a file in disk guaranteed
            switch(assetType)
            {
            case PROGRAM:
            break;
            case SHADER:
            {
                Pegasus::Shader::IShaderManagerProxy* shaderMgr = appProxy->GetShaderManagerProxy();
                Pegasus::Shader::IShaderProxy* shader = shaderMgr->OpenShader(asset);
                if (shader != nullptr)
                {
                    static_cast<CodeUserData*>(shader->GetUserData())->SetDispatchType(SHADER);
                    emit(RequestOpenCode(shader));
                }
            }
            break;
            case BLOCKSCRIPT:
            {
                Pegasus::Timeline::ITimelineProxy* timelineMgr = appProxy->GetTimelineProxy();
                Pegasus::Core::ISourceCodeProxy* code = timelineMgr->OpenScript(asset);
                if (code != nullptr)
                {
                    static_cast<CodeUserData*>(code->GetUserData())->SetDispatchType(BLOCKSCRIPT);
                    emit(RequestOpenCode(code));
                }
            }
            break;
            }
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnRenderThreadCloseSourceCode(CodeUserData* userData)
{
    switch(userData->GetDispatchType())
    {
    case SHADER:
        {
            userData->SetDispatchType(0);
            Pegasus::Shader::IShaderManagerProxy* shaderMgrProxy = Editor::GetInstance().GetApplicationManager().GetApplication()->GetApplicationProxy()->GetShaderManagerProxy();
            shaderMgrProxy->CloseShader(static_cast<Pegasus::Shader::IShaderProxy*>(userData->GetSourceCode()));
            emit(RequestUpdateUIItemsLayout());
        }
        break;
    case BLOCKSCRIPT:
        {
            userData->SetDispatchType(0);
            Pegasus::Timeline::ITimelineProxy* timelineProxy = Editor::GetInstance().GetApplicationManager().GetApplication()->GetApplicationProxy()->GetTimelineProxy();
            timelineProxy->CloseScript(userData->GetSourceCode());
            emit(RequestUpdateUIItemsLayout());
        }
        break;
    default:
        ED_FAILSTR("Invalid source code to close!!");
        break;
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughBlockScriptView(const QModelIndex& index)
{
    mCodeEditorWidget->show();
    mCodeEditorWidget->activateWindow();
    Pegasus::Core::ISourceCodeProxy * code = mBlockScriptListModel->Translate(index);
    CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
    userData->SetDispatchType(BLOCKSCRIPT);
    mCodeEditorWidget->RequestOpen(userData);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughProgramView(const QModelIndex& index)
{
    if (mProgramTreeModel->IsProgramIndex(index))
    {
    }
    else if (mProgramTreeModel->IsShaderIndex(index))
    {
        mCodeEditorWidget->show();
        mCodeEditorWidget->activateWindow();
        Pegasus::Core::ISourceCodeProxy * code = static_cast<Pegasus::Core::ISourceCodeProxy*>(mProgramTreeModel->TranslateShaderIndex(index));
        CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
        userData->SetDispatchType(PROGRAM);
        mCodeEditorWidget->RequestOpen(userData);
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::SetEnabledProgramShaderViews(bool enabled)
{
    ui.ProgramTreeView->setEnabled(enabled);
    ui.ShaderTreeView->setEnabled(enabled);
}
void AssetLibraryWidget::OnCompilationRedrawBegin()
{
    SetEnabledProgramShaderViews(false);
}

void AssetLibraryWidget::OnCompilationRedrawEnd()
{
    SetEnabledProgramShaderViews(true);
    UpdateUIItemsLayout();
}

//----------------------------------------------------------------------------------------

AssetLibraryWidget::~AssetLibraryWidget()
{
    delete mSourceCodeManagerEventListener;
    delete mProgramTreeModel;
    delete mShaderListModel;
}

// SLOTS
//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateUIForAppLoaded()
{
    Application* app = Editor::GetInstance().GetApplicationManager().GetApplication();
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy* appProxy = app->GetApplicationProxy();
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

            Pegasus::Timeline::ITimelineProxy* timelineProxy = appProxy->GetTimelineProxy();
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
}

void AssetLibraryWidget::UpdateUIItemsLayout()
{
    ui.ProgramTreeView->doItemsLayout();
    ui.ShaderTreeView->doItemsLayout();
    ui.BlockScriptTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateUIForAppFinished()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();

    mBlockScriptListModel->OnAppDestroyed();
    ui.BlockScriptTreeView->doItemsLayout();
}

void AssetLibraryWidget::OnFileChanged(const QString& path)
{

}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::ReceiveOpenCodeSignal(Pegasus::Core::ISourceCodeProxy* sourceCode)
{
    ED_ASSERT(sourceCode->GetUserData() != nullptr);
    mCodeEditorWidget->show();
    mCodeEditorWidget->activateWindow();
    CodeUserData* userData = static_cast<CodeUserData*>(sourceCode->GetUserData());
    mCodeEditorWidget->RequestOpen(userData);
    UpdateUIItemsLayout();
}
