/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramEditorWidget.h
//! \author	Kleber Garcia
//! \date	10rth March 2015
//! \brief	Program editor IDE

#ifndef EDITOR_PROGRAM_EDITOR_WIDGET_H
#define EDITOR_PROGRAM_EDITOR_WIDGET_H

#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/ProgramIOMessageController.h"

#include <QDockWidget>

#define MAX_PROGRAMS_OPEN 50

class QVBoxLayout;
class QToolButton;
class QSignalMapper;
class QToolBar;
class QTabBar;
class QFocusEvent;
class QStatusBar;

namespace Pegasus
{
    namespace Shader
    {
        class IProgramProxy;
    }
}

class ProgramEditorWidget : public QDockWidget
{
    Q_OBJECT;

public:
    //! Constructor
    ProgramEditorWidget(QWidget * parent);

    //! Destructor
    virtual ~ProgramEditorWidget();

public slots:
    //! Opens a program by gathering its state and displaying it in the ui
    //! \param program the program to request open
    void RequestOpenProgram(Pegasus::Shader::IProgramProxy* program);

    //! Closes a program.
    //! \param the tab index to request close
    void RequestCloseProgram(int tabIndex);

    //! Saves current program.
    void SignalSaveCurrentProgram();

    //! triggers when the application has been unloaded
    void UpdateUIForAppFinished();

    //! Synchronizes the current program to the UI
    void SyncUiToProgram();

    //! Posts a message to the status bar
    void PostStatusBarMessage(const QString& message);


signals:
    //! Sends a job to the render thread. This is connected in the application interface
    void SendProgramIoMessage(ProgramIOMessageController::Message msg);

    //! Sends a render thread IO Message.
    void SendAssetIoMessage(AssetIOMessageController::Message msg);

private:

    //! Ui struct describing a slot
    struct ShaderSlots
    {
        QLabel      *mSlotName;      
        QLabel      *mAsset;
        QToolButton *mLoadButton;
        QToolButton *mDeleteButton;
        QString      mFullAssetPath;
        bool         mActive;
    } mShaderSlots[Pegasus::Shader::SHADER_STAGES_COUNT];

    //! Enables/Disables the UI
    //! \param enable if true activates the UI, if false deactivates it. Use this to figure out
    //!        looks of UI when there is no program opened
    void EnableUi(bool enable);

    //! To be called in constructor
    void SetupUi();

    //! Clears the ui to a default state.
    void ClearUi();

    QSignalMapper* mAddShaderMapper;
    QSignalMapper* mRemoveShaderMapper;
    QTabBar*       mTabBar;
    QWidget*       mMainWidget;
    QAction*       mSaveAction;
    QStatusBar*    mStatusBar;
    QString        mStatusBarMessage;

protected slots:
    //! Triggered when a shader has been added / modified
    void OnAddShader(int stageId);

    //! Triggered when a shader has been removed
    void OnRemoveShader(int stageId);

    //! Triggered when a program has been viewd
    void OnViewProgram(int programid);

private:

    //! Static array holding all the programs opened.
    Pegasus::Shader::IProgramProxy* mPrograms[MAX_PROGRAMS_OPEN];

    //! Pointer to the current program opened
    Pegasus::Shader::IProgramProxy* mCurrentProgram;
};


#endif
