#include <PropertyGridEditor/PropertyGridEditor.h>
#include <PropertyGrid/PropertyGridWidget.h>
#include <Application/ApplicationManager.h>
#include <Application/Application.h>
#include <Pegasus/Application/Shared/IApplicationProxy.h>
#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include <QString>
#include <QToolBar>
#include <QIcon>

PropertyGridEditor::PropertyGridEditor(QWidget* widget, Editor* editor, QString displayName, AssetInstanceHandle asset)
: PegasusDockWidget(widget, editor)
, mPgrid(nullptr)
, mAsset(asset)
, mSaveAction(nullptr)
, mDisplayName(displayName)
, mDirty(false)
{
    mDisplayNameAscii = mDisplayName.toLocal8Bit();
}

void PropertyGridEditor::SetupUi()
{
    QWidget * mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainWidget->setLayout(mainLayout);
    setWidget(mainWidget);

    QIcon saveIcon(tr(":/CodeEditor/save.png"));
    QToolBar* toolBar = new QToolBar(mainWidget);
    mainLayout->addWidget(toolBar);
    toolBar->setIconSize(QSize(16,16));

    mSaveAction = toolBar->addAction(saveIcon, tr("save this program to its asset file"));
    connect(mSaveAction, SIGNAL(triggered(bool)),
            this, SLOT(SignalSaveCurrentAsset()));

    mPgrid = new PropertyGridWidget(this);
    mPgrid->SetMessenger(this);
    mPgrid->SetCurrentProxy(mAsset);
	mPgrid->SetApplicationProxy(GetEditor()->GetApplicationManager().GetApplication()->GetApplicationProxy());
    connect(mPgrid, SIGNAL(OnPropertyUpdated(QtProperty*)),
        this, SLOT(OnReceivePropertyUpdated(QtProperty*)));
    mainLayout->addWidget(mPgrid);
}

void PropertyGridEditor::OnReceivePropertyUpdated(QtProperty* property)
{
    if (!mDirty)
    {
        emit OnRegisterDirtyObject(mAsset);
        mDirty = true;
    }
    setWindowTitle(tr(GetTitle()) + "*");
}

const char* PropertyGridEditor::GetName() const
{
    return "Property Editor";
}

const char* PropertyGridEditor::GetTitle() const
{
    return mDisplayNameAscii.constData();
}

void PropertyGridEditor::closeEvent(QCloseEvent* event)
{
    if (mDirty)
    {
        QMessageBox::StandardButton reply = Editor::AskSaveQuestion(this);
        if (reply == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
        else if (reply == QMessageBox::Yes)
        {
            SignalSaveCurrentAsset();
        }
    }

    event->accept();
    mPgrid->ClearProperties();
 
    AssetIOMCMessage msg;
    msg.SetMessageType(AssetIOMCMessage::CLOSE_ASSET);
    msg.SetObject(mAsset);
    SendAssetIoMessage(msg);
 
    GetEditor()->ClosePropertyGridAsset(mAsset);
}

void PropertyGridEditor::SignalSaveCurrentAsset()
{
    AssetIOMCMessage msg;
    msg.SetMessageType(AssetIOMCMessage::SAVE_ASSET);
    msg.SetObject(mAsset);
    SendAssetIoMessage(msg);
}

void PropertyGridEditor::OnSaveFocusedObject()
{
    SignalSaveCurrentAsset();
}

void PropertyGridEditor::OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage msg)
{
    if (msg == AssetIOMCMessage::IO_SAVE_SUCCESS)
    {
        mDirty = false;
        emit OnUnregisterDirtyObject(mAsset);
        setWindowTitle(tr(GetTitle()));
    }
}

PropertyGridEditor::~PropertyGridEditor()
{
}
