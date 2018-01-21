/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationConfig.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Shared config structure for a Pegasus application.

#ifndef PEGASUS_SHARED_APPCONFIG_H
#define PEGASUS_SHARED_APPCONFIG_H

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Shared/AssertReturnCode.h"
#include "Pegasus/Core/Shared/OsDefs.h"
#include "Pegasus/Window/Shared/WindowConfig.h"

namespace Pegasus {
namespace App {

//! System specific components.
enum ComponentType
{
    COMPONENT_BEGIN = 0,
    COMPONENT_WORLD = COMPONENT_BEGIN  ,//! World component. Renders the world / main timeline.
    COMPONENT_GRID                     ,//! Renders an xyz grid reticle.                       
    COMPONENT_DEBUG_CAMERA             ,//! Renders debug boxes for cameras.                       
    COMPONENT_TEXTURE_VIEW             ,//! Renders 2d texture view.                       
    COMPONENT_VOLUMES                  ,//! Debug rendering of volumes.     
    COMPONENT_LIGHTING_DEBUG           ,//! Debug rendering of lighting.     
    COMPONENT_TRANSLATION_SELECTION    ,//! Renders the icon / UI controller for translation    
    COMPONENT_ROTATION_SELECTION       ,//! Renders the icon / UI controller for rotation.      
    COMPONENT_SCALE_SELECTION          ,//! Renders the icon / UI controller for selection.     
    COMPONENT_DEBUG_TEXT               ,//! Text rendering
    COMPONENT_UNUSED_SLOT              ,//! Reserved for user stuff 
    COMPONENT_COUNT = COMPONENT_UNUSED_SLOT
};

//! System specific components flags.
typedef int ComponentTypeFlags;
const ComponentTypeFlags  COMPONENT_FLAG_WORLD                = 1 << COMPONENT_WORLD;//! World component. Renders the world / main timeline.
const ComponentTypeFlags  COMPONENT_FLAG_GRID                 = 1 << COMPONENT_GRID;//! Renders an xyz grid reticle.                       
const ComponentTypeFlags  COMPONENT_FLAG_DEBUG_CAMERA         = 1 << COMPONENT_DEBUG_CAMERA;//! Renders debug camera boxes.                       
const ComponentTypeFlags  COMPONENT_FLAG_TEXTURE_VIEW         = 1 << COMPONENT_TEXTURE_VIEW;//! Renders a 2d texture view.                       
const ComponentTypeFlags  COMPONENT_FLAG_VOLUMES              = 1 << COMPONENT_VOLUMES;//! Renders a box around a selection on the screen.     
const ComponentTypeFlags  COMPONENT_FLAG_LIGHTING_DEBUG       = 1 << COMPONENT_LIGHTING_DEBUG;//! Renders debug lights into the scene.     
const ComponentTypeFlags  COMPONENT_FLAG_TRANSLATION_SELECTION= 1 << COMPONENT_TRANSLATION_SELECTION;//! Renders the icon / UI controller for translation    
const ComponentTypeFlags  COMPONENT_FLAG_ROTATION_SELECTION   = 1 << COMPONENT_ROTATION_SELECTION;//! Renders the icon / UI controller for rotation.      
const ComponentTypeFlags  COMPONENT_FLAG_SCALE_SELECTION      = 1 << COMPONENT_SCALE_SELECTION;//! Renders the icon / UI controller for selection.     
const ComponentTypeFlags  COMPONENT_FLAG_DEBUG_TEXT           = 1 << COMPONENT_DEBUG_TEXT;
const ComponentTypeFlags  COMPONENT_FLAG_UNUSED_SLOT          = 1 << COMPONENT_UNUSED_SLOT; 
const ComponentTypeFlags  COMPONENT_FLAG_ALL                  = 0x7fffffff;

//! Shared configuration structure for a Pegasus app
struct ApplicationConfig
{
public:
    Os::ModuleHandle mModuleHandle; //!< Handle to the module containing this application
    const char* mBasePath; //!< The base path to load all assets from

    // Debug API

    Core::LogHandlerFunc mLoghandler; //!< Log handler for this application

#if PEGASUS_ENABLE_ASSERT
    Core::AssertionHandlerFunc mAssertHandler; //!< Assert handler for this application
#endif

    //! Default constructor
    inline ApplicationConfig()
        : mModuleHandle(0), mBasePath(nullptr)
#if PEGASUS_ENABLE_LOG
          ,mLoghandler(nullptr)
#endif
#if PEGASUS_ENABLE_ASSERT
          ,mAssertHandler(nullptr)
#endif
    {}
};

//! Shared configuration structure for a Pegasus application window
struct AppWindowConfig
{
public:
    //! True if the window needs to be a child of a given parent window (mParentWindowHandle)
    //! \warning This variable exists to handle cases where 0 is a valid window handle for a given operating system
    bool mIsChild;

    //! Platform-specific window handle of the parent window.
    //! Defined only when mIsChild == true
    //! \warning Assuming that mParentWindowHandle == 0 results in a non-child window will become a problem
    //!          if an operating system considers 0 as a valid handle. mIsChild is used to define if a parent window is actually defined
    Os::WindowHandle mParentWindowHandle;
    
    //! Flags containing the components that this window will render.
    ComponentTypeFlags mComponentFlags;

    int mWidth; //!< Initial width of the window in pixels (> 0)
    int mHeight; //!< Initial height of the window in pixels (> 0)

#if PEGASUS_ENABLE_PROXIES
    void* mRedrawArgCallback;
    Pegasus::Wnd::WindowConfig::RedrawProxyFun mRedrawEditorCallback;
#endif
    //! Default constructor
    inline AppWindowConfig()
        :   mIsChild(false),
            mParentWindowHandle(0),            
            mWidth(960),
            mHeight(540)
#if PEGASUS_ENABLE_PROXIES
            ,mRedrawArgCallback(nullptr)
            ,mRedrawEditorCallback(nullptr)
#endif
        {}
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_APP_H
