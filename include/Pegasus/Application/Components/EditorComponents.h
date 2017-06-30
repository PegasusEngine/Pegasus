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

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Window/IWindowComponent.h"
#include "Pegasus/Window/WindowComponentState.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/RenderSystems/Config.h"
#include "Pegasus/Math/Matrix.h"

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

    //! unique id
    virtual unsigned int GetUniqueId() const { return COMPONENT_TEXTURE_VIEW; }

private:
    Alloc::IAllocator* mAlloc;
    Pegasus::Shader::ProgramLinkageRef mTextureRenderProgram;
    Pegasus::Mesh::MeshRef mQuad;
    Pegasus::Render::Uniform mTargetTexUniform;
    Pegasus::Render::RasterizerStateRef mRasterState;
    Pegasus::Render::BlendingStateRef mBlendState;
};

//3d Grid component
class GridComponentState : public Wnd::WindowComponentState
{
public:
    BEGIN_DECLARE_PROPERTIES(GridComponentState, WindowComponentState)
        DECLARE_PROPERTY(float, Scale, 1.0)
        DECLARE_PROPERTY(bool, EnableReticle, true)
        DECLARE_PROPERTY(bool, EnableGrid, true)
        DECLARE_PROPERTY(float, InternalScale, 1.0/4.0)
        DECLARE_PROPERTY(int, Len, 16)
    END_DECLARE_PROPERTIES()

public:
    GridComponentState();
    virtual ~GridComponentState() {}

    
};

class GridComponent : public Wnd::IWindowComponent
{

public:
    //! Constructor
    explicit GridComponent(Alloc::IAllocator* allocator);

    //! Destructor
    virtual ~GridComponent();

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
    virtual unsigned int GetUniqueId() const { return COMPONENT_GRID; }

private:
    Alloc::IAllocator* mAlloc;
    Pegasus::Shader::ProgramLinkageRef mGridProgram;
    Pegasus::Mesh::MeshRef mGrid;
    Pegasus::Mesh::MeshGeneratorRef mGridGenerator;
    Pegasus::Mesh::MeshRef mReticle;
    Pegasus::Shader::ProgramLinkageRef mReticleProgram;
    Pegasus::Mesh::MeshGeneratorRef mReticleGenerator;
    Pegasus::Render::RasterizerStateRef mRasterState;
    Pegasus::Render::RasterizerStateRef mReticleRasterState;
};

#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA

// Camera debug component
class CameraDebugComponentState : public Wnd::WindowComponentState
{
    BEGIN_DECLARE_PROPERTIES(CameraDebugComponentState, WindowComponentState)
        DECLARE_PROPERTY(bool, EnableDebug, false)
    END_DECLARE_PROPERTIES()

public:
    CameraDebugComponentState();
    virtual ~CameraDebugComponentState() {}

    
};

class CameraDebugComponent : public Wnd::IWindowComponent
{

public:
    //! Constructor
    explicit CameraDebugComponent(Alloc::IAllocator* allocator) : mAlloc(allocator) {}

    //! Destructor
    virtual ~CameraDebugComponent() {}

    //! Creation of a component state related to a window.
    virtual Wnd::WindowComponentState* CreateState(const Wnd::ComponentContext& context);

    //! Destruction of a component state related to a window.
    virtual void DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Load / create any rendering specific elements. Do not draw anything on the screen.
    virtual void Load(Core::IApplicationContext* appContext);

    //! Called once every tick by the app. Do any simulation operation on this function that is independant from a window.
    //! The order of layers will determine the order of execution of Update.
    virtual void Update(Core::IApplicationContext* appContext) {}

    //! Update on the window. Called once per window. Use this to update the internal state.
    //! \param context - context containing current window and app context
    //! \param state - state related to the window that is being updated.
    virtual void WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state) {}

    //! Called once for every window. 
    virtual void Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Shutdown the component. Destroy anything that was created in Load()
    virtual void Unload(Core::IApplicationContext* appContext) {}

    //! unique id
    virtual unsigned int GetUniqueId() const { return COMPONENT_DEBUG_CAMERA; }

private:
    Alloc::IAllocator* mAlloc;
    Pegasus::Shader::ProgramLinkageRef mCamDebugProgram;
    Pegasus::Mesh::MeshRef mCamMesh;
    Pegasus::Mesh::MeshGeneratorRef mGenerator;
    Pegasus::Shader::ProgramLinkageRef mDebugCamProgram;
    Pegasus::Render::RasterizerStateRef mDebugCamRasterState;

    Pegasus::Render::Uniform mCamUniform;
    Pegasus::Render::BufferRef mCamUniformBuffer;
};
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN
// Terrain debug component
class Terrain3dDebugComponentState : public Wnd::WindowComponentState
{
    BEGIN_DECLARE_PROPERTIES(Terrain3dDebugComponentState, WindowComponentState)
        DECLARE_PROPERTY(bool, EnableDebugGeometry, false)
        DECLARE_PROPERTY(bool, EnableDebugCameraCull, false)
    END_DECLARE_PROPERTIES()

public:
    Terrain3dDebugComponentState();
    virtual ~Terrain3dDebugComponentState() {}
};

class Terrain3dDebugComponent : public Wnd::IWindowComponent
{

public:
    //! Constructor
    explicit Terrain3dDebugComponent(Alloc::IAllocator* allocator) : mAlloc(allocator) {}

    //! Destructor
    virtual ~Terrain3dDebugComponent() {}

    //! Creation of a component state related to a window.
    virtual Wnd::WindowComponentState* CreateState(const Wnd::ComponentContext& context);

    //! Destruction of a component state related to a window.
    virtual void DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Load / create any rendering specific elements. Do not draw anything on the screen.
    virtual void Load(Core::IApplicationContext* appContext){}

    //! Called once every tick by the app. Do any simulation operation on this function that is independant from a window.
    //! The order of layers will determine the order of execution of Update.
    virtual void Update(Core::IApplicationContext* appContext) {}

    //! Update on the window. Called once per window. Use this to update the internal state.
    //! \param context - context containing current window and app context
    //! \param state - state related to the window that is being updated.
    virtual void WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state);

    //! Called once for every window. 
    virtual void Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state) {}

    //! Shutdown the component. Destroy anything that was created in Load()
    virtual void Unload(Core::IApplicationContext* appContext) {}

    //! unique id
    virtual unsigned int GetUniqueId() const { return COMPONENT_TERRAIN3D; }

private:
    Alloc::IAllocator* mAlloc;
};
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_LIGHTING

// Terrain debug component
class LightingDebugComponentState : public Wnd::WindowComponentState
{
    BEGIN_DECLARE_PROPERTIES(LightingDebugComponentState, WindowComponentState)
        DECLARE_PROPERTY(bool, DrawLightLocators, false)
        DECLARE_PROPERTY(bool, DrawLightInfluences, false)
    END_DECLARE_PROPERTIES()

public:
    LightingDebugComponentState();
    virtual ~LightingDebugComponentState() {}
};

class LightingDebugComponent : public Wnd::IWindowComponent
{

public:
    //! Constructor
    explicit LightingDebugComponent(Alloc::IAllocator* allocator) : mAlloc(allocator) {}

    //! Destructor
    virtual ~LightingDebugComponent() {}

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
    virtual unsigned int GetUniqueId() const { return COMPONENT_LIGHTING_DEBUG; }

private:
    Alloc::IAllocator* mAlloc;

    //light locator
    Mesh::MeshRef mLocatorMesh;
    Shader::ProgramLinkageRef mLocatorProgram;
    Render::Uniform mLocatorConstantUniform;
    Render::Uniform mLightBufferUniform;
    Render::BufferRef mLocatorConstantBuffer;
    struct LocatorConstants
    {
        Math::Mat44 billboard;
        Math::Mat44 viewProj;
        float eyeX;
        float eyeY;
        float eyeZ;
        float eyeW;
    };

    Render::RasterizerStateRef mRasterState;
    Render::BlendingStateRef mBlendState;


    Mesh::MeshRef mSphereLightMesh;
    Shader::ProgramLinkageRef mSphereLightProgram;
    Render::Uniform mSphereProgramLightBufferUniform;


    Mesh::MeshRef mSpotLightMesh;

};
#endif
}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif //EDITOR_COMPONENT_H

