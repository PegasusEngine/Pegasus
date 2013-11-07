/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureManager.cpp
//! \author	Kevin Boulanger
//! \date	03rd November 2013
//! \brief	Global texture node manager, including the factory features

#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Graph/NodeManager.h"

#include "Pegasus/Texture/Generator/ConstantColorGenerator.h"

#include "Pegasus/Texture/Operator/AddOperator.h"

namespace Pegasus {
namespace Texture {


//! Macro to register a texture node, used only in the \a RegisterAllTextureNodes() function
//! \param className Class name of the texture node to register
#define REGISTER_TEXTURE_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode)

//----------------------------------------------------------------------------------------
    
TextureManager::TextureManager(Graph::NodeManager * nodeManager)
:   mNodeManager(nodeManager)
{
    if (nodeManager != nullptr)
    {
        RegisterAllTextureNodes();
    }
    else
    {
        PG_FAILSTR("Invalid node manager given to the texture manager");
    }
}

//----------------------------------------------------------------------------------------

TextureManager::~TextureManager()
{
}

//----------------------------------------------------------------------------------------

void TextureManager::RegisterTextureNode(const char * className, Graph::Node::CreateNodeFunc createNodeFunc)
{
    if (mNodeManager != nullptr)
    {
        mNodeManager->RegisterNode(className, createNodeFunc);
    }
    else
    {
        PG_FAILSTR("Unable to register a texture node because the texture manager is not linked to the node manager");
    }
}

//----------------------------------------------------------------------------------------

TextureReturn TextureManager::CreateTextureNode(const TextureConfiguration & configuration)
{
    if (mNodeManager != nullptr)
    {
        TextureRef texture = mNodeManager->CreateNode("Texture");
        texture->SetConfiguration(configuration);
        return texture;
    }
    else
    {
        PG_FAILSTR("Unable to create a texture node because the texture manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

TextureGeneratorReturn TextureManager::CreateTextureGeneratorNode(const char * className,
                                                                  const TextureConfiguration & configuration)
{
    if (mNodeManager != nullptr)
    {
        //! \todo Check that the class corresponds to a generator texture

        TextureGeneratorRef textureGenerator = mNodeManager->CreateNode(className);
        textureGenerator->SetConfiguration(configuration);
        return textureGenerator;
    }
    else
    {
        PG_FAILSTR("Unable to create a generator texture node because the texture manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

TextureOperatorReturn TextureManager::CreateTextureOperatorNode(const char * className,
                                                                const TextureConfiguration & configuration)
{
    if (mNodeManager != nullptr)
    {
        //! \todo Check that the class corresponds to an operator texture

        TextureOperatorRef textureOperator = mNodeManager->CreateNode(className);
        textureOperator->SetConfiguration(configuration);
        return textureOperator;
    }
    else
    {
        PG_FAILSTR("Unable to create an operator texture node because the texture manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void TextureManager::RegisterAllTextureNodes()
{
    PG_ASSERTSTR(mNodeManager != nullptr, "Enable to register the texture nodes because the texture manager is not linke to the node manager");

    // Register the output texture node
    REGISTER_TEXTURE_NODE(Texture);

    // Register the generator nodes
    // IMPORTANT! Add here every texture generator node that is created,
    //            and update the list of #includes above
    REGISTER_TEXTURE_NODE(ConstantColorGenerator);

    // Register the operator nodes
    // IMPORTANT! Add here every texture operator node that is created
    //            and update the list of #includes above
    REGISTER_TEXTURE_NODE(AddOperator);
}


}   // namespace Texture
}   // namespace Pegasus