/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Editor.h
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Main window of Pegasus Editor

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Application/ApplicationManager.h"
#include "Settings/SettingsDialog.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"


#include  "Console/ConsoleDockWidget.h"
#include  "History/HistoryDockWidget.h"
#include  "Timeline/TimelineDockWidget.h"
#include  "AssetLibrary/AssetLibraryWidget.h"
#include  "CodeEditor/CodeEditorWidget.h"
#include  "Graph/GraphEditorDockWidget.h"
#include  "Graph/TextureGraphEditorViewStrategy.h"
#include  "Graph/MeshGraphEditorViewStrategy.h"
#include  "Debug/PropertyGridClasses/PropertyGridClassesDockWidget.h"
#include  "Debug/BlockScriptLibraries/BlockScriptLibraryDockWidget.h"
#include  "ProgramEditor/ProgramEditorWidget.h"
#include  "Viewport/ViewportDockWidget.h"
#include  "Viewport/ViewportWidget.h"

#include <QUndoStack>
#include <QSplashScreen>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QApplication>
#include <QFileDialog>
#include <QMessagebox>

Editor * Editor::sInstance = nullptr;
Settings * Editor::sSettings = nullptr;

//----------------------------------------------------------------------------------------

Editor::Editor(QApplication * parentApplication)
:   QMainWindow(nullptr),
    mApplicationManager(nullptr),
    mQtApplication(parentApplication),
    mMainViewportDockWidget(nullptr),
    mSecondaryViewportDockWidget(nullptr),
    mTimelineDockWidget(nullptr),
    mHistoryDockWidget(nullptr),
    mAssetLibraryWidget(nullptr),
    mCodeEditorWidget(nullptr),
    mConsoleDockWidget(nullptr),
    mTextureEditorDockWidget(nullptr),
    mPropertyGridClassesDockWidget(nullptr)
{
    sInstance = this;

    // Create the log manager
    mLogManager = new LogManager(this);

    // Create the assertion manager
    mAssertionManager = new AssertionManager(this);

    // Create the splash screen (it becomes visible once this class is initialized,
    // set by the application class)
    //! \todo Finish implementing the splash screen
    //mSplashScreen = new QSplashScreen(Qpixmap(EDITOR_SPLASH_IMAGE_FILE));
    //mSplashScreen->show();

    // Set the title of the main window
    //! \todo Use a constant in the configuration header
    setWindowTitle(tr("Pegasus Editor"));

    // Set the initial window size
    resize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    //setWindowState(windowState() | Qt::WindowMaximized);

    // Create the objects used for the main window user interface
    CreateActions();
    CreateToolBars();
    CreateStatusBar();


    // Create the application manager
    mApplicationManager = new ApplicationManager(this, this);

    //! Finish handling the initialization of the Pegasus engine.
    CreateDockWidgets();
    
    CreateMenu();    

    // Create the settings object, read the setting values from the settings file and apply them
	if (sSettings == nullptr)
    {
        sSettings = new Settings(this);
    }

    // Write the log console header
    ED_LOG("Pegasus Editor settings loaded");

    connect(mApplicationManager, SIGNAL(ApplicationLoaded()),
            this, SLOT(UpdateUIForAppLoaded()));
    connect(mApplicationManager, SIGNAL(ApplicationFinished()),
            this, SLOT(UpdateUIForAppClosed()));

    connect(sSettings, SIGNAL(OnCodeEditorStyleChanged()),
            mCodeEditorWidget, SLOT(OnSettingsChanged())); 

    connect(mActionEditUndo, SIGNAL(triggered()), mHistoryDockWidget, SLOT(TriggerUndo()));
    connect(mActionEditRedo, SIGNAL(triggered()), mHistoryDockWidget, SLOT(TriggerRedo()));

    sSettings->NotifySettingsChanged();

}

//----------------------------------------------------------------------------------------

Editor::~Editor()
{
    //! \todo Handle the closing of the apps that are still open

    if (mAssertionManager != nullptr)
    {
        delete mAssertionManager;
    }
    if (mLogManager != nullptr)
    {
        delete mLogManager;
    }
    if (mTextureEditorViewStrategy != nullptr)
    {
        delete mTextureEditorViewStrategy;
    }
    if (mMeshEditorViewStrategy != nullptr)
    {
        delete mMeshEditorViewStrategy;
    }
    sInstance = nullptr;
}

//----------------------------------------------------------------------------------------

void Editor::RegisterWidget(PegasusDockWidget* widget, Qt::DockWidgetArea area)
{
    widget->Initialize();
    mDockWidgets.push_back(widget);

    addDockWidget(area, widget);

    connect(mApplicationManager, SIGNAL(ApplicationLoaded()),
            widget, SLOT(UpdateUIForAppLoaded()));
    connect(mApplicationManager, SIGNAL(ApplicationFinished()),
            widget, SLOT(UpdateUIForAppClosed()));
    connect(widget, SIGNAL(OnFocus(PegasusDockWidget*)),
            this, SLOT(OnDockFocus(PegasusDockWidget*)));
    connect(widget, SIGNAL(OutFocus(PegasusDockWidget*)),
            this, SLOT(OutDockFocus(PegasusDockWidget*)));
    connect(widget, SIGNAL(OnRegisterDirtyObject(AssetInstanceHandle)),
            this, SLOT(OnRegisterDirtyObject(AssetInstanceHandle)));
    connect(widget, SIGNAL(OnUnregisterDirtyObject(AssetInstanceHandle)),
            this, SLOT(OnUnregisterDirtyObject(AssetInstanceHandle)));

    const Pegasus::PegasusAssetTypeDesc*const* types = widget->GetTargetAssetTypes();
    
    while (types != nullptr && *types != nullptr)
    {
        int guid = (*types)->mTypeGuid;
        QMap<int,PegasusDockWidget*>::iterator it = mTypeGuidWidgetMapping.find(guid);
        if (it != mTypeGuidWidgetMapping.end())
        {
            ED_ASSERTSTR(false, "Two widgets can process the same asset type. There must be only one widget per type. We can do multiple types per widgets though.");
        }   
        else
        {
            mTypeGuidWidgetMapping.insert(guid, widget);
        }
        ++types;
    }
}

//----------------------------------------------------------------------------------------

void Editor::RaiseSplashScreen()
{
	if (mSplashScreen != nullptr)
    {
        mSplashScreen->raise();
    }
}

//----------------------------------------------------------------------------------------

void Editor::CloseSplashScreen()
{
	if (mSplashScreen != nullptr)
    {
        delete mSplashScreen;
        mSplashScreen = nullptr;
    }
}

//----------------------------------------------------------------------------------------


void Editor::SaveCurrentAsset()
{
    for (int i = 0 ; i < mDockWidgets.size(); ++i)
    {
        if (mDockWidgets[i]->HasFocus())
        {
            mDockWidgets[i]->OnSaveFocusedObject();
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

void Editor::DeleteSelectedObjects()
{
    for (int i = 0 ; i < mDockWidgets.size(); ++i)
    {
        if (mDockWidgets[i]->HasFocus())
        {
            mDockWidgets[i]->OnDeleteFocusedObject();
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

void Editor::CreateActions()
{
    mSaveCurrentAsset = new QAction(tr("Save Current Asset"),this);
    mSaveCurrentAsset->setShortcut(tr("Ctrl+S"));
    connect(mSaveCurrentAsset, SIGNAL(triggered()),
            this, SLOT(SaveCurrentAsset()));

    mDeleteSelectedObjects = new QAction(tr("Delete selected objeects"),this);
    mDeleteSelectedObjects->setShortcut(tr("Del"));
    connect(mDeleteSelectedObjects, SIGNAL(triggered()),
            this, SLOT(DeleteSelectedObjects()));

	mActionFileOpenApp = new QAction(tr("&Open App..."), this);
	mActionFileOpenApp->setIcon(QIcon(":/Toolbar/File/OpenApp24.png"));
	mActionFileOpenApp->setShortcut(tr("Ctrl+O"));
	mActionFileOpenApp->setStatusTip(tr("Open an existing app"));
	connect(mActionFileOpenApp, SIGNAL(triggered()), this, SLOT(OpenApp()));

	mActionFileReloadApp = new QAction(tr("&Reload App"), this);
	//! \todo Use the correct icon
    mActionFileReloadApp->setIcon(QIcon(":/Toolbar/File/refresh-512.png"));
	mActionFileReloadApp->setShortcut(tr("Ctrl+R"));
	mActionFileReloadApp->setStatusTip(tr("Reload the current app"));
	connect(mActionFileReloadApp, SIGNAL(triggered()), this, SLOT(ReloadApp()));
    mActionFileReloadApp->setEnabled(false);

    mActionFileCloseApp = new QAction(tr("&Close App"), this);
	//! \todo Use the correct icon
    mActionFileCloseApp->setIcon(QIcon(":/TimelineToolbar/Remove16.png"));
	mActionFileCloseApp->setShortcut(tr("Shift+F4"));
	mActionFileCloseApp->setStatusTip(tr("Close the current app"));
	connect(mActionFileCloseApp, SIGNAL(triggered()), this, SLOT(CloseApp()));
    mActionFileCloseApp->setEnabled(false);

    mActionFileQuit = new QAction(tr("&Quit"), this);
	mActionFileQuit->setShortcut(tr("Alt+F4"));
	mActionFileQuit->setStatusTip(tr("Quit Pegasus Editor"));
	connect(mActionFileQuit, SIGNAL(triggered()), this, SLOT(Quit()));


    mActionEditUndo = new QAction(tr("&Undo"), this);
	mActionEditUndo->setIcon(QIcon(":/Toolbar/Edit/Undo16.png"));
	mActionEditUndo->setShortcut(tr("Ctrl+Z"));
	mActionEditUndo->setStatusTip(tr("Undo the last command"));

    mActionEditRedo = new QAction(tr("&Redo"), this);
	mActionEditRedo->setIcon(QIcon(":/Toolbar/Edit/Redo16.png"));
	mActionEditRedo->setShortcut(tr("Ctrl+Y"));
	mActionEditRedo->setStatusTip(tr("Redo the last command"));

    mActionEditPreferences = new QAction(tr("&Preferences..."), this);
	mActionEditPreferences->setShortcut(tr("Ctrl+K"));
	mActionEditPreferences->setStatusTip(tr("Open the preferences dialog"));
	connect(mActionEditPreferences, SIGNAL(triggered()), this, SLOT(OpenPreferences()));


	mActionViewShowFullscreenViewport = new QAction(tr("Show &Fullscreen Viewport"), this);
	mActionViewShowFullscreenViewport->setCheckable(true);
	mActionViewShowFullscreenViewport->setChecked(false);
	mActionViewShowFullscreenViewport->setIcon(QIcon(":/Toolbar/View/FullScreenViewport24.png"));
	mActionViewShowFullscreenViewport->setStatusTip(tr("Open a fullscreen viewport of the scene"));
	connect(mActionViewShowFullscreenViewport, SIGNAL(triggered()), this, SLOT(ShowFullscreenViewport()));


    mActionCreatePrimitiveBox = new QAction(tr("&Box"), this);
	mActionCreatePrimitiveBox->setStatusTip(tr("Create a box object"));
	connect(mActionCreatePrimitiveBox, SIGNAL(triggered()), this, SLOT(CreateBox()));

    mActionCreatePrimitiveSphere = new QAction(tr("&Sphere"), this);
	mActionCreatePrimitiveSphere->setStatusTip(tr("Create a sphere object"));
	connect(mActionCreatePrimitiveSphere, SIGNAL(triggered()), this, SLOT(CreateSphere()));

    mActionWindowMainViewport = new QAction(tr("&Main Viewport"), this);
	mActionWindowMainViewport->setStatusTip(tr("Open the main viewport window"));
	connect(mActionWindowMainViewport, SIGNAL(triggered()), this, SLOT(OpenMainViewportWindow()));

    mActionWindowSecondaryViewport = new QAction(tr("&Secondary Viewport"), this);
	mActionWindowSecondaryViewport->setStatusTip(tr("Open the secondary viewport window"));
	connect(mActionWindowSecondaryViewport, SIGNAL(triggered()), this, SLOT(OpenSecondaryViewportWindow()));

    mActionWindowTimeline = new QAction(tr("&Timeline"), this);
	mActionWindowTimeline->setStatusTip(tr("Open the timeline window"));
	connect(mActionWindowTimeline, SIGNAL(triggered()), this, SLOT(OpenTimelineWindow()));

    mActionWindowHistory = new QAction(tr("&History"), this);
	mActionWindowHistory->setStatusTip(tr("Open the history window"));
	connect(mActionWindowHistory, SIGNAL(triggered()), this, SLOT(OpenHistoryWindow()));

    mActionWindowConsole = new QAction(tr("&Console"), this);
	mActionWindowConsole->setStatusTip(tr("Open the console window"));
	connect(mActionWindowConsole, SIGNAL(triggered()), this, SLOT(OpenConsoleWindow()));

    mActionWindowCodeEditor = new QAction(tr("&Code Editor"), this);
	mActionWindowCodeEditor->setStatusTip(tr("Open the code editor"));
	connect(mActionWindowCodeEditor, SIGNAL(triggered()), this, SLOT(OpenCodeEditorWindow()));

    mActionWindowAssetLibrary = new QAction(tr("&Asset Library"), this);
	mActionWindowAssetLibrary->setStatusTip(tr("Open the asset library"));
	connect(mActionWindowAssetLibrary, SIGNAL(triggered()), this, SLOT(OpenAssetLibraryWindow()));

    mActionWindowTextureEditor = new QAction(tr("Te&xture Editor"), this);
	mActionWindowTextureEditor->setStatusTip(tr("Open the texture editor window"));
	connect(mActionWindowTextureEditor, SIGNAL(triggered()), this, SLOT(OpenTextureEditorWindow()));

    mActionWindowMeshEditor = new QAction(tr("&Mesh Editor"), this);
    mActionWindowMeshEditor->setStatusTip(tr("Open the mesh editor window"));
    connect(mActionWindowMeshEditor, SIGNAL(triggered()), this, SLOT(OpenMeshEditorWindow()));

    mActionProgramEditor = new QAction(tr("&Program Editor"), this);
    mActionProgramEditor->setStatusTip(tr("Open the program editor window"));
    connect(mActionProgramEditor, SIGNAL(triggered()), this, SLOT(OpenProgramEditorWindow()));

    mActionWindowDebugPropertyGridClasses = new QAction(tr("&Property Grid Classes"), this);
	mActionWindowDebugPropertyGridClasses->setStatusTip(tr("Open the list of classes with their lists of properties"));
	connect(mActionWindowDebugPropertyGridClasses, SIGNAL(triggered()), this, SLOT(OpenPropertyGridClassesWindow()));

    mActionWindowDebugBsLibWidget = new QAction(tr("BlockScript &Libraries View"), this);
	mActionWindowDebugBsLibWidget->setStatusTip(tr("Open tree view of blockscript libraries in the runtime"));
	connect(mActionWindowDebugBsLibWidget, SIGNAL(triggered()), this, SLOT(OpenBsLibWidget()));

    mActionHelpIndex = new QAction(tr("&Index..."), this);
	mActionHelpIndex->setShortcut(tr("F1"));
	mActionHelpIndex->setStatusTip(tr("Open the help for Pegasus Editor"));
	connect(mActionHelpIndex, SIGNAL(triggered()), this, SLOT(HelpIndex()));

	mActionHelpAboutQt = new QAction(tr("&About Qt..."), this);
	mActionHelpAboutQt->setStatusTip(tr("Open the Qt about dialog box"));
	connect(mActionHelpAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	mActionHelpAbout = new QAction(tr("&About..."), this);
	mActionHelpAbout->setStatusTip(tr("Open the about dialog box"));
	connect(mActionHelpAbout, SIGNAL(triggered()), this, SLOT(About()));
}

//----------------------------------------------------------------------------------------


void Editor::OnRegisterDirtyObject(AssetInstanceHandle handle)
{
    mDirtyAssets.insert(handle);
}

//----------------------------------------------------------------------------------------

void Editor::OnUnregisterDirtyObject(AssetInstanceHandle handle)
{
    mDirtyAssets.remove(handle);
}

//----------------------------------------------------------------------------------------

void Editor::CreateMenu()
{
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(mActionFileOpenApp);
    fileMenu->addAction(mActionFileReloadApp);
    fileMenu->addAction(mActionFileCloseApp);
    fileMenu->addAction(mSaveCurrentAsset);
    fileMenu->addSeparator();
    fileMenu->addAction(mActionFileQuit);

    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(mActionEditUndo);
    editMenu->addAction(mActionEditRedo);
    fileMenu->addAction(mDeleteSelectedObjects);
    editMenu->addSeparator();
    editMenu->addAction(mActionEditPreferences);

    QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(mActionViewShowFullscreenViewport);
    
    QMenu * windowMenu = menuBar()->addMenu(tr("&Window"));

    windowMenu->addAction(mActionWindowMainViewport);
    windowMenu->addAction(mActionWindowSecondaryViewport);
    windowMenu->addAction(mActionWindowTimeline);
    windowMenu->addAction(mActionWindowHistory);
    windowMenu->addAction(mActionWindowConsole);
    windowMenu->addAction(mActionWindowCodeEditor);
    windowMenu->addAction(mActionWindowAssetLibrary);
    windowMenu->addAction(mActionWindowTextureEditor);
    windowMenu->addAction(mActionWindowMeshEditor);
    windowMenu->addAction(mActionProgramEditor);

    QMenu * debugMenu = windowMenu->addMenu(tr("&Debug"));
    debugMenu->addAction(mActionWindowDebugPropertyGridClasses);
    debugMenu->addAction(mActionWindowDebugBsLibWidget);

    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(mActionHelpIndex);
    helpMenu->addSeparator();
    helpMenu->addAction(mActionHelpAboutQt);
    helpMenu->addAction(mActionHelpAbout);

}

//----------------------------------------------------------------------------------------

void Editor::CreateToolBars()
{
    QToolBar * fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("FileToolBar");
    fileToolBar->setIconSize(QSize(16, 16));
	fileToolBar->setAllowedAreas(Qt::TopToolBarArea);
	fileToolBar->addAction(mActionFileOpenApp);
	fileToolBar->addAction(mActionFileReloadApp);
    fileToolBar->addAction(mActionFileCloseApp);

	QToolBar * editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("EditToolBar");
	editToolBar->setIconSize(QSize(16, 16));
	editToolBar->setAllowedAreas(Qt::TopToolBarArea);
	editToolBar->addAction(mActionEditUndo);
	editToolBar->addAction(mActionEditRedo);

    QToolBar * viewToolBar = addToolBar(tr("View"));
    viewToolBar->setObjectName("ViewToolBar");
	viewToolBar->setIconSize(QSize(16, 16));
	viewToolBar->setAllowedAreas(Qt::TopToolBarArea);
	viewToolBar->addAction(mActionViewShowFullscreenViewport);

}

//----------------------------------------------------------------------------------------

void Editor::CreateDockWidgets()
{
    // Set the dock widget areas to assign to the four main window corners
    setCorner(Qt::TopLeftCorner    , Qt::TopDockWidgetArea );
    setCorner(Qt::TopRightCorner   , Qt::TopDockWidgetArea);
    setCorner(Qt::BottomLeftCorner , Qt::BottomDockWidgetArea );
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    // Allow nesting of dock widgets within dock areas
    setDockNestingEnabled(true);

    //Generic widget initialization
    mTimelineDockWidget = new TimelineDockWidget(this, this);
    RegisterWidget(mTimelineDockWidget, Qt::BottomDockWidgetArea);

    mProgramEditorWidget = new ProgramEditorWidget(this,this); 
    RegisterWidget(mProgramEditorWidget,Qt::BottomDockWidgetArea);
    mProgramEditorWidget->hide();
    mProgramEditorWidget->setFloating(true);

    mTextureEditorViewStrategy = new TextureGraphEditorViewStrategy();
    mMeshEditorViewStrategy = new MeshGraphEditorViewStrategy();


    // Create the dock widgets and assign their initial position
    //! \todo Use the correct icons for the docks, and add them to the menu and toolbar

    mMainViewportDockWidget = new ViewportDockWidget(this,this,"Viewport 1", "MainViewportDockWidget");
    RegisterWidget(mMainViewportDockWidget,Qt::TopDockWidgetArea);
    mViewportWidgets.push_back(mMainViewportDockWidget->GetViewportWidget());
    mViewportDockWidgets.push_back(mMainViewportDockWidget); 

    mSecondaryViewportDockWidget = new ViewportDockWidget(this,this,"Viewport 2", "SecondaryViewportDockWidget");
    RegisterWidget(mSecondaryViewportDockWidget,Qt::TopDockWidgetArea);
    mViewportWidgets.push_back(mSecondaryViewportDockWidget->GetViewportWidget());
    mViewportDockWidgets.push_back(mSecondaryViewportDockWidget);

    mHistoryDockWidget = new HistoryDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, mHistoryDockWidget);

    mConsoleDockWidget = new ConsoleDockWidget(this);
    addDockWidget(Qt::BottomDockWidgetArea, mConsoleDockWidget);

    mCodeEditorWidget = new CodeEditorWidget(this, this);
    RegisterWidget(mCodeEditorWidget, Qt::BottomDockWidgetArea);
    mCodeEditorWidget->hide();
    mCodeEditorWidget->setFloating(true);


    mAssetLibraryWidget = new AssetLibraryWidget(this, this);
    RegisterWidget(mAssetLibraryWidget, Qt::RightDockWidgetArea);
    mAssetLibraryWidget->hide();
    mAssetLibraryWidget->setFloating(true);

    mTextureEditorDockWidget = new GraphEditorDockWidget(this, this, mTextureEditorViewStrategy);
    RegisterWidget(mTextureEditorDockWidget, Qt::RightDockWidgetArea);
    mViewportWidgets.push_back(mTextureEditorDockWidget->GetViewportWidget());

    mMeshEditorDockWidget = new GraphEditorDockWidget(this, this, mMeshEditorViewStrategy);
    RegisterWidget(mMeshEditorDockWidget, Qt::RightDockWidgetArea);
    mViewportWidgets.push_back(mMeshEditorDockWidget->GetViewportWidget());

    mPropertyGridClassesDockWidget = new PropertyGridClassesDockWidget(this, this);
    RegisterWidget(mPropertyGridClassesDockWidget, Qt::RightDockWidgetArea);

    mBsLibWidget = new BlockScriptLibraryDockWidget(this, this);
    RegisterWidget(mBsLibWidget, Qt::RightDockWidgetArea);
    mBsLibWidget->hide();
    mBsLibWidget->setFloating(true);


    //from ui to ui
    connect(mAssetLibraryWidget, SIGNAL(OnHighlightBlock(unsigned)),
            mTimelineDockWidget, SLOT(OnFocusBlock(unsigned)));

}

//----------------------------------------------------------------------------------------

void Editor::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

//----------------------------------------------------------------------------------------

void Editor::InternalCloseAndPumpEvents()
{
   mApplicationManager->CloseApplication(false);
   while (!mApplicationManager->PollApplicationThreadIsDone())
   {
       //let the operating system process any extra events internally, such as detection of destruction of child windows
       mQtApplication->processEvents();
   }
   mApplicationManager->OnApplicationFinished();
}

//----------------------------------------------------------------------------------------

void Editor::closeEvent(QCloseEvent * event)
{
     // Close the current application if one is open
    if (mApplicationManager != nullptr)
    {
        if (mApplicationManager->IsApplicationOpened())
        {
            InternalCloseAndPumpEvents();
        }
    }
 
	//	// Save the application settings and destroy the settings object
    if (sSettings != nullptr)
    {
        sSettings->Save();
    }
    delete sSettings;
    sSettings = nullptr;
}

//----------------------------------------------------------------------------------------

void Editor::OpenApp()
{
    //! \todo If a scene is open, handle its saving and closing (maybe use closeEvent?)

	// Open the file dialog box
	//! \todo Manage the current directory
#if PEGASUS_PLATFORM_WINDOWS
    QString filter("Pegasus application (*.dll)");
#else
#error "Unhandled platform for the Pegasus editor"
#endif
    //! \todo Use a directory that comes from the preferences
    QString fileName = QFileDialog::getOpenFileName(this, "Load application",
													QString("../../../Bin/VS14/Win32/Dev"), filter);

	// Import the file to the current scene
	if (!fileName.isEmpty())
    {
        //if an application is already opened, close it first
        if (mApplicationManager->IsApplicationOpened())
        {
            InternalCloseAndPumpEvents();
        }

        mApplicationManager->OpenApplication(fileName);
        //! \todo Handle errors
    }
}

//----------------------------------------------------------------------------------------

void Editor::ReloadApp()
{
    ED_ASSERTSTR(mApplicationManager->IsApplicationOpened(), "An application must be opened to be able to reload it");

    // Get the filename of the current app
    const QString appFileName = mApplicationManager->GetApplication()->GetFileName();

    // Close the current app
    InternalCloseAndPumpEvents();

    // Open the app again with the cached file name
    mApplicationManager->OpenApplication(appFileName);
}

//----------------------------------------------------------------------------------------

void Editor::CloseApp()
{
    bool performClosing = true;
    if (mDirtyAssets.size() > 0)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
                                       this, "Unsaved changes in assets.",
                                      "There are unsaved changes in opened assets. Are you sure you want to exit?",
                                       QMessageBox::Yes|QMessageBox::No);
        performClosing = reply == QMessageBox::Yes;
    }

    if (performClosing)
    {
        if (mApplicationManager->IsApplicationOpened())
        {
            mApplicationManager->CloseApplication();
        }
    }
}

//----------------------------------------------------------------------------------------

void Editor::Quit()
{
    //! \todo Properly quit the editor
}

//----------------------------------------------------------------------------------------

void Editor::OpenPreferences()
{
	SettingsDialog * settingsDialog = nullptr;
    settingsDialog = new SettingsDialog(this);
	settingsDialog->show();
}

//----------------------------------------------------------------------------------------

void Editor::ShowFullscreenViewport()
{
    //! /todo Show the viewport window in fullscreen
}

//----------------------------------------------------------------------------------------

void Editor::CreateBox()
{
    //! /todo Create a box primitive
}

//----------------------------------------------------------------------------------------

void Editor::CreateSphere()
{
    //! /todo Create a sphere primitive
}

//----------------------------------------------------------------------------------------

void Editor::OpenMainViewportWindow()
{
    mMainViewportDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenSecondaryViewportWindow()
{
    mSecondaryViewportDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenTimelineWindow()
{
    mTimelineDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenHistoryWindow()
{
    mHistoryDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenConsoleWindow()
{
    mConsoleDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenCodeEditorWindow()
{
    mCodeEditorWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenAssetLibraryWindow()
{
    mAssetLibraryWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenTextureEditorWindow()
{
    mTextureEditorDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenMeshEditorWindow()
{
    mMeshEditorDockWidget->show();
}


//----------------------------------------------------------------------------------------

void Editor::OpenPropertyGridClassesWindow()
{
    mPropertyGridClassesDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenBsLibWidget()
{
    mBsLibWidget->show();
}
    
//----------------------------------------------------------------------------------------

void Editor::OpenProgramEditorWindow()
{
    mProgramEditorWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::HelpIndex()
{
    //! /todo Show the help
}

//----------------------------------------------------------------------------------------

void Editor::About()
{
    //! /todo Show the about dialog box
}

//----------------------------------------------------------------------------------------

void Editor::UpdateUIForAppLoaded()
{
    mActionFileReloadApp->setEnabled(true);
    mActionFileCloseApp->setEnabled(true);
}

//----------------------------------------------------------------------------------------

void Editor::UpdateUIForAppClosed()
{
    mActionFileReloadApp->setEnabled(false);
    mActionFileCloseApp->setEnabled(false);
}

//----------------------------------------------------------------------------------------

void Editor::OpenPreferencesAppearance()
{
	SettingsDialog * settingsDialog = nullptr;
    settingsDialog = new SettingsDialog(this);
    settingsDialog->SetCurrentPage(SettingsDialog::PAGE_APPEARANCE);
	settingsDialog->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenPreferencesConsole()
{
	SettingsDialog * settingsDialog = nullptr;
    settingsDialog = new SettingsDialog(this);
    settingsDialog->SetCurrentPage(SettingsDialog::PAGE_CONSOLE);
	settingsDialog->show();
}

//----------------------------------------------------------------------------------------

void Editor::OnDockFocus(PegasusDockWidget* target)
{
    if (target->GetCurrentUndoStack() != nullptr)
    {
        mHistoryDockWidget->SetUndoStack(target->GetCurrentUndoStack());
    }
}

//----------------------------------------------------------------------------------------

void Editor::OnOpenObject(AssetInstanceHandle handle, QString displayName, int typeGuid, QVariant initData)
{
    QMap<int,PegasusDockWidget*>::iterator it = mTypeGuidWidgetMapping.find(typeGuid);
    if (it != mTypeGuidWidgetMapping.end())
    {
        (*it)->ReceiveOpenRequest(handle, displayName, initData);
    }
    else
    {
        ED_LOG("No editor found for asset.");
    }
}

//----------------------------------------------------------------------------------------

void Editor::OutDockFocus(PegasusDockWidget* target)
{

}

