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
#include "ApplicationManager.h"
#include "Settings/SettingsDialog.h"
#include "Viewport/ViewportDockWidget.h"
#include "Timeline/TimelineDockWidget.h"
#include "Console/ConsoleDockWidget.h"

#include "Pegasus/Pegasus.h"

#include <QSplashScreen>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QApplication>
#include <QFileDialog>


Editor * Editor::sInstance = nullptr;
Settings * Editor::sSettings = nullptr;

//----------------------------------------------------------------------------------------

Editor::Editor(QWidget *parent)
:   QMainWindow(parent),
    mApplicationManager(nullptr)
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
    CreateMenu();
    CreateToolBars();
    CreateStatusBar();

    //! \todo Finish handling the initialization of the Pegasus engine.
    //!       Remove the dock widgets initialization from here and move it
    //!       to the callback function called once the engine has initialized.
    CreateDockWidgets();

    // Create the settings object, read the setting values from the settings file and apply them
	if (sSettings == nullptr)
    {
        sSettings = new Settings(this);
    }

    // Write the log console header
    ED_LOG("Pegasus Editor settings loaded");

    // Test messages
    /****/
    //! \todo Remove
	for (int i = 0; i < 100; i++)
    {
        if ((i / 10) & 1)
        {
            if (i == 12) mConsoleDockWidget->AddMessage('CRIT', QString("Hello, world %1!").arg(i));
            else if (i == 13) mConsoleDockWidget->AddMessage('ERR_', QString("Hello, world %1!").arg(i));
            else if (i == 14) mConsoleDockWidget->AddMessage('ASRT', QString("Hello, world %1!").arg(i));
            else if (i == 15) mConsoleDockWidget->AddMessage('WNDW', QString("Hello, world %1!").arg(i));
            else mConsoleDockWidget->AddMessage('WARN', QString("Hello, world %1!").arg(i));
        }
        else
        {
            mConsoleDockWidget->AddMessage(QString("Hello, world %1!").arg(i));
        }
    }

    // Create the application manager
    mApplicationManager = new ApplicationManager(this, mViewportDockWidget, this);
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

void Editor::CreateActions()
{
    mActionFileNewScene = new QAction(tr("&New Scene"), this);
	mActionFileNewScene->setIcon(QIcon(":/Toolbar/File/NewScene24.png"));
	mActionFileNewScene->setShortcut(tr("Ctrl+N"));
	mActionFileNewScene->setStatusTip(tr("Create a new empty scene"));
    connect(mActionFileNewScene, SIGNAL(triggered()), this, SLOT(NewScene()));

	mActionFileOpenApp = new QAction(tr("&Open App..."), this);
	mActionFileOpenApp->setIcon(QIcon(":/Toolbar/File/OpenApp24.png"));
	mActionFileOpenApp->setShortcut(tr("Ctrl+O"));
	mActionFileOpenApp->setStatusTip(tr("Open an existing app"));
	connect(mActionFileOpenApp, SIGNAL(triggered()), this, SLOT(OpenApp()));

	mActionFileCloseApp = new QAction(tr("&Close App"), this);
	//! \todo Use the correct icon
    mActionFileCloseApp->setIcon(QIcon(":/Toolbar/File/SaveScene24.png"));
	mActionFileCloseApp->setShortcut(tr("Shift+F4"));
	mActionFileCloseApp->setStatusTip(tr("Close the current app"));
	connect(mActionFileCloseApp, SIGNAL(triggered()), this, SLOT(CloseApp()));

    mActionFileQuit = new QAction(tr("&Quit"), this);
	mActionFileQuit->setShortcut(tr("Alt+F4"));
	mActionFileQuit->setStatusTip(tr("Quit Pegasus Editor"));
	connect(mActionFileQuit, SIGNAL(triggered()), this, SLOT(Quit()));


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


    mActionWindowViewport = new QAction(tr("&Viewport"), this);
	mActionWindowViewport->setStatusTip(tr("Open the viewport window"));
	connect(mActionWindowViewport, SIGNAL(triggered()), this, SLOT(OpenViewportWindow()));

    mActionWindowTimeline = new QAction(tr("&Timeline"), this);
	mActionWindowTimeline->setStatusTip(tr("Open the timeline window"));
	connect(mActionWindowTimeline, SIGNAL(triggered()), this, SLOT(OpenTimelineWindow()));

    mActionWindowConsole = new QAction(tr("&Console"), this);
	mActionWindowConsole->setStatusTip(tr("Open the console window"));
	connect(mActionWindowConsole, SIGNAL(triggered()), this, SLOT(OpenConsoleWindow()));


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

void Editor::CreateMenu()
{
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(mActionFileNewScene);
    fileMenu->addAction(mActionFileOpenApp);
    fileMenu->addAction(mActionFileCloseApp);
    fileMenu->addSeparator();
    fileMenu->addAction(mActionFileQuit);

    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(mActionEditPreferences);

    QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(mActionViewShowFullscreenViewport);

    QMenu * createMenu = menuBar()->addMenu(tr("&Create"));
    QMenu * createPrimitiveMenu = createMenu->addMenu(tr("&Primitive"));
    createPrimitiveMenu->addAction(mActionCreatePrimitiveBox);
    createPrimitiveMenu->addAction(mActionCreatePrimitiveSphere);

    QMenu * windowMenu = menuBar()->addMenu(tr("&Window"));
	//mToolbarMenu = windowMenu->addMenu(tr("&Toolbars"));
	//mDockMenu = windowMenu->addMenu(tr("&Windows"));
    //! \todo Temporary. Use the toolbar and dock menus with checkboxes
    windowMenu->addAction(mActionWindowViewport);
    windowMenu->addAction(mActionWindowTimeline);
    windowMenu->addAction(mActionWindowConsole);

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
	fileToolBar->addAction(mActionFileNewScene);
	fileToolBar->addAction(mActionFileOpenApp);
    fileToolBar->addAction(mActionFileCloseApp);

	QToolBar * viewToolBar = addToolBar(tr("View"));
    viewToolBar->setObjectName("ViewToolBar");
	viewToolBar->setIconSize(QSize(16, 16));
	viewToolBar->setAllowedAreas(Qt::TopToolBarArea);
	viewToolBar->addAction(mActionViewShowFullscreenViewport);

	// Create and place the view actions for the toolbars to the toolbar menu
	// (in alphabetical order)
	//toolbarMenu->addAction(_editToolBar->toggleViewAction());
	//toolbarMenu->addAction(_fileToolBar->toggleViewAction());
	//toolbarMenu->addAction(_viewToolBar->toggleViewAction());
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

    // Create the dock widgets and assign their initial position
    //! \todo Use the correct icons for the docks, and add them to the menu and toolbar

    mViewportDockWidget = new ViewportDockWidget(this);
    //mViewportDockWidget->setWindowIcon(QIcon(QPixmap(":/res/qt.png")));
    addDockWidget(Qt::TopDockWidgetArea, mViewportDockWidget);

    mTimelineDockWidget = new TimelineDockWidget(this);
    //mTimelineDockWidget->setWindowIcon(QIcon(QPixmap(":/res/qt.png")));
    addDockWidget(Qt::BottomDockWidgetArea, mTimelineDockWidget);

    mConsoleDockWidget = new ConsoleDockWidget(this);
    //mConsoleDockWidget->setWindowIcon(QIcon(QPixmap(":/res/qt.png")));
    addDockWidget(Qt::BottomDockWidgetArea, mConsoleDockWidget);

    // Create and place the view actions for the dock widgets to the dock menu
    // (in alphabetical order)
    //! \todo To implement from N3D editor
}

//----------------------------------------------------------------------------------------

void Editor::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

//----------------------------------------------------------------------------------------

void Editor::closeEvent(QCloseEvent * event)
{
    //! \todo Handle the thread management in the viewport
    //! \todo Handle the saving of unsaved files
    //! \todo Add log messages for the different close events

	// If an application thread is not created or has been destroyed
	//if (!engineThread)
	//{
	//	_event->accept();
	//	return;
	//}
	
	// If an application is running, quit it but do not close the main window
	//if (engineThread->isRunning())
	//{
	//	/***********/hide();
	//	DestroyN3DDockWidgets();
	//	N3D_DELETE(n3DInterface);

	//	N3D_Engine::Quit();
	//	event->ignore();
	//}

	// Once the application has quit, the thread object can be destroyed and
	// the window can be closed
	//else
	//{
	//	//N3D_DELETE(n3DInterface);
	//	N3D_DELETE(engineThread);
	//	N3D_DELETE(viewportsSceneGraph);

    // Close the current application if one is open
    if (mApplicationManager != nullptr)
    {
        if (mApplicationManager->IsApplicationOpened())
        {
            mApplicationManager->CloseApplication();
        }
    }
 
	//	// Save the application settings and destroy the settings object
    if (sSettings != nullptr)
    {
        sSettings->Save();
    }
    delete sSettings;
    sSettings = nullptr;

	//	event->accept();
	//}
}

//----------------------------------------------------------------------------------------

void Editor::NewScene()
{
    //! \todo Create a new scene
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
        mApplicationManager->OpenApplication(fileName);
        //! \todo Handle errors
    }
}

//----------------------------------------------------------------------------------------

void Editor::CloseApp()
{
    //! \todo If a scene is open, handle its saving and closing (maybe use closeEvent?)
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

void Editor::OpenViewportWindow()
{
    mViewportDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenTimelineWindow()
{
    mTimelineDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenConsoleWindow()
{
    mConsoleDockWidget->show();
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

void Editor::OpenPreferencesAppearance()
{
	SettingsDialog * settingsDialog = nullptr;
    settingsDialog = new SettingsDialog(this);
    settingsDialog->SetCurrentPage(SettingsDialog::PAGE_APPEARANCE);
	settingsDialog->show();
}
