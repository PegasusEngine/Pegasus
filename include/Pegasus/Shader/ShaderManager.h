#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/Shader/ProgramLinkage.h"


namespace Pegasus
{
namespace Graph
{
    class NodeManager;
}

namespace Shader
{

class IEventListener;

class ShaderManager
{
public:
    explicit ShaderManager(Graph::NodeManager * nodeManager);
    virtual ~ShaderManager();

    ProgramLinkageRef CreateProgram() const;

private:
    void RegisterAllNodes();

    Graph::NodeManager * mNodeManager;
    
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
