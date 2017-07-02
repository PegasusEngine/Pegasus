/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LightRig.h
//! \author Kleber Garcia
//! \date   June 25, 2017
//! \brief  Light rig resource

#ifndef PEGASUS_RENDER_LIGHTRIG
#define PEGASUS_RENDER_LIGHTRIG 
#include "Pegasus/Core/Ref.h"
#include "Pegasus/RenderSystems/Config.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Math/Vector.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_LIGHTING

namespace Pegasus
{

namespace Lighting
{

typedef int LightHandle;
const LightHandle InvalidLightHandle = -1;

class LightRig : public Application::GenericResource
{
    BEGIN_DECLARE_PROPERTIES(LightRig, GenericResource)
    END_DECLARE_PROPERTIES()

public:

    // Must match definitions in shader header LightingCore.h
    enum LightType
    {
        LIGHT_TYPE_BEGIN = 0,
        SPHERE = LIGHT_TYPE_BEGIN,
        SPOT,
        LIGHT_TYPE_END
    };

    enum Constants
    {
        MaxLightsCount = 256
    };

    struct LightBase
    {
        Math::Vec4 colorAndIntensity;
        LightBase() : colorAndIntensity(1.0f,1.0f,1.0f,1.0f) {}
    };

    struct SphereLight : public LightBase
    {
        Math::Vec4 posAndRadius;
        SphereLight() : posAndRadius(Math::Vec4(0.0f, 0.0f, 0.0f, 1.0f)) {}
    };

    struct SpotLight : public SphereLight
    {
        Math::Vec4 dirAndSpread;
        SpotLight() : dirAndSpread(Math::Vec4(0.0f, 1.0f, 0.0f, 0.5)) {}
    };

    union LightState
    {
        LightBase   base;
        SphereLight sphere;
        SpotLight   spot;
        LightState() {}
    };

    struct Light
    {
        LightType type;
        LightState state;
        Light() : type(SPHERE) {}

        void SetState(SphereLight& sl)
        {
            type = SPHERE;
            state.sphere = sl;
        }

        void SetState(SpotLight& sl)
        {
            type = SPOT;
            state.spot = sl;
        }
    };

    //! Constructor
    LightRig(Alloc::IAllocator* allocator);

    //! Destructor
    ~LightRig();

    //! updates corresponding light rig data.
    virtual void Update();

    //Adds a new light, and returns a handle to access it.
    LightHandle AddLight(const Light& light);

    //Updates the state of a light
    void UpdateLight(LightHandle lightHandle, const Light& state);

    int GetLightCount() const { return mLightListCount; }

    // Clear the entire light rig 
    void Clear();

    struct GpuLight
    {
        Math::Vec4 attr0;
        Math::Vec4 attr1;
        Math::Vec4 attr2;
        int        attr3;
        int        attr4;
    };

    struct GpuLightBuffer
    {
        GpuLight gpuLights[MaxLightsCount];
    };

    const GpuLightBuffer& GetGpuBuffer(unsigned int& bufferByteSize, int& version) const;
    
private:
    Alloc::IAllocator* mAllocator;
    Light mLightList[MaxLightsCount];
    int mLightListCount;
    int mLightBufferVersion;

    GpuLightBuffer mLightBuffer;
    bool mIsLightBufferDirty;
    int mGpuLightCount;

};

typedef Pegasus::Core::Ref<LightRig> LightRigRef;

}
}

#endif

#endif
