/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineDockWidget.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the timeline graphics view

#ifndef EDITOR_TIMELINEDOCKWIDGET_H
#define EDITOR_TIMELINEDOCKWIDGET_H

#include <QDockWidget>
#include "ui_TimelineDockWidget.h"


//! Dock widget containing the timeline graphics view
class TimelineDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    TimelineDockWidget(QWidget *parent = 0);
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

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the current beat has been updated
    //! \param beat Beat after update, can have fractional part
    void BeatUpdated(float beat);

    //! Emitted when the play mode button has been enabled or disabled
    //! \param enabled True if the play mode button has just been enabled
    void PlayModeToggled(bool enabled);

    //------------------------------------------------------------------------------------

public slots:

    //! Called when an application is successfully loaded to unlock the controls of the timeline
    //! and reset its state
    void UpdateUIForAppLoaded();

    //! Called when an application is closed to lock the controls of the timeline
    //! and reset its state
    void UpdateUIForAppClosed();

    //------------------------------------------------------------------------------------

private slots:

    //! Called when the speed of the demo has changed
    //! \param bpm Tempo in beats per minute
    void SetBeatsPerMinute(double bpm);

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

    //------------------------------------------------------------------------------------

private:

    Ui::TimelineDockWidget ui;
};


#endif  // EDITOR_TIMELINEDOCKWIDGET_H
