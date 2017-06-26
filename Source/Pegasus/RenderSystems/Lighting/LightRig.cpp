/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LightRig.cpp
//! \author Kleber Garcia
//! \date   June 25, 2017
//! \brief  Light rig resource

#include "Pegasus/RenderSystems/Lighting/LightRig.h"

using namespace Pegasus;
using namespace Pegasus::Lighting;

BEGIN_IMPLEMENT_PROPERTIES(LightRig)
END_IMPLEMENT_PROPERTIES(LightRig)

LightRig::LightRig(Alloc::IAllocator* allocator)
: Pegasus::Application::GenericResource(allocator), mAllocator(allocator), mLightListCount(0),mIsLightBufferDirty(false), mGpuLightCount(0)
{

}

LightRig::~LightRig()
{
}

void LightRig::Update()
{
    if (mIsLightBufferDirty)
    {
        mGpuLightCount = 0;
        //convert lights to gpu state
        for (int i = 0; i < mLightListCount; ++i)
        {
            const Light& source = mLightList[i];
            if (source.type >= LIGHT_TYPE_BEGIN || source.type < LIGHT_TYPE_END)
            {
                GpuLight& destination = mLightBuffer.gpuLights[mGpuLightCount++];
                destination.attr0 = source.state.base.colorAndIntensity;
                destination.attr3 = static_cast<int>(source.type);
                switch (source.type)
                {
                case SPOT:
                    destination.attr1 = source.state.spot.posAndRadius;
                    destination.attr2 = source.state.spot.dirAndSpread;
                    break;
                case SPHERE:
                    destination.attr1 = source.state.sphere.posAndRadius;
                    break;
                }
            }
            else
            {
                PG_FAILSTR("Invalid light type set.");
            }
        }
        mIsLightBufferDirty = false;
    }
}

LightHandle LightRig::AddLight(const Light& light)
{
    if (mLightListCount >= MaxLightsCount)
    {
        PG_FAILSTR("Maximum lightcount reached. Cannot add a light to light rig.");
        return  InvalidLightHandle;
    }
    LightHandle h = mLightListCount;
    mLightList[mLightListCount++];
    mIsLightBufferDirty = true;
    return h;
}

void LightRig::Clear()
{
    mLightListCount = 0;
    mIsLightBufferDirty = true;
}

void LightRig::UpdateLight(LightHandle handle, const Light& state)
{
    if (handle >= 0  && handle < mLightListCount)
    {
        mLightList[handle] = state;
        mIsLightBufferDirty = true;
    }
    else
    {
        PG_FAILSTR("Invalid light handle passed to update light state.");
    }
}

const LightRig::GpuLightBuffer& LightRig::GetGpuBuffer(unsigned int& lightCount, unsigned int& bufferByteSize) const
{
    lightCount = mGpuLightCount;
    bufferByteSize = mGpuLightCount * sizeof(GpuLight);
    return mLightBuffer;
}
