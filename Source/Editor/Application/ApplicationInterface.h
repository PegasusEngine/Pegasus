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

class Application;
class CodeUserData;
class SourceCodeManagerEventListener;
class TimelineIOMessageController;
class GraphIOMessageController;
class WindowIOMessageController;
class ProgramIOMessageController;
class SourceIOMessageController;
class AssetIOMessageController;
class PropertyGridIOMessageController;

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

    //! Connect the asset event listener
    void ConnectAssetEventListeners();

    //! Disconnect the asset event listener
    void DisconnectAssetEventListeners();

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

    //! forwards a message to the asset IO controller so its executed in the render thread
    void ForwardAssetIoMessage(PegasusDockWidget* sender, AssetIOMCMessage msg);

    //! forwards a message to the shader IO controller so its executed in the render thread
    void ForwardSourceIoMessage(SourceIOMCMessage msg);

    //! forwards a message to the program IO controller so its executed in the render thread
    void ForwardProgramIoMessage(ProgramIOMCMessage msg);

    //! forwards a message to the window IO controller so its executed in the render thread
    void ForwardWindowIoMessage(WindowIOMCMessage msg);

    //! forwards a message to the property grid IO controller so its executed in the render thread
    void ForwardPropertyGridIoMessage(PropertyGridIOMCMessage msg);

    //! forwards a message to the graph IO controller so its executed in the render thread
    void ForwardGraphIoMessage(GraphIOMCMessage msg);

    //! forwards a to the window io controller so its executed in the render thread
    void ForwardTimelineIoMessage(TimelineIOMCMessage msg);

    //------------------------------------------------------------------------------------

private:

    //! Editor application object
    Application * mApplication;

    //! True while an assertion dialog box is shown to prevent any paint message to reach the application windows
    //! \todo Seems not useful anymore. Test and remove if possible
    //bool mAssertionBeingHandled;

    //! Controllers, used to process messages from the application to the render thread, and messages back to the UI
    AssetIOMessageController*        mAssetIoMessageController;
    SourceIOMessageController*       mSourceIoMessageController;
    ProgramIOMessageController*      mProgramIoMessageController;
    WindowIOMessageController*       mWindowIoMessageController;
    PropertyGridIOMessageController* mPropertyGridMessageController;
    GraphIOMessageController*        mGraphMessageController;
    SourceCodeManagerEventListener*  mSourceCodeEventListener;
    TimelineIOMessageController*     mTimelineMessageController;

    
};


#endif  // EDITOR_APPLICATIONINTERFACE_H
