/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	EditorComponents.h
//! \author	Kleber Garcia
//! \date   March 29th 2016
//! \brief	Timeline block, describing the instance of an effect on the timeline
//! \brief	Application visual components for editor specific items (things that get edited)

#ifndef EDITOR_COMPONENT_H
#define EDITOR_COMPONENT_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Window/IWindowComponent.h"
#include "Pegasus/Window/WindowComponentState.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Shader/ProgramLinkage.h"

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
class TextureViewComponentState : public Wnd::WindowComponentState
{
public:
    BEGIN_DECLARE_PROPERTIES(TextureViewComponentState, WindowComponentState)
        DECLARE_PROPERTY(Pegasus::Math::Vec2, Offset, Pegasus::Math::Vec2(0.0f,0.0f))
        DECLARE_PROPERTY(Pegasus::Math::Vec2, Scale, Pegasus::Math::Vec2(1.0f))
    END_DECLARE_PROPERTIES()

public:
    TextureViewComponentState();
    virtual ~TextureViewComponentState() {}

    //private state not visible on the UI:
    Pegasus::Texture::TextureRef mTargetTexture;
    bool mOnStartOffset; // when the user decides to start offset capturing.
    bool mOnStartScale; // when the user decides to start scaling.
    float mPrevMousePos[2]; //mouse pos when clicked down.
    float mMousePos[2]; //current mouse pos while click is down.
    
};

class TextureViewComponent : public Wnd::IWindowComponent
{

public:
    //! Constructor
    explicit TextureViewComponent(Alloc::IAllocator* allocator);

    //! Destructor
    virtual ~TextureViewComponent();

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

    //! Mouse & editor event callbacks. Called from the editor.
    //! \param button - 1, 2 or 3, 1 is left, 2 is middle, 3 is right click.
    //! \param isDown true if the mouse is down, false otherwise
    //! \param x coordinate of the window, in texture coordinates [0, 1]
    //! \param y coordinate of the window, in texture coordintes [0, 1], where 1 is the top.
    virtual void OnMouseEvent(Wnd::WindowComponentState* state, IWindowComponent::MouseButton button, bool isDown, float x, float y);

    //! Keyboard event, when a key is pressed. Called from the editor.
    //! \param key ascii value of the key pressed.
    //! \param isDown true if key is down, false if key goes up.
    virtual void OnKeyEvent(Wnd::WindowComponentState* state, char key, bool isDown);

private:
    Alloc::IAllocator* mAlloc;
    Pegasus::Shader::ProgramLinkageRef mTextureRenderProgram;
    Pegasus::Mesh::MeshRef mQuad;
    Pegasus::Render::Uniform mTargetTexUniform;
};

}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif //EDITOR_COMPONENT_H

