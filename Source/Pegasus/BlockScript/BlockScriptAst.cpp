#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/Utils/String.h"


using namespace Pegasus::Utils;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

void Visitor::StartVisit(Node* n)
{
    n->Access(this);
}
