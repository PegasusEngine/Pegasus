/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowComponent.h
//! \author Kleber Garcia
//! \date   9/9/2015
//! \brief  Window component - represents a layer on the screen to render.

#ifndef PEGASUS_WND_ICOMPONENT
#define PEGASUS_WND_ICOMPONENT

#include "Pegasus/PropertyGrid/PropertyGridObject.h"

//fwd declarations
namespace Pegasus {
    namespace Core {
        class IApplicationContext;
    }

    namespace Wnd {
        class WindowComponentState;
        class Window;
    }
}

namespace Pegasus 
{
namespace Wnd
{

//! Context where this component is rendered.
struct ComponentContext
{
    Core::IApplicationContext* mAppContext; //!< the application context
    Window*                    mTargetWindow; //!< the current window using this component
};

//! A component is a window independent piece of rendering. Examples of components:
//! - frame rate counter.
//! - interactive camera.
//! - debug renderer.
class IWindowComponent
{
public:
    
    //Destructor
    virtual ~IWindowComponent(){}

    //! Creation of a component state related to a window.
    virtual WindowComponentState* CreateState(const ComponentContext& context) = 0;

    //! Destruction of a component state related to a window.
    virtual void DestroyState(const ComponentContext& context, WindowComponentState* state) = 0;

    //! Load / create any rendering specific elements. Do not draw anything on the screen.
    //! \param appContext - the application context
    virtual void Load(Core::IApplicationContext* appContext) = 0;

    //! Called once every tick by the app. Do any simulation operation on this function that is independant from a window.
    //! The order of layers will determine the order of execution of Update.
    virtual void Update(Core::IApplicationContext* appContext) = 0;

    //! Update on the window. Called once per window. Use this to update the internal state.
    //! \param context - context containing current window and app context
    //! \param state - state related to the window that is being updated.
    virtual void WindowUpdate(const ComponentContext& context, WindowComponentState* state) = 0;

    //! Called once for every window. 
    virtual void Render(const ComponentContext& context, WindowComponentState* state) = 0;

    //! Shutdown the component. Destroy anything that was created in Load()
    //! \param appContext - the application context
    virtual void Unload(Core::IApplicationContext* appContext) = 0;

#if PEGASUS_ENABLE_PROXIES
    enum MouseButton
    {
        MouseButton_Left,
        MouseButton_Right,
        MouseButton_Center,
        MouseButton_DoubleClick,
        MouseButton_None,
    };

    //! Mouse & editor event callbacks. Called from the editor.
    //! \param button - 1, 2 or 3, 1 is left, 2 is middle, 3 is right click.
    //! \param isDown true if the mouse is down, false otherwise
    //! \param x coordinate of the window, in texture coordinates [0, 1]
    //! \param y coordinate of the window, in texture coordintes [0, 1], where 1 is the top.
    virtual void OnMouseEvent(WindowComponentState* state, MouseButton button, bool isDown, float x, float y) {}

    //! Keyboard event, when a key is pressed. Called from the editor.
    //! \param key ascii value of the key pressed.
    //! \param isDown true if key is down, false if key goes up.
    virtual void OnKeyEvent(WindowComponentState* state, char key, bool isDown) {}
#endif

};

}
}

#endif
