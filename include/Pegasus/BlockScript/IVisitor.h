/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IVisitor.h
//! \author Kleber Garcia
//! \date   1st September 2014
//! \brief  Blockscript ast tree visitor class

#ifndef PEGASUS_BLOCKSCRIPT_IVISITOR_H
#define PEGASUS_BLOCKSCRIPT_IVISITOR_H

namespace Pegasus
{
namespace BlockScript
{

namespace Ast
{
    //fwd declarations
    #define BS_PROCESS(N) class N;
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS
    class Node;
}

class IVisitor
{
public:
    IVisitor(){}
    virtual ~IVisitor(){}
    #define BS_PROCESS(N) virtual void Visit(Ast::N* n) = 0;
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS
};

};
}

#endif
