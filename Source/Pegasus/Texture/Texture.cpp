/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Texture.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture output node

#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/ITextureFactory.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Texture/TextureProxy.h"
#include "Pegasus/Texture/TextureTracker.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


Texture::Texture(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
{
#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the texture
    mProxy = PG_NEW(nodeAllocator, -1, "Texture::Texture::mProxy", Pegasus::Alloc::PG_MEM_PERM) TextureProxy(this);
    mTracker = nullptr;
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Texture::Texture(const TextureConfiguration & configuration,
                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the texture
    mProxy = PG_NEW(nodeAllocator, -1, "Texture::Texture::mProxy", Pegasus::Alloc::PG_MEM_PERM) TextureProxy(this);
    mTracker = nullptr;
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

void Texture::SetConfiguration(const TextureConfiguration & configuration)
{
    if (GetNumInputs() == 0)
    {
        mConfiguration = configuration;
    }
    else
    {
        PG_FAILSTR("Cannot set the configuration of a texture because the node is already in use");
    }
}

//----------------------------------------------------------------------------------------

void Texture::SetGeneratorInput(TextureGeneratorIn textureGenerator)
{
    // Check that the configuration is compatible
    if (textureGenerator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        RemoveAllInputs();
        AddInput(textureGenerator);
    }
    else
    {
        PG_FAILSTR("Unable to set the generator input of a Texture node since their configurations are incompatible");
    }
}

//----------------------------------------------------------------------------------------

void Texture::SetOperatorInput(TextureOperatorIn textureOperator)
{
    // Check that the configuration is compatible
    if (textureOperator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        RemoveAllInputs();
        AddInput(textureOperator);
    }
    else
    {
        PG_FAILSTR("Unable to set the operator input of a Texture node since their configurations are incompatible");
    }
}

//----------------------------------------------------------------------------------------

TextureDataReturn Texture::GetUpdatedTextureData()
{
    PG_ASSERT(mFactory);
    bool updated = false;
    TextureDataRef textureData = Graph::OutputNode::GetUpdatedData(updated);
    if (updated)
    {
        mFactory->GenerateTextureGPUData(&(*textureData));
    }
    return textureData;
}

//----------------------------------------------------------------------------------------

void Texture::ReleaseDataAndPropagate()
{
    ReleaseGPUData();
    Graph::Node::ReleaseDataAndPropagate();
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
void Texture::SetTracker(TextureTracker * tracker)
{
    PG_ASSERTSTR(tracker != nullptr, "Invalid tracker given to a tracker");
    mTracker = tracker;
}
#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

void Texture::ReleaseGPUData()
{
    bool dummyVariable = false;
    if (GetNumInputs() == 1 && GetInput(0)->GetUpdatedData(dummyVariable) != nullptr && mFactory != nullptr)
    {
        mFactory->DestroyNodeGPUData((TextureData*)&(*GetInput(0)->GetUpdatedData(dummyVariable)));
    }
}

//----------------------------------------------------------------------------------------

Texture::~Texture()
{
    ReleaseGPUData();

#if PEGASUS_ENABLE_PROXIES
    // Unregister the texture from the tracker
    if (mTracker != nullptr)
    {
        mTracker->DeleteTexture(this);
    }
    else
    {
        PG_FAILSTR("Trying to delete a texture that has no associated tracker");
    }

    //! Destroy the proxy associated with the texture
    PG_DELETE(GetNodeAllocator(), mProxy);
#endif
}


}   // namespace Texture
}   // namespace Pegasus
