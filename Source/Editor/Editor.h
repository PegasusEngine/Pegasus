/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Editor.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Main window of Pegasus Editor

#ifndef EDITOR_H
#define EDITOR_H

#include "Settings/Settings.h"

#include "Console/ConsoleDockWidget.h"
#include "Timeline/TimelineDockWidget.h"
#include "Viewport/ViewportDockWidget.h"

#include <QtWidgets/QMainWindow>

class ApplicationManager;
class AssertionManager;

class QSplashScreen;
class QAction;


//! Main window of Pegasus Editor
class Editor : public QMainWindow
{
    Q_OBJECT

public:

    Editor(QWidget *parent = 0);
    ~Editor();

    //! Return the unique instance of the editor
    //! \return Reference to the unique instance of the editor
    inline static Editor & GetInstance() { return *sInstance; }

    //! Get the assertion manager, used by all assertion macros
    inline AssertionManager & GetAssertionManager() const { return *mAssertionManager; }

    //------------------------------------------------------------------------------------

public:

    //! Raise the splash screen (put the window on top)
    void RaiseSplashScreen();

    //! Close the splash screen (if opened)
    void CloseSplashScreen();

    //! Get the editor settings object
    //! \return Pointer to the editor settings object
    inline static Settings * GetSettings() { return sSettings; }

	//------------------------------------------------------------------------------------

public:

    //! Get the viewport dock widget
    //! \return Pointer to the viewport dock widget
    inline ViewportDockWidget * GetViewportDockWidget() const { return mViewportDockWidget; }

    //! Get the timeline dock widget
    //! \return Pointer to the timeline dock widget
    inline TimelineDockWidget * GetTimelineDockWidget() const { return mTimelineDockWidget; }

    //! Get the console dock widget
    //! \return Pointer to the console dock widget
    inline ConsoleDockWidget * GetConsoleDockWidget() const { return mConsoleDockWidget; }

    //------------------------------------------------------------------------------------

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
    void NewScene();
    void OpenApp();
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
    void OpenViewportWindow();
    void OpenTimelineWindow();
    void OpenConsoleWindow();
    //@}

    //@{
    //! Slots for the actions of the help menu
    void HelpIndex();
    void About();
    //@}

    //------------------------------------------------------------------------------------

public slots:

    // Extra slots for other parts of the editor that request global controls

    //! Open the preferences dialog box at the appearance page
    void OpenPreferencesAppearance();

    //------------------------------------------------------------------------------------

private:

    //@{
    //! Actions triggered when an item of the File menu is selected
    QAction * mActionFileNewScene;
    QAction * mActionFileOpenApp;
    QAction * mActionFileCloseApp;
    QAction * mActionFileQuit;
    //@}

    //@{
    //! Actions triggered when an item of the Edit menu is selected
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
	QAction * mActionWindowViewport;
    QAction * mActionWindowTimeline;
    QAction * mActionWindowConsole;
    //@}

    //@{
    //! Actions triggered when an item of the help menu is selected
    QAction * mActionHelpIndex;
    QAction * mActionHelpAboutQt;
    QAction * mActionHelpAbout;
    //@}

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

    //! Unique assertion manager
    AssertionManager * mAssertionManager;


    //! Splash screen displayed at the startup of the program
	QSplashScreen * mSplashScreen;

    //! Editor settings (preferences)
	static Settings * sSettings;

    //@{
    //! Dock widgets
    ViewportDockWidget * mViewportDockWidget;
    TimelineDockWidget * mTimelineDockWidget;
    ConsoleDockWidget * mConsoleDockWidget;
    //@}

	//! Menu containing the checkable actions for the tool bars
	QMenu * mToolbarMenu;

	//! Menu containing the checkable actions for the dock widgets
	QMenu * mDockMenu;


    //! Application manager, taking take of opening and closing applications
    ApplicationManager * mApplicationManager;
};


#endif // EDITOR_H
