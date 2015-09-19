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

//fwd declarations
namespace Pegasus {
    namespace Core {
        class IApplicationContext;
    }

    namespace Wnd {
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

    //! Struct representing the state of this context.
    //! Every component that requires state per window needs to implement / create a state for itself.
    struct IState
    {
    };

    //! Creation of a component state related to a window.
    virtual IState* CreateState(const ComponentContext& context) = 0;

    //! Destruction of a component state related to a window.
    virtual void DestroyState(const ComponentContext& context, IState* state) = 0;

    //! Load / create any rendering specific elements. Do not draw anything on the screen.
    //! \param appContext - the application context
    virtual void Load(Core::IApplicationContext* appContext) = 0;

    //! Called once every tick by the app. Do any simulation operation on this function that is independant from a window.
    //! The order of layers will determine the order of execution of Update.
    virtual void Update(Core::IApplicationContext* appContext) = 0;

    //! Update on the window. Called once per window. Use this to update the internal state.
    //! \param context - context containing current window and app context
    //! \param state - state related to the window that is being updated.
    virtual void WindowUpdate(const ComponentContext& context, IState* state) = 0;

    //! Called once for every window. 
    virtual void Render(const ComponentContext& context, IState* state) = 0;

    //! Shutdown the component. Destroy anything that was created in Load()
    //! \param appContext - the application context
    virtual void Unload(Core::IApplicationContext* appContext) = 0;

};

}
}

#endif
