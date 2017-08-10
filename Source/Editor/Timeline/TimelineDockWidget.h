/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineDockWidget.h
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the timeline graphics view

#ifndef EDITOR_TIMELINEDOCKWIDGET_H
#define EDITOR_TIMELINEDOCKWIDGET_H

#include <QDockWidget>
#include <QVector>
#include "MessageControllers/MsgDefines.h"
#include "Widgets/PegasusDockWidget.h" 
#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "PropertyGrid/qtpropertybrowser/qtpropertybrowser.h"

namespace Pegasus {

    namespace App {
        class IApplicationProxy;
    }

    struct PegasusAssetTypeDesc;
}

namespace Ui
{
    class TimelineDockWidget;
}

class QUndoStack;
class QMenu;
class Editor;
class QAction;
class QGraphicsObject;
class QSignalMapper;
class TdwObserver;
class TimelineIOMessageObserver;

//! Dock widget containing the timeline graphics view
class TimelineDockWidget : public PegasusDockWidget
{
    Q_OBJECT

public:
    friend TdwObserver;

    TimelineDockWidget(QWidget *parent, Editor* editor);
    virtual ~TimelineDockWidget();


    //! Set the antialiasing mode of the timeline
    //! \param enable True to enable antialiasing
    void EnableAntialiasing(bool enable);

    //! Update the UI elements from the given beat
    //! \param beat Current beat, can have fractional part
    void UpdateUIFromBeat(float beat);

    //! Test if the play mode is currently enabled
    //! \return True if the play button is currently toggled
    bool IsPlaying() const;

    //! Set the speed of the demo
    //! \note Updates the Pegasus timeline, refreshed the tempo field and updates the graphics view
    //! \param bpm Tempo in beats per minute
    void SetBeatsPerMinute(double bpm);

    //! Get the current snapping mode
    //! \return Number of ticks per snap
    inline unsigned int GetSnapNumTicks() const { return mSnapNumTicks; }

    //------------------------------------------------------------------------------------
    //----------- Pegasus Dock Widget overrides -----------------------------------

    //! Returns the current undo stack in focus for this widget
    //! \return implementation specific, must return the current active undo stack of this widget
    virtual QUndoStack * GetCurrentUndoStack() const { return mUndoStack; }

    //! Returns the name this widget
    virtual const char * GetName() const  { return "TimelineDockWidget"; }

    //! Returns the title of this widget
    virtual const char * GetTitle() const { return "Timeline"; }

    //! Callback to construct ui
    virtual void SetupUi();

    //! Callback, implement here functionality that requires saving of current object
    virtual void OnSaveFocusedObject();

    //! Callback, implement here functionality that would pursue the deletion of whatever this widget has selected internally
    virtual void OnDeleteFocusedObject();

    //! Special Pegasus forwarder function which asserts if this widget has focus
    virtual bool HasFocus() const;

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc*const* GetTargetAssetTypes() const;

    //! Implement this function with functionality on how to process for edit.
    //! Only objects of type retured in GetTargetAssetTypes will be the ones opened.
    virtual void OnOpenObject(AssetInstanceHandle object, const QString& displayName, const QVariant& initData);
    
    //------------------------------------------------------------------------------------

    const AssetInstanceHandle& GetTimelineAssetHandle() const { return mTimelineHandle; }

    //! Shadow state of the timeline.
    const ShadowTimelineState& GetProxy() const { return mTimelineState; }
signals:


public slots:

    //! Emitted when the timeline is being saved
    void SaveTimeline();

    //!  Focus command on a block item in the graphics view
    void OnFocusBlock(unsigned blockGuid);

    //When the master timeline button is pressed. request load of a blockscript asset.
    void RequestMasterTimelineScriptLoad();

    //When clicked removes the master timeline script.
    void EditMasterScript();

    //When clicked removes the master timeline script.
    void RemoveMasterScript();
    
    //! Called when the timeline requires to be repainted.
    void OnRepaintTimeline();

    //! Called when there is an active timeline script
    void OnShowActiveTimelineButton(bool shouldShowActiveScript, QString script);

    //! Emitted when a block has been double clicked by the user
    void OnBlockDoubleClicked(QString blockScriptToOpen);

    //! When a new block has been requested
    void OnCreateNewBlock(int blockTypeId);

    //! Emitted when the play mode button has been enabled or disabled
    void OnPlayModeToggled(bool enabled);

    //When clicked requests adding a new music file.
    void OpenMusicFile();

    //When clicked removes the current musci file.
    void RemoveMusicFile();

    //Toggle for mute/unmute button
    void MuteUnmuteToggle(bool isNotMuted);

private slots:

    //! Called when the tempo has changed value
    //! \param bpm Tempo in beats per minute
    void OnBeatsPerMinuteChanged(double bpm);

    //! Called when the snap mode has changed
    //! \param New snap mode
    void OnSnapModeChanged(int mode);

    //! Called when the current beat has been updated
    //! \param beat Current beat, can have fractional part
    void SetCurrentBeat(float beat);

    //! Set the label of the current beat
    //! \param beat Integer beat (>= 0)
    //! \param subBeat Integer sub-beat (0, 1, 2, 3), representing quarter beats
    //! \param subSubBeat Integer sub-sub-beat (0, 1 , 2, 3), representing sixteenth of beats
    void SetBeatLabel(unsigned int beat, unsigned int subBeat, unsigned int subSubBeat);

    //! Set the label of the current time
    //! \param minutes Integer number of minutes (can be > 59)
    //! \param seconds Integer number of seconds (0 <= seconds <= 59)
    //! \param milliseconds Integer number of milliseconds (0 <= milliseconds <= 999)
    void SetTimeLabel(unsigned int minutes, unsigned int seconds, unsigned int milliseconds);

    //! Called when a single block is being selected (not multiple selection)
    void OnBlockSelected(unsigned blockGuid);

    //! Called when multiple blocks are being selected (not single selection)
    void OnMultiBlocksSelected();

    //! Called when blocks are deselected (single or multiple selection)
    void OnBlocksDeselected();

    //! Called when a property has been updated
    void OnPropertyUpdated(QtProperty* property);

    //! Called to request / remove blockscripts
    void RequestChangeScript(QGraphicsObject* sender, unsigned blockGuid); 
    void RequestRemoveScript(QGraphicsObject* sender, unsigned blockGuid); 
    void RequestMoveBlock(QGraphicsObject* sender, QPointF amount);
    void RequestDeleteSelectedBlocks();

    //------------------------------------------------------------------------------------

public:

    TimelineIOMessageObserver* GetObserver() { return mObserver; }

private:

    TimelineIOMessageObserver* mObserver;

    //! Event filter override
    bool eventFilter(QObject* obj, QEvent* event);

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* applicationProxy);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

    //! Receives an IO message
    virtual void OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage msg);

    //! Launch loader window to find timeline script
    QString AskForTimelineScript();

    //! Current snapping mode (in number of ticks per snap)
    unsigned int mSnapNumTicks;

    //! reference to the application proxy
    Pegasus::App::IApplicationProxy* mApplication;

    //! Undo stack pointer
    QUndoStack * mUndoStack;

    //! Menu button for script open / removal
    QMenu* mMasterScriptMenu;

    //! Menu for music options
    QMenu* mMusicOptionsMenu;

    //! Menu containing all the blockscripts available by the app
    QMenu* mAppAvailableBlocksMenu;

    //! Contains a list of strings with the name of the available blocks.
    char mAvailableBlocksClassNames[Pegasus::Timeline::MAX_NUM_REGISTERED_BLOCKS][Pegasus::Timeline::MAX_BLOCK_CLASS_NAME_LENGTH + 1];
    char mAvailableBlocksEditorNames[Pegasus::Timeline::MAX_NUM_REGISTERED_BLOCKS][Pegasus::Timeline::MAX_BLOCK_EDITOR_STRING_LENGTH + 1];
    unsigned int mAvailableBlocksNamesCount;
    QVector<QAction*> mAvailableBlocksActions;

    //! Signal mapper, apping the id of the right context to use for the new block selected.
    QSignalMapper* mAppAvailableBlocksSignalMapper;

    //! True if undo commands can be sent
    bool mEnableUndo;

    //! Boolean state that indicates if a timeline is active open, or if there is non open.
    //! This will soon be replaced by a proper active handle once we support multiple timelines edits
    bool mTimelineOpen;

    //If 1 block is selected this will contain its guid. If non, it contains ~0;
    unsigned mCurrentBlockSelected;

    //! Handle of the asset instance
    AssetInstanceHandle mTimelineHandle;

    //! Shadow state of the timeline.
    ShadowTimelineState mTimelineState;

    //! path of master loaded script
    QString mLoadedScript;

    //! actions for removing / editing master script
    QAction* mEditMasterScriptButton;
    QAction* mOpenMasterScriptButton;
    QAction* mRemoveMasterScriptButton;

    //! actions for removing / editing music
    QAction* mOpenNewMusicFile;
    QAction* mRemoveMusicFile;

    //boolean avoids sending too many messages on setting beat.
    bool mIsCursorQueued;

    //! User interface definition
    Ui::TimelineDockWidget* mUi;

};


#endif  // EDITOR_TIMELINEDOCKWIDGET_H
