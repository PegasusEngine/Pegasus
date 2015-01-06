/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationInterface.h
//! \author Kevin Boulanger
//! \date   20th October 2013
//! \brief  Interface object used to interface with the Pegasus related messages,
//!         created in the application thread

#ifndef EDITOR_APPLICATIONINTERFACE_H
#define EDITOR_APPLICATIONINTERFACE_H

#include "Viewport/ViewportType.h"

class Application;


//! Interface object used to interface with the Pegasus related messages,
//! created in the application thread
class ApplicationInterface : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param application Editor application object (!= nullptr)
    //! \param parent Parent Qt object
    ApplicationInterface(Application * application, QObject * parent = 0);

    //! Destructor, closes the running application
    virtual ~ApplicationInterface();


    //! Set the state indicating if an assertion error is currently being handled
    //! \param state True if an assertion is currently being handled
    //! \todo Seems not useful anymore. Test and remove if possible
    //inline void SetAssertionBeingHandled(bool state) { mAssertionBeingHandled = state; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when a viewport rendering completes while being in play mode
    //! \param beat Beat used to render the last frame in the viewport, can have fractional part
    void ViewportRedrawnInPlayMode(float beat);

    //! Signal emitted when \a RequestSetCurrentBeatAfterBeatUpdated() has been called at least once
    void EnqueuedBeatUpdated();

    //! Signal emitted when \a RequestRedrawAllViewportsAfterBlockMoved() has been called at least once
    void EnqueuedBlockMoved();

    //! Signal emitted when a compilation redraw has finished.
    void CompilationEnd();

    //------------------------------------------------------------------------------------
    
private slots:

    //! Called when the viewport needs to be resized
    //! \param viewportType Type of the viewport (VIEWPORTTYPE_xxx constant)
    //! \param width New width of the viewport, in pixels
    //! \param height New height of the viewport, in pixels
    void ResizeViewport(ViewportType viewportType, int width, int height);

    //! Request the redraw of the content of the main viewport
    //! \param updateTimeline True to update the timeline, so animation happens
    //! \return True if drawing happened (when the dock widget is open)
    bool RedrawMainViewport(bool updateTimeline = true);

    //! Request the redraw of the content of the secondary viewport
    //! \return True if drawing happened (when the dock widget is open)
    bool RedrawSecondaryViewport(bool updateTimeline = true);

    //! Request the redraw of the content of the main and secondary viewports
    //! \note Nothing happens if the dock widgets are closed
    void RedrawAllViewports();

    //! Request the redraw of the texture editor preview
    //! \note Nothing happens if the dock widget is closed
    //! \return True if drawing happened (when the dock widget is open)
    bool RedrawTextureEditorPreview();


    //! Request a set current beat call in the application thread after the current beat has been updated on the timeline
    //! \param beat Current beat, can have fractional part
    void RequestSetCurrentBeatAfterBeatUpdated(float beat);

    //! Request a redraw all viewports call in the application thread after a block has been moved on the timeline
    void RequestRedrawAllViewportsAfterBlockMoved();

    //! Called when the current beat has been updated, to force a redraw of all viewports
    //! \note Uses mSetCurrentBeatEnqueuedBeat for the beat to use
    void SetCurrentBeat();

    //! Called when a timeline block has been moved, to force a redraw of all viewports
    void RedrawAllViewportsForBlockMoved();

    //! Called when the current beat has been updated
    //! \param id ID of the source code
    void ReceiveCompilationRequest(int id);


    //! Enable or disable the play mode of the demo timeline
    //! \param enabled True if the play mode has just been enabled, false if it has just been disabled
    void TogglePlayMode(bool enabled);

    //! Request the rendering of a new frame while in play mode
    void RequestFrameInPlayMode();

    //------------------------------------------------------------------------------------

private:

    //! Editor application object
    Application * mApplication;

    //! True while an assertion dialog box is shown to prevent any paint message to reach the application windows
    //! \todo Seems not useful anymore. Test and remove if possible
    //bool mAssertionBeingHandled;

    //! True while a set current beat call has been enqueued (to avoid duplicated calls, reset by the Pegasus thread)
    bool mSetCurrentBeatEnqueued;

    //! Enqueued beat to use for a set current beat call
    float mSetCurrentBeatEnqueuedBeat;

    //! True while a redraw all viewports call has been enqueued (to avoid duplicated calls, reset by the Pegasus thread)
    bool mRedrawAllViewportsForBlockMovedEnqueued;
};


#endif  // EDITOR_APPLICATIONINTERFACE_H
