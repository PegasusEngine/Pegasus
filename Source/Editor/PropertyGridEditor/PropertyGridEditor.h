
#pragma once

#include <QDockWidget>
#include <Widgets/PegasusDockWidget.h>
#include <QByteArray>
#include "MessageControllers/MsgDefines.h"

class QString;
class QAction;
class QCloseEvent;
class QtProperty;
class Editor;
class PropertyGridWidget;

class PropertyGridEditor : public PegasusDockWidget
{
    Q_OBJECT

public:
    PropertyGridEditor(QWidget* widget, Editor* editor, QString displayName, AssetInstanceHandle asset);
    virtual ~PropertyGridEditor();
    virtual void SetupUi() override;
    virtual const char* GetName() const override;
    virtual const char* GetTitle() const override;

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage msg) override;
    virtual void PropertyGridEditor::OnSaveFocusedObject() override;

public slots:
    void SignalSaveCurrentAsset();
    void OnReceivePropertyUpdated(QtProperty* property);

private:
    PropertyGridWidget* mPgrid;
    AssetInstanceHandle mAsset;
    QString mDisplayName;
    QByteArray mDisplayNameAscii;
    QAction* mSaveAction;
    bool mDirty;
};
