/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LightingSystem.h
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  deferred renderer system definition file
                               
#ifndef PEGASUS_RENDER_SYSTEM_LIGHTING_H
#define PEGASUS_RENDER_SYSTEM_LIGHTING_H
#include "Pegasus/RenderSystems/Config.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_LIGHTING

#include "Pegasus/RenderSystems/System/RenderSystem.h"
#include "Pegasus/Render/Render.h"

namespace Pegasus 
{
namespace RenderSystems
{

//! LightingSystem system implementation. Adds a blockscript library.
class LightingSystem : public RenderSystem
{
public:
    //! Constructor
    explicit LightingSystem(Alloc::IAllocator* allocator) : RenderSystem(allocator), mIsLightBufferDirty(true) {}

    //! destructor
    virtual ~LightingSystem() {}

    virtual void Load(Core::IApplicationContext* appContext);

    virtual bool CanCreateBlockScriptApi() const { return true; }

    virtual const char* GetSystemName() const { return "DeferredRendererSystem"; }

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);

    virtual void WindowUpdate(unsigned int width, unsigned int height);

private:
    bool mIsLightBufferDirty;
    Render::BufferRef mLightBuffer;

};
}
}

#endif
#endif
