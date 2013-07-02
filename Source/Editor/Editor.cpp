#include "Editor.h"
#include "ApplicationManager.h"
#include "Viewport/ViewportDockWidget.h"
#include "Timeline/TimelineDockWidget.h"

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


Editor::Editor(QWidget *parent)
:   QMainWindow(parent),
    mApplicationManager(nullptr)
{
    // Create the splash screen (it becomes visible once this class is initialized,
    // set by the application class)
    //! \todo Finish implementing the splash screen
    //mSplashScreen = new QSplashScreen(Qpixmap(EDITOR_SPLASH_IMAGE_FILE));
    //mSplashScreen->show();

    // Create the options object, read the option values from the operating system
	// and apply them
    //! \todo To implement
	//mOptions = new EditorOptions;

    // Set the title of the main window
    //! \todo Use a constant in the configuration header
    setWindowTitle(tr("Pegasus Editor"));

    // Set the initial window size
    //! \todo Read from the options
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

    // Create the application manager
    ApplicationManager::CreateInstance();
}

//----------------------------------------------------------------------------------------

Editor::~Editor()
{
    // Destroy the application manager
    //! \todo Handle the closing of the apps that are still open
    ApplicationManager::DestroyInstance();
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
    //! \todo Add support for icons

    mActionFileNewScene = new QAction(tr("&New Scene"), this);
	//mActionFileNewScene->setIcon(QIcon(":/Toolbar/File/NewScene24.png"));
	mActionFileNewScene->setShortcut(tr("Ctrl+N"));
	mActionFileNewScene->setStatusTip(tr("Create a new empty scene"));
    connect(mActionFileNewScene, SIGNAL(triggered()), this, SLOT(NewScene()));

	mActionFileOpenApp = new QAction(tr("&Open App..."), this);
	//mActionFileOpenApp->setIcon(QIcon(":/Toolbar/File/OpenApp24.png"));
	mActionFileOpenApp->setShortcut(tr("Ctrl+O"));
	mActionFileOpenApp->setStatusTip(tr("Open an existing app"));
	connect(mActionFileOpenApp, SIGNAL(triggered()), this, SLOT(OpenApp()));

	mActionFileCloseApp = new QAction(tr("&Close App"), this);
	//mActionFileOpenApp->setIcon(QIcon(":/Toolbar/File/CloseApp24.png"));
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
	//mActionViewShowFullscreenViewport->setIcon(QIcon(":/Toolbar/Viewport/FullScreenViewport24.png"));
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
    fileToolBar->setIconSize(QSize(16, 16));
	fileToolBar->setAllowedAreas(Qt::TopToolBarArea);
	fileToolBar->addAction(mActionFileNewScene);
	fileToolBar->addAction(mActionFileOpenApp);
    fileToolBar->addAction(mActionFileCloseApp);

	QToolBar * viewToolBar = addToolBar(tr("View"));
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
    //! \todo Probably needs to be inverted, as we will want the timeline to use the full width
    setCorner(Qt::TopLeftCorner    , Qt::LeftDockWidgetArea );
    setCorner(Qt::TopRightCorner   , Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner , Qt::LeftDockWidgetArea );
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Create the dock widgets and assign their initial position

    mViewportDockWidget = new ViewportDockWidget(this);
    //mViewportDockWidget->setWindowIcon(QIcon(QPixmap(":/res/qt.png")));
    addDockWidget(Qt::TopDockWidgetArea, mViewportDockWidget);

    mTimelineDockWidget = new TimelineDockWidget(this);
    //mTimelineDockWidget->setWindowIcon(QIcon(QPixmap(":/res/qt.png")));
    addDockWidget(Qt::BottomDockWidgetArea, mTimelineDockWidget);

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

    //! \todo Temporary
    event->accept();
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
    QString fileName = QFileDialog::getOpenFileName(this, "Load application",
													 QString(/***/"."), filter);

	// Import the file to the current scene
	if (!fileName.isEmpty())
    {
        //! \todo Load the application
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
    //! /todo Open the preference dialog    
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

void Editor::HelpIndex()
{
    //! /todo Show the help
}

//----------------------------------------------------------------------------------------

void Editor::About()
{
    //! /todo Show the about dialog box
}
