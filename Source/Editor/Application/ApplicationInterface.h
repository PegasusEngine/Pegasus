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

    //------------------------------------------------------------------------------------
    
private slots:

    //! Called when the viewport needs to be resized
    //! \param viewportType Type of the viewport (VIEWPORTTYPE_xxx constant)
    //! \param width New width of the viewport, in pixels
    //! \param height New height of the viewport, in pixels
    void ResizeViewport(ViewportType viewportType, int width, int height);

    //! Called when a request to redraw the content of the main viewport is sent
    void RedrawMainViewport();

    //! Enable or disable the play mode of the demo timeline
    //! \param enabled True if the play mode has just been enabled, false if it has just been disabled
    void TogglePlayMode(bool enabled);

    //! Request the rendering of a new frame while in play mode
    void RequestFrameInPlayMode();

    //! Called when the current beat has been updated
    //! \param beat Current beat, can have fractional part
    void SetCurrentBeat(float beat);

    //! Called when the current beat has been updated
    //! \param beat Current beat, can have fractional part
    void ReceiveShaderCompilationRequest(int id);

    //------------------------------------------------------------------------------------

private:

    //! Editor application object
    Application * mApplication;

    //! True while an assertion dialog box is shown to prevent any paint message to reach the application windows
    //! \todo Seems not useful anymore. Test and remove if possible
    //bool mAssertionBeingHandled;
};


#endif  // EDITOR_APPLICATIONINTERFACE_H
