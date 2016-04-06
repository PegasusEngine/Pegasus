/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureNodeProxy.cpp
//! \author	Karolyn Boulanger
//! \date	08th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture node

//! \todo Why do we need this in Rel-Debug? TextureProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Proxy/TextureNodeProxy.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"
#include "Pegasus/Texture/Texture.h"

namespace Pegasus {
namespace Texture {


TextureNodeProxy::TextureNodeProxy(TextureGenerator* textureGenerator)
:   mNodeType(NODETYPE_GENERATOR)
,   mTextureGeneratorNode(textureGenerator)
{
    PG_ASSERTSTR(textureGenerator != nullptr, "Trying to create a texture node proxy from an invalid texture generator object");
}

//----------------------------------------------------------------------------------------

TextureNodeProxy::TextureNodeProxy(TextureOperator * textureOperator)
:   mNodeType(NODETYPE_OPERATOR)
,   mTextureOperatorNode(textureOperator)
{
    PG_ASSERTSTR(textureOperator != nullptr, "Trying to create a texture node proxy from an invalid texture operator object");
}

//----------------------------------------------------------------------------------------

TextureNodeProxy::TextureNodeProxy(Texture * texture)
:   mNodeType(NODETYPE_OUTPUT)
,   mTextureNode(texture)
{
    PG_ASSERTSTR(texture != nullptr, "Trying to create a texture node proxy from an invalid texture object");
}

//----------------------------------------------------------------------------------------

TextureNodeProxy::~TextureNodeProxy()
{
}

//----------------------------------------------------------------------------------------

TextureGenerator* TextureNodeProxy::GetTextureGenerator() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_GENERATOR, "Trying to get a texture node of the wrong type");
    return mTextureGeneratorNode;
}

//----------------------------------------------------------------------------------------

TextureOperator* TextureNodeProxy::GetTextureOperator() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_OPERATOR, "Trying to get a texture node of the wrong type");
    return mTextureOperatorNode;
}

//----------------------------------------------------------------------------------------

Texture* TextureNodeProxy::GetTexture() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_OUTPUT, "Trying to get a texture node of the wrong type");
    return mTextureNode;
}

//----------------------------------------------------------------------------------------

const char* TextureNodeProxy::GetName() const
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            return mTextureGeneratorNode->GetName();

        case NODETYPE_OPERATOR:
            return mTextureOperatorNode->GetName();

        case NODETYPE_OUTPUT:
            return mTextureNode->GetName();

        default:
            PG_FAILSTR("Trying to get the name of an unknown texture node type");
            return nullptr;
    }
}

//----------------------------------------------------------------------------------------

const ITextureConfigurationProxy* TextureNodeProxy::GetConfiguration() const
{
    if (mNodeType == NODETYPE_OUTPUT)
    {
        return mTextureNode->GetConfiguration().GetProxy();
    }
    else
    {
        PG_FAILSTR("Trying to get the configuration of a non-output texture node");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

unsigned int TextureNodeProxy::GetNumInputs() const
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            PG_ASSERTSTR(mTextureGeneratorNode->GetNumInputs() == 0, "A texture generator node is supposed to have no input");
            return mTextureGeneratorNode->GetNumInputs();

        case NODETYPE_OPERATOR:
            return mTextureOperatorNode->GetNumInputs();

        case NODETYPE_OUTPUT:
            PG_ASSERTSTR(mTextureGeneratorNode->GetNumInputs() <= 1, "A texture output node is supposed to have at most one input");
            return mTextureNode->GetNumInputs();

        default:
            PG_FAILSTR("Trying to get the number of inputs of an unknown texture node type");
            return 0;
    }
}

//----------------------------------------------------------------------------------------

ITextureNodeProxy* TextureNodeProxy::GetInputNode(unsigned int index)
{
    Graph::NodeRef inputNode;
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            PG_FAILSTR("Trying to retrieve an input from a texture generator node, but it cannot have inputs");
            break;

        case NODETYPE_OPERATOR:
            if (mTextureOperatorNode->IsInputIndexValid(index))
            {
                inputNode = mTextureOperatorNode->GetInput(index);
            }
            break;

        case NODETYPE_OUTPUT:
            if (mTextureNode->IsInputIndexValid(index))
            {
                inputNode = mTextureNode->GetInput(index);
            }
            break;

        default:
            PG_FAILSTR("Trying to get an input of an unknown texture node type");
    }

    if (inputNode != nullptr)
    {
        switch (inputNode->GetNodeType())
        {
            case Graph::Node::NODETYPE_GENERATOR:
                return static_cast<TextureGenerator *>(inputNode)->GetProxy();

            case Graph::Node::NODETYPE_OPERATOR:
                return static_cast<TextureOperator *>(inputNode)->GetProxy();

            case Graph::Node::NODETYPE_OUTPUT:
                PG_FAILSTR("Invalid node type, an input node must be a generator or an operator, not an output");
                return nullptr;

            default:
                PG_FAILSTR("Invalid node type, it must be a generator or an operator");
                return nullptr;
        }
    }
    else
    {
        PG_FAILSTR("Invalid input index (%d) to access a node, it must be < %d", index, GetNumInputs());
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

PropertyGrid::IPropertyGridObjectProxy* TextureNodeProxy::GetPropertyGridObjectProxy() const
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            return mTextureGeneratorNode->GetPropertyGridProxy();

        case NODETYPE_OPERATOR:
            return mTextureOperatorNode->GetPropertyGridProxy();

        case NODETYPE_OUTPUT:
            return mTextureNode->GetPropertyGridProxy();

        default:
            PG_FAILSTR("Trying to get the property grid object of an unknown texture node type");
            return nullptr;
    }
}

//----------------------------------------------------------------------------------------

AssetLib::IRuntimeAssetObjectProxy* TextureNodeProxy::GetDecoratedObject() const
{
    if (mNodeType == NODETYPE_OUTPUT)
    {
        return mTextureNode->GetRuntimeAssetObjectProxy();
    }
    else
    {
        PG_FAILSTR("Trying to get the decorated object of a non-output texture node");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void TextureNodeProxy::SetEventListener(ITextureNodeEventListener* listener)
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            mTextureGeneratorNode->SetEventListener(listener);
            break;

        case NODETYPE_OPERATOR:
            mTextureOperatorNode->SetEventListener(listener);
            break;

        case NODETYPE_OUTPUT:
            mTextureNode->SetEventListener(listener);
            break;

        default:
            PG_FAILSTR("Trying to set the event listener of an unknown texture node type");
    }
}

//----------------------------------------------------------------------------------------

void TextureNodeProxy::SetUserData(Core::IEventUserData* userData)
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            mTextureGeneratorNode->SetEventUserData(userData);
            break;

        case NODETYPE_OPERATOR:
            mTextureOperatorNode->SetEventUserData(userData);
            break;

        case NODETYPE_OUTPUT:
            mTextureNode->SetEventUserData(userData);
            break;

        default:
            PG_FAILSTR("Trying to set the event user data of an unknown texture node type");
    }
}

//----------------------------------------------------------------------------------------

Core::IEventUserData* TextureNodeProxy::GetUserData() const
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            return mTextureGeneratorNode->GetEventUserData();

        case NODETYPE_OPERATOR:
            return mTextureOperatorNode->GetEventUserData();

        case NODETYPE_OUTPUT:
            return mTextureNode->GetEventUserData();

        default:
            PG_FAILSTR("Trying to get the event user data of an unknown texture node type");
            return nullptr;
    }
}

//----------------------------------------------------------------------------------------

PropertyGrid::IPropertyGridObjectProxy* TextureNodeProxy::GetPropertyGrid()
{
    switch (mNodeType)
    {
        case NODETYPE_GENERATOR:
            return mTextureGeneratorNode->GetPropertyGridProxy();

        case NODETYPE_OPERATOR:
            return mTextureOperatorNode->GetPropertyGridProxy();

        case NODETYPE_OUTPUT:
            return mTextureNode->GetPropertyGridProxy();

        default:
            PG_FAILSTR("Trying to get the event user data of an unknown texture node type");
            return nullptr;
    }
}

}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
