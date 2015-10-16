/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationInterface.h
//! \author Karolyn Boulanger
//! \date   20th October 2013
//! \brief  Interface object used to interface with the Pegasus related messages,
//!         created in the application thread

#ifndef EDITOR_APPLICATIONINTERFACE_H
#define EDITOR_APPLICATIONINTERFACE_H

#include "ProgramEditor/ProgramEditorWidget.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/SourceIOMessageController.h"
#include "MessageControllers/ProgramIOMessageController.h"
#include "MessageControllers/WindowIOMessageController.h"

class Application;
class CodeUserData;
class SourceCodeManagerEventListener;

namespace Pegasus {
    namespace Timeline {
        class IBlockProxy;
    }
}

class PegasusDockWidget;



//! Interface object used to interface with the Pegasus related messages,
//! created in the application thread
class ApplicationInterface : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param application Editor application object (!= nullptr)
    ApplicationInterface(Application * application);

    //! Destructor, closes the running application
    virtual ~ApplicationInterface();


    //! Set the state indicating if an assertion error is currently being handled
    //! \param state True if an assertion is currently being handled
    //! \todo Seems not useful anymore. Test and remove if possible
    //inline void SetAssertionBeingHandled(bool state) { mAssertionBeingHandled = state; }

    //------------------------------------------------------------------------------------

    //! Garbage collect all windows and kill them.
    void DestroyAllWindows();

signals:

    //! Emitted when a viewport rendering completes while being in play mode
    //! \param beat Beat used to render the last frame in the viewport, can have fractional part
    void ViewportRedrawnInPlayMode(float beat);

    //! Signal emitted when \a RequestSetCurrentBeatAfterBeatUpdated() has been called at least once
    void EnqueuedBeatUpdated();

    //! Signal emitted when \a RequestRedrawAllViewportsAfterBlockMoved() has been called at least once
    void EnqueuedBlockMoved();

    //------------------------------------------------------------------------------------
    
private slots:

    //! Request the redraw of the content of the main viewport
    //! \param updateTimeline True to update the timeline, so animation happens
    //! \return True if drawing happened (when the dock widget is open)
    void RedrawMainViewport();

    //! Request the redraw of the content of the secondary viewport
    //! \return True if drawing happened (when the dock widget is open)
    void RedrawSecondaryViewport();

    //! Request the redraw of the content of the main and secondary viewports
    //! \note Nothing happens if the dock widgets are closed
    void RedrawAllViewports();

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

    //! Enable or disable the play mode of the demo timeline
    //! \param enabled True if the play mode has just been enabled, false if it has just been disabled
    void TogglePlayMode(bool enabled);

    //! Request the rendering of a new frame while in play mode
    void RequestFrameInPlayMode();
    
    //! slot received when the user double clicks a block in the timeline
    void PerformBlockDoubleClickedAction(Pegasus::Timeline::IBlockProxy* blockProxy);

    //! forwards a to the asset io controller so its executed in the render thread
    void ForwardAssetIoMessage(PegasusDockWidget* sender, AssetIOMessageController::Message msg);

    //! forwards a to the shader io controller so its executed in the render thread
    void ForwardSourceIoMessage(SourceIOMessageController::Message msg);

    //! forwards a to the program io controller so its executed in the render thread
    void ForwardProgramIoMessage(ProgramIOMessageController::Message msg);

    //! forwards a to the window io controller so its executed in the render thread
    void ForwardWindowIoMessage(WindowIOMessageController::Message msg);

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

    //! Controllers, used to process messages from the application to the render thread, and messages back to the UI
    AssetIOMessageController*       mAssetIoMessageController;
    SourceIOMessageController*      mSourceIoMessageController;
    ProgramIOMessageController*     mProgramIoMessageController;
    WindowIOMessageController*      mWindowIoMessageController;
    SourceCodeManagerEventListener* mSourceCodeEventListener;
    
};


#endif  // EDITOR_APPLICATIONINTERFACE_H
