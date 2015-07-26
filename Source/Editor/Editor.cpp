/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Editor.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Main window of Pegasus Editor

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Application/ApplicationManager.h"
#include "Settings/SettingsDialog.h"

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

    connect(mApplicationManager, SIGNAL(ApplicationLoaded()),
            mTextureEditorDockWidget, SLOT(UpdateUIForAppLoaded()));
    connect(mApplicationManager, SIGNAL(ApplicationFinished()),
            mTextureEditorDockWidget, SLOT(UpdateUIForAppClosed()));

    connect(mApplicationManager, SIGNAL(ApplicationLoaded()),
            mPropertyGridClassesDockWidget, SLOT(UpdateUIForAppLoaded()));
    connect(mApplicationManager, SIGNAL(ApplicationFinished()),
            mPropertyGridClassesDockWidget, SLOT(UpdateUIForAppClosed()));

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
    sInstance = nullptr;
}

//----------------------------------------------------------------------------------------

void Editor::RegisterWidget(PegasusDockWidget* widget, Qt::DockWidgetArea area)
{
    widget->Initialize();
    mWidgets.push_back(widget);

    addDockWidget(area, widget);

    connect(mApplicationManager, SIGNAL(ApplicationLoaded()),
            widget, SLOT(UpdateUIForAppLoaded()));
    connect(mApplicationManager, SIGNAL(ApplicationFinished()),
            widget, SLOT(UpdateUIForAppClosed()));
    connect(widget, SIGNAL(OnFocus(PegasusDockWidget*)),
            this, SLOT(OnDockFocus(PegasusDockWidget*)));
    connect(widget, SIGNAL(OutFocus(PegasusDockWidget*)),
            this, SLOT(OutDockFocus(PegasusDockWidget*)));
    connect(widget, SIGNAL(OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
            this, SLOT(OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)));
    connect(widget, SIGNAL(OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)),
            this, SLOT(OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy*)));
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
    for (int i = 0 ; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i]->HasFocus())
        {
            mWidgets[i]->OnSaveFocusedObject();
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

ViewportWidget * Editor::GetViewportWidget(ViewportType viewportType) const
{
    switch (viewportType)
    {
        case VIEWPORTTYPE_MAIN:
            return mMainViewportDockWidget->GetViewportWidget();

        case VIEWPORTTYPE_SECONDARY:
            return mSecondaryViewportDockWidget->GetViewportWidget();

        case VIEWPORTTYPE_TEXTURE_EDITOR_PREVIEW:
            return mTextureEditorDockWidget->GetViewportWidget();

        case VIEWPORTTYPE_MESH_EDITOR_PREVIEW:
            //! \todo Add support for mesh editor
            ED_FAILSTR("The mesh editor preview viewport is not implemented yet");
            return nullptr;

        default:
            ED_FAILSTR("Invalid viewport widget type (%d), it should be < %d", viewportType, NUM_VIEWPORT_TYPES);
            return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void Editor::CreateActions()
{
    mSaveCurrentAsset = new QAction(tr("Save Current Asset"),this);
    mSaveCurrentAsset->setShortcut(tr("Ctrl+S"));
    connect(mSaveCurrentAsset, SIGNAL(triggered()),
            this, SLOT(SaveCurrentAsset()));

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


    mActionWindowDebugPropertyGridClasses = new QAction(tr("&Property Grid Classes"), this);
	mActionWindowTextureEditor->setStatusTip(tr("Open the list of classes with their lists of properties"));
	connect(mActionWindowDebugPropertyGridClasses, SIGNAL(triggered()), this, SLOT(OpenPropertyGridClassesWindow()));
    mActionProgramEditor = new QAction(tr("&Program Editor"), this);
    mActionProgramEditor->setStatusTip(tr("Open the program editor window"));
    connect(mActionProgramEditor, SIGNAL(triggered()), this, SLOT(OpenProgramEditorWindow()));


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


void Editor::OnRegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    mDirtyAssets.insert(object);
}

//----------------------------------------------------------------------------------------

void Editor::OnUnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    mDirtyAssets.remove(object);
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
    editMenu->addSeparator();
    editMenu->addAction(mActionEditPreferences);

    QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(mActionViewShowFullscreenViewport);

    menuBar()->addMenu(mAssetLibraryWidget->CreateNewAssetMenu(tr("&Create"), this));

    QMenu * windowMenu = menuBar()->addMenu(tr("&Window"));

    windowMenu->addAction(mActionWindowMainViewport);
    windowMenu->addAction(mActionWindowSecondaryViewport);
    windowMenu->addAction(mActionWindowTimeline);
    windowMenu->addAction(mActionWindowHistory);
    windowMenu->addAction(mActionWindowConsole);
    windowMenu->addAction(mActionWindowCodeEditor);
    windowMenu->addAction(mActionWindowAssetLibrary);
    windowMenu->addAction(mActionWindowTextureEditor);
    windowMenu->addAction(mActionProgramEditor);

    QMenu * debugMenu = windowMenu->addMenu(tr("&Debug"));
    debugMenu->addAction(mActionWindowDebugPropertyGridClasses);

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


    // Create the dock widgets and assign their initial position
    //! \todo Use the correct icons for the docks, and add them to the menu and toolbar

    mMainViewportDockWidget = new ViewportDockWidget(VIEWPORTTYPE_MAIN, this);
    addDockWidget(Qt::TopDockWidgetArea, mMainViewportDockWidget);

    mSecondaryViewportDockWidget = new ViewportDockWidget(VIEWPORTTYPE_SECONDARY, this);
    addDockWidget(Qt::TopDockWidgetArea, mSecondaryViewportDockWidget);

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

    mTextureEditorDockWidget = new TextureEditorDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, mTextureEditorDockWidget);

    mPropertyGridClassesDockWidget = new PropertyGridClassesDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, mPropertyGridClassesDockWidget);
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
													QString("../../../Bin/VS11/Win32/Dev"), filter);

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

void Editor::OpenPropertyGridClassesWindow()
{
    mPropertyGridClassesDockWidget->show();
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

void Editor::OutDockFocus(PegasusDockWidget* target)
{

}

