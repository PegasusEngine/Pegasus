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
#include "ui_AssetLibraryWidget.h"

namespace Pegasus
{
    namespace Core {
        class ISourceCodeProxy;
    }
}
class CodeUserData;
class QWidget;
class QItemSelectionModel;
class QSemaphore;
class QFileSystemModel;
class QMenu;
class QFileSystemWatcher;
class CodeEditorWidget;
class ProgramTreeModel;
class SourceCodeListModel;
class SourceCodeManagerEventListener;

//! Graphics Widget meant for shader navigation & management
class AssetLibraryWidget : public QDockWidget
{
    Q_OBJECT

public:
    
    //dispatch types
    enum DispatchTypes {
        SHADER,
        PROGRAM,
        TEXTURE,
        MESH,
        BLOCKSCRIPT
    };

    AssetLibraryWidget(QWidget * parent, CodeEditorWidget * editorWidget);
    virtual ~AssetLibraryWidget();

    //! Opens an asset in the render thread (to be called by the application proxy)
    void OnRenderThreadOpenAsset (const QString& path);

    //! Closes a source code runtime object from the render thread (to be called by the application interface)
    void OnRenderThreadCloseSourceCode(CodeUserData* userData);

    //! from the render thread, creates a new asset from a new file
    void OnRenderThreadNewAsset(const QString path, int assetType);

    //! Creates the menu that contains list of assets. Does binding internally
    QMenu* CreateNewAssetMenu(const QString& name, QWidget* parent);

public slots:
    //! slot triggered when app is loaded
    void UpdateUIForAppLoaded();

    //! updates the ui items laytout
    void UpdateUIItemsLayout();

    //! slot triggered when app is unloaded
    void UpdateUIForAppFinished();

    //! slot, triggered when an asset is dispatched through the asset view
    void DispatchAsset(const QModelIndex& index);

    //! slot triggered when something has been selected through shader tree view
    void DispatchTextEditorThroughShaderView(const QModelIndex& index);

    //! slot triggered when something has been selected through program tree view
    void DispatchTextEditorThroughProgramView(const QModelIndex& index);

     //! slot triggered when something has been selected through blockscript tree view
    void DispatchTextEditorThroughBlockScriptView(const QModelIndex& index);

    //! gets the shader editor widget
    CodeEditorWidget * GetCodeEditorWidget() { return mCodeEditorWidget; }

    //! gets the shader manager event listener
    SourceCodeManagerEventListener * GetSourceCodeManagerEventListener() { return mSourceCodeManagerEventListener;}
    
    //! grays out shader / program views. Prevents any asset view thread race condition while adding new program / shader assets
    void SetEnabledProgramShaderViews(bool enabled);

    //! triggered when the ui thread is about to request a redraw on the app thread.(disables all ui shader views)
    void OnCompilationRedrawBegin();

    //! triggered when the main thread has finished a redraw for compilation
    void OnCompilationRedrawEnd();

    //! triggered when somebody requests to open a ISourceCodeProxy, on the UI thread
    void ReceiveOpenCodeSignal(Pegasus::Core::ISourceCodeProxy* code);

    //! functions that trigger a new asset
    void OnNewMesh(bool enabled);
    void OnNewProgram(bool enabled);
    void OnNewVS(bool enabled);
    void OnNewPS(bool enabled);
    void OnNewTCS(bool enabled);
    void OnNewTES(bool enabled);
    void OnNewGS(bool enabled);
    void OnNewCS(bool enabled);
    void OnNewTexture(bool enabled);
    void OnNewTimelineScript(bool enabled);
    void OnSaveCode(CodeUserData* code);


    //slots on file system changes
private slots:
    void OnFileChanged(const QString& path);

signals:

    //! sends event to request opening an asset
    void DispatchAssetCreation(const QString& path);

    //! internal signal, sent it to switch to UI thread
    void RequestOpenCode(Pegasus::Core::ISourceCodeProxy* shaderProxy);

    //! signals from the render thread to the ui thread to update ui layout
    void RequestUpdateUIItemsLayout();

    //! signals from the ui thread to the render thread to create a new asset
    void RequestNewAsset(QString path, int assetType);

private:

    //! Asks through a popup the file name to use to save this asset
    //! \param the filter to use
    //! \return a blank string if unsuccessful, otherwise a string
    QString AskFilePath(const QString& filter);

    //! Saves a new asset from scratch. Triggers the event chain from ui -> render -> ui
    //! \param the filter to use
    void SaveAsFile(const QString& filter, int assetType);

    //! updates all the blockscript user data (fills in for blockscripts lacking of user data).
    void UpdateBlockScriptUserData();

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
    
    //! reference to the shader text editor widget
    CodeEditorWidget * mCodeEditorWidget;

    //! shader manager event listener
    SourceCodeManagerEventListener * mSourceCodeManagerEventListener;

    //! file system watcher to detect any changes outside pegasus on asset files
    QFileSystemWatcher* mFileSystemWatcher;


};

#endif
