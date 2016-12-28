/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetLibraryWidget.h
//! \author	Kleber Garcia
//! \date	16 Match 2014
//! \brief	Graphics widget representing the shader library form.

#ifndef EDITOR_SHADERLIBRARYWIDGET_H
#define EDITOR_SHADERLIBRARYWIDGET_H

#include <QDockWidget>
#include <QAction>
#include <qvector.h>
#include "MessageControllers/MsgDefines.h"
#include "Widgets/PegasusDockWidget.h"


namespace Pegasus {

    struct PegasusAssetTypeDesc;

    namespace App 
    {
        class IApplicationProxy;
    }

}

namespace Ui
{
    class AssetLibraryWidget;
}

class QWidget;
class QItemSelectionModel;
class QSemaphore;
class QFileSystemModel;
class QMenu;
class QFileSystemWatcher;
class QSignalMapper;
class QNewAssetDialog;
class Editor;
class InstanceViewer;

//! Graphics Widget meant for shader navigation & management
class AssetLibraryWidget : public PegasusDockWidget
{
    Q_OBJECT

public:
        
    AssetLibraryWidget(QWidget * parent, Editor* editor);
    virtual ~AssetLibraryWidget();

    //! Callback fired when the UI needs to be set.
    void SetupUi();
    
    //! Returns the name this widget
    virtual const char* GetName() const { return "AssetLibraryWidget"; }
    
    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "Asset Library"; }

signals:
    //! Highlights a block.
    void OnHighlightBlock(unsigned blockGuid);

public slots:

    //! slot, triggered when an asset is dispatched through the asset view
    void DispatchAsset(const QModelIndex& index);

    //! Opens an asset.
    void OnOpenObject(QString assetPath);

    void StartNewDialog();

private slots:
    //slots on file system changes
    void OnFileChanged(const QString& path);

    void OnFilterByCategoryPressed();

    void OnFilterByTypePressed();

private:

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application);

    //! slot triggered when app is unloaded
    virtual void OnUIForAppClosed();

    //! Resets the state of all the button filters.
    void ResetAllFilterButtons();

    //! ui components
    Ui::AssetLibraryWidget* mUi;

    //! procedural dialog for new asset
    QNewAssetDialog* mNewAssetDialog;


    //! reference to the shader list selection model (used to determine what shader is selected)
    QFileSystemModel* mAssetTreeFileSystemModel;

    //! reference to the asset list selection model
    QItemSelectionModel * mAssetTreeSelectionModel;
    
    //! file system watcher to detect any changes outside pegasus on asset files
    QFileSystemWatcher* mFileSystemWatcher;

    //! Mappers for all new menus
    QVector<QSignalMapper*> mNewButtonMappers;

    //! Reference to the instance viewer (manipulates the tree widget).
    InstanceViewer* mInstanceViewer;

};

#endif
