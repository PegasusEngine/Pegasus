/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Editor.h
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Main window of Pegasus Editor

#ifndef EDITOR_H
#define EDITOR_H

#include "Settings/Settings.h"

#include "Console/ConsoleDockWidget.h"
#include "History/HistoryDockWidget.h"
#include "Timeline/TimelineDockWidget.h"
#include "AssetLibrary/AssetLibraryWidget.h"
#include "CodeEditor/CodeEditorWidget.h"
#include "Texture/TextureEditorDockWidget.h"
#include "Debug/PropertyGridClasses/PropertyGridClassesDockWidget.h"
#include "Debug/BlockScriptLibraries/BlockScriptLibraryDockWidget.h"
#include "ProgramEditor/ProgramEditorWidget.h"
#include "Viewport/ViewportDockWidget.h"
#include "Viewport/ViewportWidget.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

#include <QtWidgets/QMainWindow>
#include <QMap>

class ApplicationManager;
class LogManager;
class AssertionManager;
class ViewportWidget;

class QSplashScreen;
class QUndoCommand;
class QAction;
class QUndoStack;


//! Main window of Pegasus Editor
class Editor : public QMainWindow
{
    Q_OBJECT

public:

    explicit Editor(QApplication * parentApplication);
    virtual ~Editor();

    //! Return the unique instance of the editor
    //! \return Reference to the unique instance of the editor
    inline static Editor & GetInstance() { return *sInstance; }

    //! Get the log manager, used by all log macros
    inline LogManager & GetLogManager() const { return *mLogManager; }

    //! Get the assertion manager, used by all assertion macros
    inline AssertionManager & GetAssertionManager() const { return *mAssertionManager; }

    //! Get the application manager
    inline ApplicationManager & GetApplicationManager() const { return *mApplicationManager; }

    //! Test if the application manager has been created yet
    //! \return True if the application manager has been created
    inline bool IsApplicationManagerAvailable() const { return mApplicationManager != nullptr; }

    //------------------------------------------------------------------------------------

public:

    //! Raise the splash screen (put the window on top)
    void RaiseSplashScreen();

    //! Close the splash screen (if opened)
    void CloseSplashScreen();

    //! Get the editor settings object
    //! \return Pointer to the editor settings object
    inline static Settings * GetSettings() { return sSettings; }

    //! Get the global Qt application
    //! \return Global Qt application
    inline QApplication * GetQtApplication() const { return mQtApplication; }

	//------------------------------------------------------------------------------------

public:

    //! Get the main viewport dock widget
    //! \return Pointer to the main viewport dock widget
    inline ViewportDockWidget * GetMainViewportDockWidget() const { return mMainViewportDockWidget; }

    //! Get the secondary viewport dock widget
    //! \return Pointer to the secondary viewport dock widget
    inline ViewportDockWidget * GetSecondaryViewportDockWidget() const { return mSecondaryViewportDockWidget; }

    //! Get the timeline dock widget
    //! \return Pointer to the timeline dock widget
    inline TimelineDockWidget * GetTimelineDockWidget() const { return mTimelineDockWidget; }

    //! Get the history dock widget
    //! \return Pointer to the history dock widget
    inline HistoryDockWidget * GetHistoryDockWidget() const { return mHistoryDockWidget; }

    //! Get the console dock widget
    //! \return Pointer to the console dock widget
    inline ConsoleDockWidget * GetConsoleDockWidget() const { return mConsoleDockWidget; }

    //! Get the Code editor widget
    //! \return Pointer to Code editor widget
    inline CodeEditorWidget * GetCodeEditorWidget() const { return mCodeEditorWidget; }

    //! Get the asset library widget
    //! \return Pointer to asset library widget
    inline AssetLibraryWidget * GetAssetLibraryWidget() const { return mAssetLibraryWidget; }

    //! Get the texture editor dock widget
    //! \return Pointer to the texture editor dock widget
    inline TextureEditorDockWidget * GetTextureEditorDockWidget() const { return mTextureEditorDockWidget; }

    //! Get the property grid classes dock widget
    //! \return Pointer to the property grid classes dock widget
    inline PropertyGridClassesDockWidget * GetPropertyGridClassesDockWidget() const { return mPropertyGridClassesDockWidget; }

    //! Get the bs lib dock widget
    //! \return Pointer to the bs lib dock widget
    inline BlockScriptLibraryDockWidget * GetBlockScriptLibraryWidget() const { return mBsLibWidget; }

    //! Get the program editor dock widget
    //! \return Pointer to the program editor dock widget
    inline ProgramEditorWidget * GetProgramEditorWidget() const { return mProgramEditorWidget; }

    //! Gets the list of widgets for this editor
    inline const QVector<PegasusDockWidget*>& GetDockWidgets() const { return mDockWidgets; }

    //! Gets the list of viewport widgets for this editor
    inline const QVector<ViewportWidget*>& GetViewportWidgets() const { return mViewportWidgets; }

    //----------------------------------------------------------------------------------------

private:

    //! Create the set of QT actions needed by the main window
    void CreateActions();

    //! Create the menu of the main window
    void CreateMenu();

    //! Create the tool bars of the main window
    void CreateToolBars();

    //! Create the dock widgets of the main window
    void CreateDockWidgets();

    //! Create the status bar of the main window
    void CreateStatusBar();
   
    //! closes the application using a pump event pattern (to allow for operating system window events to 
    //! be handled
    void InternalCloseAndPumpEvents();

    //! Registers a pegasus dock widget, and registers all its common functions
    void RegisterWidget( PegasusDockWidget* widget, Qt::DockWidgetArea area);

    //------------------------------------------------------------------------------------

protected:

    // Overrides

    //! Function called when the main window is asked to be closed
    //! \param event Description of the event
    void closeEvent(QCloseEvent * event);
    
    //------------------------------------------------------------------------------------

private slots:

    //@{
    //! Slots for the actions of the File menu
    void OpenApp();
    void ReloadApp();
    void CloseApp();
    void Quit();
    //@}

    //@{
    //! Slots for the actions of the Edit menu
    void OpenPreferences();
    //@}

    //@{
    //! Slots for the actions of the View menu
    void ShowFullscreenViewport();
    //@}

    //@{
    //! Slots for the actions of the Create menu
    void CreateBox();
    void CreateSphere();
    //@}

    //@{
    //! Slots for the actions of the Window menu
    void OpenMainViewportWindow();
    void OpenSecondaryViewportWindow();
    void OpenTimelineWindow();
    void OpenHistoryWindow();
    void OpenConsoleWindow();
    void OpenCodeEditorWindow();
    void OpenAssetLibraryWindow();
    void OpenTextureEditorWindow();
    void OpenProgramEditorWindow();
    //@}

    //@{
    //! Slots for the actions of the Window/Debug menu
    void OpenPropertyGridClassesWindow();
    void OpenBsLibWidget();
    //@}

    //@{
    //! Slots for the actions of the help menu
    void HelpIndex();
    void About();
    //@}


    //! Called when an application is successfully loaded
    void UpdateUIForAppLoaded();

    //! Called when an application is closed
    void UpdateUIForAppClosed();

    //!@{
    //! Called when an object has been marked as dirty / non dirty
    
    //! Called when an object has been registered as dirty
    void OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Called when an object has been unregistered as dirty
    void OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //@}

    //------------------------------------------------------------------------------------

public slots:

    // Extra slots for other parts of the editor that request global controls

    //! Open the preferences dialog box at the appearance page
    void OpenPreferencesAppearance();

    //! Open the preferences dialog box at the console page
    void OpenPreferencesConsole();

    //! Actions taken when a widget is on focus
    void OnDockFocus(PegasusDockWidget* target);

    //! Actions taken when a widget is not on focus
    void OutDockFocus(PegasusDockWidget* target);

    //! Action taken when the asset controller requests opening of assets
    void OnOpenObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //------------------------------------------------------------------------------------

private:

    //@{
    //! Actions triggered when an item of the File menu is selected
    QAction * mActionFileOpenApp;
    QAction * mActionFileReloadApp;
    QAction * mActionFileCloseApp;
    QAction * mActionFileQuit;
    //@}

    //@{
    //! Actions triggered when an item of the Edit menu is selected
    QAction * mActionEditUndo;
    QAction * mActionEditRedo;
    QAction * mActionEditPreferences;
    //@}

    //@{
    //! Actions triggered when an item of the View menu is selected
    QAction * mActionViewShowFullscreenViewport;
    //@}

    //@{
    //! Actions triggered when an item of the Create menu is selected
	QAction * mActionCreatePrimitiveBox;
    QAction * mActionCreatePrimitiveSphere;
    //@}

    //@{
    //! Actions triggered when an item of the Window menu is selected
	QAction * mActionWindowMainViewport;
	QAction * mActionWindowSecondaryViewport;
    QAction * mActionWindowTimeline;
    QAction * mActionWindowHistory;
    QAction * mActionWindowConsole;
    QAction * mActionWindowCodeEditor;
    QAction * mActionWindowAssetLibrary;
    QAction * mActionWindowTextureEditor;
    QAction * mActionProgramEditor;
    //@}

    //@{
    //! Actions triggered when an item of the Window/Debug menu is selected
	QAction * mActionWindowDebugPropertyGridClasses;
	QAction * mActionWindowDebugBsLibWidget;
    //@}

    //@{
    //! Actions triggered when an item of the help menu is selected
    QAction * mSaveCurrentAsset;
    QAction * mActionHelpIndex;
    QAction * mActionHelpAboutQt;
    QAction * mActionHelpAbout;
    //@}

private slots:
    void SaveCurrentAsset();

    //------------------------------------------------------------------------------------

private:

    //! Initial width of the main window
    static const int INITIAL_WINDOW_WIDTH = 1280;

    //! Initial height of the main window
    static const int INITIAL_WINDOW_HEIGHT = 720;


    //! Copy constructor, private to prevent any copy of the singleton
    explicit Editor(const Editor &);

    //! Copy operator, private to prevent any copy of the singleton
    Editor & operator=(const Editor &);

    //! Unique instance of the editor class
    static Editor * sInstance;

    //! Unique log manager
    LogManager * mLogManager;

    //! Unique assertion manager
    AssertionManager * mAssertionManager;


    //! Splash screen displayed at the startup of the program
	QSplashScreen * mSplashScreen;

    //! Editor settings (preferences)
	static Settings * sSettings;

    //@{
    //! Dock widgets
    ViewportDockWidget      * mMainViewportDockWidget;
    ViewportDockWidget      * mSecondaryViewportDockWidget;
    TimelineDockWidget      * mTimelineDockWidget;
    HistoryDockWidget       * mHistoryDockWidget;
    AssetLibraryWidget      * mAssetLibraryWidget;
    CodeEditorWidget        * mCodeEditorWidget;
    ConsoleDockWidget       * mConsoleDockWidget;
    TextureEditorDockWidget * mTextureEditorDockWidget;
    ProgramEditorWidget     * mProgramEditorWidget;
    PropertyGridClassesDockWidget * mPropertyGridClassesDockWidget;
    BlockScriptLibraryDockWidget*       mBsLibWidget;
    //@}

	//! Menu containing the checkable actions for the tool bars
	QMenu * mToolbarMenu;

	//! Menu containing the checkable actions for the dock widgets
	QMenu * mDockMenu;


    //! Application manager, taking take of opening and closing applications
    ApplicationManager * mApplicationManager;

    //! Global Qt application
    QApplication * mQtApplication;

    //! List holding all the dirty assets, so we warn the user there are dirty assets
    QSet<Pegasus::AssetLib::IRuntimeAssetObjectProxy*> mDirtyAssets;

    //! Master list of widgets
    QVector<PegasusDockWidget*> mDockWidgets;

    //! List of viewport widgets
    QVector<ViewportWidget*> mViewportWidgets;

    //! Mapping that maps an asset type to a widget that can process and open it.
    QMap<int, PegasusDockWidget*> mTypeGuidWidgetMapping;
};


#endif // EDITOR_H
