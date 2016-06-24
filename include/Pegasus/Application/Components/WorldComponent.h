/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WorldComponent.h
//! \author Kleber Garcia
//! \date   9/10/2015
//! \brief  Main component for world elements - the actual demoscene.

#ifndef WORLD_COMPONENT_H
#define WORLD_COMPONENT_H

#include "Pegasus/Window/IWindowComponent.h"
#include "Pegasus/Window/WindowComponentState.h"
#include "Pegasus/Render/Render.h"

//!Forward declarations
namespace Pegasus {
    namespace Alloc {
        class IAllocator;
    }

    namespace Core {
        class IApplicationContext;
    }
}

namespace Pegasus {
    namespace App {
        BEGIN_DECLARE_ENUM(WorldModeType)
            DECLARE_ENUM(WorldModeType, FILL_LIGHT)
            DECLARE_ENUM(WorldModeType, WIREFRAME)
        END_DECLARE_ENUM()
    }
}

REGISTER_ENUM_METATYPE(Pegasus::App::WorldModeType)


namespace Pegasus {
namespace App {


//State of the world component
class WorldComponentState : public Wnd::WindowComponentState
{
    BEGIN_DECLARE_PROPERTIES(WorldComponentState, WindowComponentState)
        DECLARE_PROPERTY(WorldModeType, Mode, FILL_LIGHT)
    END_DECLARE_PROPERTIES()

public:
    WorldComponentState();
    virtual ~WorldComponentState() {}
};

class WorldComponent : public Wnd::IWindowComponent
{

public:

    //! Constructor
    explicit WorldComponent(Alloc::IAllocator* allocator);

    //! Destructor
    virtual ~WorldComponent();

    //! Creation of a component state related to a window.
    virtual Wnd::WindowComponentState* CreateState(const Wnd::ComponentContext& context);

    //! Destruction of a component state related to a window.
    virtual void DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Load / create any rendering specific elements. Do not draw anything on the screen.
    virtual void Load(Core::IApplicationContext* appContext);

    //! Called once every tick by the app. Do any simulation operation on this function that is independant from a window.
    //! The order of layers will determine the order of execution of Update.
    virtual void Update(Core::IApplicationContext* appContext);

    //! Update on the window. Called once per window. Use this to update the internal state.
    //! \param context - context containing current window and app context
    //! \param state - state related to the window that is being updated.
    virtual void WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Called once for every window. 
    virtual void Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Shutdown the component. Destroy anything that was created in Load()
    virtual void Unload(Core::IApplicationContext* appContext);

private:
    Alloc::IAllocator* mAlloc;
    
    Render::RasterizerStateRef mDefaultRasterState;
};

}
}

#endif

