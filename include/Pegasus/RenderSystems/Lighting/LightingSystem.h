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

#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/RenderSystems/Lighting/LightRig.h"
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
    explicit LightingSystem(Alloc::IAllocator* allocator) : RenderSystem(allocator), mCachedGpuLightBufferVersion(0) {}

    //! destructor
    virtual ~LightingSystem() {}

    virtual void Load(Core::IApplicationContext* appContext);

    virtual bool CanCreateBlockScriptApi() const { return true; }

    virtual const char* GetSystemName() const { return "LightingSystem"; }

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);

    virtual void WindowUpdate(unsigned int width, unsigned int height);

    //functions of the lighting system
    void SetActiveLightRig(Lighting::LightRigRef& lightRig)
    {
        if (mActiveLightRig != lightRig)
        {
            mCachedGpuLightBufferVersion = 0;
            mActiveLightRig = lightRig;
        }
    }

    //function of the
    Render::BufferRef GetCulledLightBuffer() { return mLightBuffer; }

    int GetActiveLightCount() const;

private:
    int mCachedGpuLightBufferVersion;
    Render::BufferRef mLightBuffer;
    Lighting::LightRigRef mActiveLightRig;

};
}
}

#endif
#endif
