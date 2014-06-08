/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowRegistry.h
//! \author David Worsham
//! \date   20th October 2013
//! \brief  Interface for a window registry, which is a repository of window types.

#ifndef PEGASUS_APP_IWINDOWREGISTRY_H
#define PEGASUS_APP_IWINDOWREGISTRY_H

#include "Pegasus/Window/Window.h"

namespace Pegasus {
namespace App {


//! Window tag type, this is the window "meta-type" as seen from outside the runtime.
//! IE TYPE_MAIN, TYPE_TEXTUREEDITORPREVIEW, TYPE_MESHEDITORPREVIEW, etc
enum WindowTypeTag
{
    WINDOW_TYPE_INVALID = -1,           //!< Not a valid window type.
    WINDOW_TYPE_MAIN = 0,               //!< The main view window in the editor, or the main app window when launched alone
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    WINDOW_TYPE_SECONDARY,              //!< The optional secondary view window in the editor
    WINDOW_TYPE_TEXTUREEDITORPREVIEW,   //!< Texture viewer for the editor
    WINDOW_TYPE_MESHEDITORPREVIEW,      //!< Mesh viewer for the editor
#endif

    NUM_WND_TYPES
};


//! Registration structure for a window type.
struct WindowRegistration
{
public:
    WindowTypeTag mTypeTag; //!< Type tag for the window "meta type"
    char* mDescription; //!< User friendly description of the window for the editor.  nullptr in REL mode
    Wnd::WindowFactoryFunc mCreateFunc; //!< Function to use to create this window type
};


//! Abstract interface for a registry of window types.  Exists so we can expose a subset of the window
//! manager to the rest of the app.
class IWindowRegistry
{
public:
    //! Destructor.
    //! \warning Unregister your classes before you call this!
    virtual ~IWindowRegistry() {};


    //! Registers a window class with this registry
    //! \param className Name of the class to register.
    //! \param classReg Class registration info.
    //! \warning Registering the same name more than once is not permitted, nor is the registration
    //!          of more than one MAIN type window.
    virtual void RegisterWindowClass(const char* className, const WindowRegistration& classReg) = 0;

    //! Unregisters a window class with this registry
    //! \param className Name of the class to unregister.
    virtual void UnregisterWindowClass(const char* className) = 0;

    //! Gets the class name of the MAIN window type for for this registry
    //! \return Class name.
    virtual const char* GetMainWindowType() const = 0;

#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES

    //! Gets the class name of the SECONDARY window type for for this registry
    //! \return Class name.
    virtual const char* GetSecondaryWindowType() const = 0;

    //! Gets the class name of the TEXTURE_EDITOR_PREVIEW window type for this registry
    //! \return Class name.
    virtual const char* GetTextureEditorPreviewWindowType() const = 0;

#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_APP_IWINDOWREGISTRY_H
