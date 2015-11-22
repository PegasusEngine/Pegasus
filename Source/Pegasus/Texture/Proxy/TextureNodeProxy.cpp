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


TextureNodeProxy::TextureNodeProxy(TextureGenerator * textureGenerator)
:   mNodeType(NODETYPE_GENERATOR)
,   mNode(textureGenerator)
{
    PG_ASSERTSTR(textureGenerator != nullptr, "Trying to create a texture node proxy from an invalid texture generator object");
}

//----------------------------------------------------------------------------------------

TextureNodeProxy::TextureNodeProxy(TextureOperator * textureOperator)
:   mNodeType(NODETYPE_OPERATOR)
,   mNode(textureOperator)
{
    PG_ASSERTSTR(textureOperator != nullptr, "Trying to create a texture node proxy from an invalid texture operator object");
}

//----------------------------------------------------------------------------------------

TextureNodeProxy::TextureNodeProxy(Texture * texture)
:   mNodeType(NODETYPE_OUTPUT)
,   mNode(texture)
{
    PG_ASSERTSTR(texture != nullptr, "Trying to create a texture node proxy from an invalid texture object");
}

//----------------------------------------------------------------------------------------

TextureNodeProxy::~TextureNodeProxy()
{
}

//----------------------------------------------------------------------------------------

TextureGenerator * TextureNodeProxy::GetTextureGenerator() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_GENERATOR, "Trying to get a texture node of the wrong type");
    return static_cast<TextureGenerator *>(mNode);
}

//----------------------------------------------------------------------------------------

TextureOperator * TextureNodeProxy::GetTextureOperator() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_OPERATOR, "Trying to get a texture node of the wrong type");
    return static_cast<TextureOperator *>(mNode);
}

//----------------------------------------------------------------------------------------

Texture * TextureNodeProxy::GetTexture() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_OUTPUT, "Trying to get a texture node of the wrong type");
    return static_cast<Texture *>(mNode);
}

//----------------------------------------------------------------------------------------

const char * TextureNodeProxy::GetName() const
{
    return mNode->GetName();
}

//----------------------------------------------------------------------------------------

const ITextureConfigurationProxy * TextureNodeProxy::GetConfiguration() const
{
    PG_ASSERTSTR(mNodeType == NODETYPE_OUTPUT, "Trying to get the configuration of a non-output texture node");
    return static_cast<Texture *>(mNode)->GetConfiguration().GetProxy();
}

//----------------------------------------------------------------------------------------

unsigned int TextureNodeProxy::GetNumInputs() const
{
    return mNode->GetNumInputs();
}

//----------------------------------------------------------------------------------------

ITextureNodeProxy * TextureNodeProxy::GetInputNode(unsigned int index)
{
    if (mNode->IsInputIndexValid(index))
    {
        Graph::NodeRef inputNode = mNode->GetInput(index);
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
        PG_FAILSTR("Invalid input index (%d) to access a node, it must be < %d", index, mNode->GetNumInputs());
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

PropertyGrid::IPropertyGridObjectProxy * TextureNodeProxy::GetPropertyGridObjectProxy() const
{
    return mNode->GetPropertyGridProxy();
}


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
