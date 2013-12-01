/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderManager.cpp
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Main shader interface

#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Graph/NodeManager.h"


#define REGISTER_SHADER_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode);

Pegasus::Shader::ShaderManager::ShaderManager(Pegasus::Graph::NodeManager * nodeManager)
:
mNodeManager(nodeManager)
{
    if (nodeManager != nullptr)
    {
        RegisterAllNodes();
    }
    else
    {
        PG_FAILSTR("Invalid node manager given to ShaderManager");
    }
}

Pegasus::Shader::ShaderManager::~ShaderManager()
{
}

void Pegasus::Shader::ShaderManager::RegisterAllNodes()
{
    PG_ASSERTSTR(mNodeManager != nullptr, "Enable to register the shader nodes because the texture manager is not linke to the node manager");

    REGISTER_SHADER_NODE(ProgramLinkage);
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram() const
{
    return mNodeManager->CreateNode("ProgramLinkage");
}
