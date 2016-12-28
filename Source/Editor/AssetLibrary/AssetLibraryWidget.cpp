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
#include "Pegasus/PegasusAssetTypes.h"
#include "Application/ApplicationManager.h"
#include "AssetLibrary/AssetLibraryWidget.h"
#include "AssetLibrary/InstanceViewer.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Core/Shared/IoErrors.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/Version.h"

#include "ui_AssetLibraryWidget.h"

#include <QSignalMapper>
#include <QMenu>
#include <QGroupBox>
#include <QItemSelectionModel>
#include <QTextDocument>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <qfilesystemwatcher.h>
#include <qmessagebox.h>
#include <qerrormessage.h>

#include "Widgets/PegasusDockWidget.h"

//new asset dialog
class QNewAssetDialog : public QDialog
{
public:
    QNewAssetDialog(AssetLibraryWidget* parent) :
        QDialog(parent),
        mAppProxy(nullptr)
    {
        setupUI();
    }

public:

    void OnAppOpened(Pegasus::App::IApplicationProxy* application)
    {
        application->GetEngineDesc(mEngineDesc);
        setupTypeSelector();
        mAppProxy = application;
    }
    
    void OnAppClosed()
    {
        destroyTypeSelector();
    }

    void ClearTextPrompts()
    {
        mTargetFile = tr("");
        mFileLabel->setText(mTargetFile);
    }

private:

    void setupUI()
    {
        QVBoxLayout* mainLayout = new QVBoxLayout();
        
        QWidget* prompt = new QWidget(this);
        QHBoxLayout* promptLayout = new QHBoxLayout();
        QLabel* typeLabel = new QLabel(tr("Type:"));
        promptLayout->addWidget(typeLabel);
        mTypeCombo = new QComboBox(this);        
        mTypeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        promptLayout->addWidget(mTypeCombo);
        prompt->setLayout(promptLayout);
        mainLayout->addWidget(prompt);

        QWidget* fileSelect = new QWidget(this);
        QHBoxLayout* fileSelectLayout = new QHBoxLayout();
        mFileLabel = new QLabel(this);
        fileSelectLayout->addWidget(mFileLabel);
        fileSelectLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum));        
        QPushButton* button = new QPushButton(this);
        button->setText(tr("...")); 
        connect(button, &QPushButton::clicked,
                this, &QNewAssetDialog::SelectFile);
        fileSelectLayout->addWidget(button);
        fileSelect->setLayout(fileSelectLayout);
        mainLayout->addWidget(fileSelect); 
        mainLayout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::Expanding));

        QWidget* decision = new QWidget(this);
        QHBoxLayout* decisionLayout = new QHBoxLayout(); 
        decisionLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum));
        QPushButton* createButton = new QPushButton(this);
        createButton->setText(tr("Create"));
        decisionLayout->addWidget(createButton);
        connect(createButton, &QPushButton::clicked,
                this, &QNewAssetDialog::Create);
        QPushButton* cancelButton = new QPushButton(this);
        cancelButton->setText(tr("Cancel"));
        connect(cancelButton, SIGNAL(clicked()),
                this, SLOT(close()));
        decisionLayout->addWidget(cancelButton);
        decision->setLayout(decisionLayout);
        mainLayout->addWidget(decision); 
        
        setLayout(mainLayout);
    }
    
    void setupTypeSelector()
    {
        for (int i = 0; mEngineDesc.mAssetTypes[i] != nullptr; ++i)
        {
            QIcon assetIcon (tr(mEngineDesc.mAssetTypes[i]->mIconPath));
            mTypeCombo->addItem(assetIcon, mEngineDesc.mAssetTypes[i]->mTypeName);
        }
    }

    void destroyTypeSelector()
    {
        mTypeCombo->clear();
    }

private:
    void SelectFile()
    {
        if (mTypeCombo->currentIndex() == -1)
        {
            QMessageBox::warning(
                this, 
                tr("Can't pick target file."), 
                tr("Must select a proper asset type in order to create the target file."), QMessageBox::Ok);
        }
        else
        {
            const Pegasus::PegasusAssetTypeDesc* typeDesc = mEngineDesc.mAssetTypes[mTypeCombo->currentIndex()];
            QString ext = tr(typeDesc->mExtension);
            QString assetName = tr(typeDesc->mTypeName);
            QString root = tr(mAppProxy->GetAssetsRoot());
            QString filter = assetName + " ( *." + ext + ")";
            QString newFile = QFileDialog::getSaveFileName(this, tr("Save Asset."), root, filter);
            if (newFile.size() > 0)
            {
                mTargetFile = SanitizeSlashes(newFile);
                mFileLabel->setText(mTargetFile);
            }
        }
    }

    void Create()
    {
        QString basePath = SanitizeSlashes(tr(mAppProxy->GetAssetsRoot()));
        if (mTypeCombo->currentIndex() == -1)
        {
            QMessageBox::warning(
                this, 
                tr("Can't create asset."), 
                tr("Must select a proper asset type in order to create the target file."), QMessageBox::Ok);
        }
        else if (mTargetFile.size() == 0 || !mTargetFile.startsWith(basePath))
        {
            QMessageBox::warning(
                this, 
                tr("Can't create asset."), 
                tr("Invalid file path. Make sure the file path is in the proper root folder and is not empty."), QMessageBox::Ok);
        }
        else
        {
            QString newFilePath = mTargetFile.right(mTargetFile.size() - basePath.size());
            const Pegasus::PegasusAssetTypeDesc* typeDesc = mEngineDesc.mAssetTypes[mTypeCombo->currentIndex()];

            if (!newFilePath.endsWith(tr(typeDesc->mExtension)))
            {
                newFilePath += tr(".")+tr(typeDesc->mExtension);
            }

            AssetLibraryWidget* parent = static_cast<AssetLibraryWidget*>(parentWidget());
            AssetIOMCMessage msg(AssetIOMCMessage::NEW_ASSET);
            msg.SetString(newFilePath);
            msg.SetTypeDesc(typeDesc);
            parent->OnSendAssetIoMessage(parent, msg);            
            close();
        }    
    }

    QString SanitizeSlashes(QString in)
    {
        return in.replace('\\','/');
    }
    Pegasus::PegasusDesc mEngineDesc;

    QComboBox* mTypeCombo;
    Pegasus::App::IApplicationProxy* mAppProxy;
    QLabel * mFileLabel;
    QString mTargetFile;

};

AssetLibraryWidget::AssetLibraryWidget(
        QWidget * parent,
        Editor* editor
)
: PegasusDockWidget(parent, editor)
{
    mUi = new Ui::AssetLibraryWidget();
}

void AssetLibraryWidget::SetupUi()
{
    mAssetTreeFileSystemModel = new QFileSystemModel(this);
    mAssetTreeSelectionModel = new QItemSelectionModel(mAssetTreeFileSystemModel);

    //TODO: use this file watcher to resolve external changes in real time to files
    mFileSystemWatcher = new QFileSystemWatcher();

    mUi->setupUi(this);

    connect(mUi->AssetTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchAsset(QModelIndex)));

    connect(mFileSystemWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(OnFileChanged(QString)));

    connect(mUi->AddAssetButton, SIGNAL(clicked()),
            this, SLOT(StartNewDialog()));

    mUi->AssetTreeView->setModel(mAssetTreeFileSystemModel);
    mUi->AssetTreeView->setSelectionModel(mAssetTreeSelectionModel);

    mNewAssetDialog = new QNewAssetDialog(this);

    mUi->AddAssetButton->setEnabled(false);

    mInstanceViewer = new InstanceViewer(mUi->instanceTreeWidget);
    connect(mInstanceViewer, SIGNAL(RequestOpenAsset(QString)), this, SLOT(OnOpenObject(QString)));
    connect(mInstanceViewer, SIGNAL(RequestHighlightBlock(unsigned)), this, SIGNAL(OnHighlightBlock(unsigned)));

    connect(mUi->sortByCategoryButton, SIGNAL(clicked()),
            this, SLOT(OnFilterByCategoryPressed()));
    connect(mUi->sortByTypeButton, SIGNAL(clicked()),
            this, SLOT(OnFilterByTypePressed()));

    mUi->sortByCategoryButton->setCheckable(true);
    mUi->sortByTypeButton->setCheckable(true);
    //initial state of instance view is by type
    OnFilterByCategoryPressed();
}

void AssetLibraryWidget::ResetAllFilterButtons()
{
    mUi->sortByCategoryButton->setChecked(false);
    mUi->sortByTypeButton->setChecked(false);
}

void AssetLibraryWidget::OnFilterByCategoryPressed()
{
    ResetAllFilterButtons();
    mUi->sortByCategoryButton->setChecked(true);
    mInstanceViewer->SetFilterMode(InstanceViewer::BLOCK_FILTER);
}

void AssetLibraryWidget::OnFilterByTypePressed()
{
    ResetAllFilterButtons();
    mUi->sortByTypeButton->setChecked(true);
    mInstanceViewer->SetFilterMode(InstanceViewer::TYPE_FILTER);
}


//----------------------------------------------------------------------------------------

void AssetLibraryWidget::StartNewDialog()
{
    mNewAssetDialog->ClearTextPrompts();
    mNewAssetDialog->open();
}

void AssetLibraryWidget::DispatchAsset(const QModelIndex& assetIdx)
{
    QFileInfo fileInfo = mAssetTreeFileSystemModel->fileInfo(assetIdx);
    QString rootPath = mAssetTreeFileSystemModel->rootPath();
    if (fileInfo.isFile())
    {
        QString path = fileInfo.filePath();
        QString relativePath = path.right(path.size() - rootPath.size() - 1);

        OnOpenObject(relativePath);
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnOpenObject(QString assetPath)
{
    AssetIOMCMessage msg;
    msg.SetMessageType(AssetIOMCMessage::OPEN_ASSET);
    msg.SetString(assetPath);;
    SendAssetIoMessage(msg);

}

//----------------------------------------------------------------------------------------

AssetLibraryWidget::~AssetLibraryWidget()
{
    //delete all mappers
    for (QSignalMapper* mapper : mNewButtonMappers)
    {
        delete(mapper);
    }

    delete mAssetTreeFileSystemModel;    
    delete mInstanceViewer;
    delete mUi;
}

// SLOTS
//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* appProxy)
{
    ED_ASSERTSTR(appProxy != nullptr, "App proxy can't be null");
    if (appProxy != nullptr)
    {
        // set the assets root folder
        QString assetRoot = appProxy->GetAssetsRoot();
        mAssetTreeFileSystemModel->setRootPath(assetRoot);
        QModelIndex idx = mAssetTreeFileSystemModel->index(assetRoot);
        mUi->AssetTreeView->setRootIndex(idx);
        mUi->AssetTreeView->show();

        // register this widget instance viewer as an observer
        AssetIOMCMessage msg(AssetIOMCMessage::QUERY_START_VIEW_ASSET_TREE);
        msg.SetTreeObserver(mInstanceViewer);
        SendAssetIoMessage(msg);

        mNewAssetDialog->OnAppOpened(appProxy);
        mUi->AddAssetButton->setEnabled(true);
    }
    
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnUIForAppClosed()
{    
    mNewAssetDialog->OnAppClosed();
    mUi->AddAssetButton->setEnabled(false);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnFileChanged(const QString& path)
{

}
