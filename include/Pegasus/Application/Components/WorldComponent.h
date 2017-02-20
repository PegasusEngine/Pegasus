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

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Window/IWindowComponent.h"
#include "Pegasus/Window/WindowComponentState.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/RenderSystems/Camera/Camera.h"

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


//State of the world component
class WorldComponentState : public Wnd::WindowComponentState
{
    BEGIN_DECLARE_PROPERTIES(WorldComponentState, WindowComponentState)
        DECLARE_PROPERTY(bool, IsWireframe, false)
        DECLARE_PROPERTY(bool, EnableFreeCam, false)
        DECLARE_PROPERTY(bool, ResetFreeCam, false)
    END_DECLARE_PROPERTIES()

public:
    WorldComponentState(Alloc::IAllocator* allocator, int windowIndex);
    virtual ~WorldComponentState() {}

#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    Camera::CameraRef GetFreeCam() const { return mFreeCam; }
    void ResetFreeCamPos();
#endif

    int GetWindowIndex() const { return mWindowIndex; }

private:
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    Camera::CameraRef mFreeCam;
#endif
    Alloc::IAllocator* mAllocator;
    int mWindowIndex;
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

    //! unique id
    virtual unsigned int GetUniqueId() const { return COMPONENT_WORLD; }

#if PEGASUS_ENABLE_PROXIES
    virtual void OnMouseEvent(Wnd::WindowComponentState* state, MouseButton button, bool isDown, float x, float y);

    virtual void OnKeyEvent(Wnd::WindowComponentState* state, Pegasus::Wnd::Keys key, bool isDown);
#endif

private:
    Alloc::IAllocator* mAlloc;
    unsigned int mWindowIndexCount;
    
#if PEGASUS_ENABLE_PROXIES
    enum Cmd{
        CMD_NONE,
        CMD_ORBIT,
        CMD_PAN,
        CMD_POS_ZOOM,
        CMD_FOV_ZOOM
    };

    struct FreeCamState
    {
        float x, y;
        Cmd cmd;
        bool mKeyStates[Pegasus::Wnd::KEYS_COUNT];
    public:
        FreeCamState() : x(0.0f), y(0.0f), cmd(CMD_NONE) { 
            for (int i = 0; i < Pegasus::Wnd::KEYS_COUNT; ++i) mKeyStates[i] = false;
        }
    };

    FreeCamState mFreeCamState;
#endif 

    Render::RasterizerStateRef mDefaultRasterState;
};

}
}

#endif

