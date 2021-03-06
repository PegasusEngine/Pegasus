/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Texture.cpp
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Texture output node

#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/ITextureFactory.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Texture/Proxy/TextureNodeProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(Texture)
END_IMPLEMENT_PROPERTIES(Texture)

//----------------------------------------------------------------------------------------

Texture::Texture(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeManager, nodeAllocator, nodeDataAllocator)
,   mConfiguration()
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif  // PEGASUS_ENABLE_PROXIES
{
    BEGIN_INIT_PROPERTIES(Texture)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

Texture::Texture(Graph::NodeManager* nodeManager, const TextureConfiguration & configuration,
                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeManager, nodeAllocator, nodeDataAllocator)
,   mConfiguration(configuration)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif  // PEGASUS_ENABLE_PROXIES
{
    BEGIN_INIT_PROPERTIES(Texture)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
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
    if (textureData->IsGPUDataDirty())
    {
#if PEGASUS_ENABLE_DETAILED_LOG
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('TXTR', "Generating the GPU data of texture \"%s\"", GetName());
#else
        PG_LOG('TXTR', "Generating the GPU data of a texture");
#endif
#endif  // PEGASUS_ENABLE_DETAILED_LOG

        mFactory->GenerateTextureGPUData(&(*textureData));
    }
    return textureData;
}

//----------------------------------------------------------------------------------------

void Texture::ReleaseDataAndPropagate()
{
    //! \todo See note in ReleaseGPUData()
    ReleaseGPUData();

    Graph::Node::ReleaseDataAndPropagate();
}

//----------------------------------------------------------------------------------------

void Texture::ReleaseGPUData()
{
    //! \todo Investigate and optimize this function.
    //!       This function assumes node GPU data exists only once for the graph used by the texture.
    //!       This function can destroy GPU data of another graph sharing the same node.
    //!       GetUpdatedData() is called twice, and the first call might generate the data
    //!       of the graph that could have been empty, to release the content right after.

    bool dummyVariable = false;
    if (GetNumInputs() == 1 && GetInput(0)->GetUpdatedData(dummyVariable) != nullptr && mFactory != nullptr)
    {
#if PEGASUS_ENABLE_DETAILED_LOG
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('TXTR', "Destroying the GPU data of texture \"%s\"", GetName());
#else
        PG_LOG('TXTR', "Destroying the GPU data of a texture");
#endif
#endif  // PEGASUS_ENABLE_DETAILED_LOG

        mFactory->DestroyNodeGPUData((TextureData*)&(*GetInput(0)->GetUpdatedData(dummyVariable)));
    }
}

//----------------------------------------------------------------------------------------

Texture::~Texture()
{
    PEGASUS_EVENT_DESTROY_USER_DATA(&mProxy, "Texture", GetEventListener());

    ReleaseGPUData();
}


}   // namespace Texture
}   // namespace Pegasus
