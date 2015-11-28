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
#include "MessageControllers/AssetIOMessageController.h"
#include "Widgets/PegasusDockWidget.h"

#include "ui_TimelineDockWidget.h"

namespace Pegasus {
    namespace Timeline {
        class IBlockProxy;
    }
}

class QUndoStack;
class Editor;

//! Dock widget containing the timeline graphics view
class TimelineDockWidget : public PegasusDockWidget 
{
    Q_OBJECT

public:

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
    inline bool IsPlaying() const { return ui.playButton->isChecked(); }


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

    //! Special Pegasus forwarder function which asserts if this widget has focus
    virtual bool HasFocus() const { return hasFocus() || ui.graphicsView->hasFocus(); }


signals:

    //! Emitted when the current beat has been updated
    //! \param beat Beat after update, can have fractional part
    void BeatUpdated(float beat);

    //! Emitted when a block has been moved by the user
    void BlockMoved();

    //! Emitted when a single block is being selected (not multiple selection)
    void BlockSelected(Pegasus::Timeline::IBlockProxy * blockProxy);

    //! Emitted when blocks are deselected (single or multiple selection)
    void BlocksDeselected();

    //! Emitted when a block has been double clicked by the user
    void BlockDoubleClicked(Pegasus::Timeline::IBlockProxy * blockProxy);

    //! Emitted when the play mode button has been enabled or disabled
    //! \param enabled True if the play mode button has just been enabled
    void PlayModeToggled(bool enabled);
    
    //------------------------------------------------------------------------------------

public slots:

    //! Emitted when the timeline is being saved
    void SaveTimeline();

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
    void OnBlockSelected(Pegasus::Timeline::IBlockProxy* blockProxy);

    //! Called when blocks are deselected (single or multiple selection)
    void OnBlocksDeselected();

    //------------------------------------------------------------------------------------

private:

    //! Event filter override
    bool eventFilter(QObject* obj, QEvent* event);

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* applicationProxy);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

    //! Receives an IO message
    virtual void OnReceiveAssetIoMessage(AssetIOMessageController::Message::IoResponseMessage msg);


    //! User interface definition
    Ui::TimelineDockWidget ui;

    //! Current snapping mode (in number of ticks per snap)
    unsigned int mSnapNumTicks;

    //! Undo stack pointer
    QUndoStack * mUndoStack;

    //! True if undo commands can be sent
    bool mEnableUndo;

};


#endif  // EDITOR_TIMELINEDOCKWIDGET_H
