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
#include "ui_AssetLibraryWidget.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "Widgets/PegasusDockWidget.h"

namespace Pegasus {

    struct PegasusAssetTypeDesc;

    namespace App 
    {
        class IApplicationProxy;
    }

}

class QWidget;
class QItemSelectionModel;
class QSemaphore;
class QFileSystemModel;
class QMenu;
class QFileSystemWatcher;
class QSignalMapper;
class ProgramTreeModel;
class SourceCodeListModel;
class Editor;

//! Graphics Widget meant for shader navigation & management
class AssetLibraryWidget : public PegasusDockWidget
{
    Q_OBJECT

public:
    
    AssetLibraryWidget(QWidget * parent, Editor* editor);
    virtual ~AssetLibraryWidget();

    //! Creates the menu that contains list of assets. Does binding internally
    QMenu* CreateNewAssetMenu(const QString& name, QWidget* parent);

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi();
    
    //! Returns the name this widget
    virtual const char* GetName() const { return "AssetLibraryWidget"; }
    
    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "Asset Library"; }

public slots:

    //! updates the ui items laytout
    void UpdateUIItemsLayout();

    //! slot, triggered when an asset is dispatched through the asset view
    void DispatchAsset(const QModelIndex& index);

    //! slot triggered when something has been selected through shader tree view
    void DispatchTextEditorThroughShaderView(const QModelIndex& index);

    //! slot triggered when something has been selected through program tree view
    void DispatchTextEditorThroughProgramView(const QModelIndex& index);

     //! slot triggered when something has been selected through blockscript tree view
    void DispatchTextEditorThroughBlockScriptView(const QModelIndex& index);

    //! grays/ungrays out shader / program views. Prevents any asset view thread race condition while adding new program / shader assets
    void SetEnabledProgramShaderViews(bool enabled);

    //! ungrays out shader / program views. Prevents any asset view thread race condition while adding new program / shader assets
    void EnableProgramShaderViews();

    //! grays out shader / program views. Prevents any asset view thread race condition while adding new program / shader assets
    void DisableProgramShaderViews();

    //! functions that trigger a new asset
    void OnNewObject(int index);


    //slots on file system changes
private slots:
    void OnFileChanged(const QString& path);

private:

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application);

    //! slot triggered when app is unloaded
    virtual void OnUIForAppClosed();

    //! Asks through a popup the file name to use to save this asset
    //! \param the filter to use
    //! \return a blank string if unsuccessful, otherwise a string
    QString AskFilePath(const QString& filter);

    //! Saves a new asset from scratch. Triggers the event chain from ui -> render -> ui
    //! \param the filter to use
    void SaveAsFile(const QString& filter, const Pegasus::PegasusAssetTypeDesc* desc);

    //! ui components
    Ui::AssetLibraryWidget ui;

    //! reference to the program tree data model
    ProgramTreeModel * mProgramTreeModel;

    //! reference to the shader list selection model (used to determine what shader is selected)
    QFileSystemModel* mAssetTreeFileSystemModel;

    //! reference to the asset list selection model
    QItemSelectionModel * mAssetTreeSelectionModel;

    //! reference to the program tree selection model (used to determine what is selected)
    QItemSelectionModel * mProgramSelectionModel;

    //! reference to the shader list data model
    SourceCodeListModel * mShaderListModel;

    //! reference to the block script list data model
    SourceCodeListModel * mBlockScriptListModel;

    //! reference to the shader list selection model (used to determine what shader is selected)
    QItemSelectionModel * mShaderListSelectionModel;

    //! reference to the shader list selection model (used to determine what shader is selected)
    QItemSelectionModel * mBlockScriptListSelectionModel;

    //! file system watcher to detect any changes outside pegasus on asset files
    QFileSystemWatcher* mFileSystemWatcher;

    //! Mappers for all new menus
    QVector<QSignalMapper*> mNewButtonMappers;

};

#endif
