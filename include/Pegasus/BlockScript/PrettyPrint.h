/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   prettyprint.h
//! \author Kleber Garcia
//! \date   1st september 2014
//! \brief  blockscript ast tree pretty printer.
 
#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include "Pegasus/BlockScript/IVisitor.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/BlockScriptCanon.h"
#include "Pegasus/BlockScript/FunCallback.h"

namespace Pegasus
{
namespace BlockScript
{

class TypeDesc;
struct Assembly;

class PrettyPrint : public IVisitor
{
public:
    PrettyPrint(PrintStringCallbackType str, PrintIntCallbackType pint, PrintFloatCallbackType pfloat) :mStr(str), mInt(pint), mFloat(pfloat), mScope(0) {}
    virtual ~PrettyPrint(){}    
    
    void Print(Ast::Program* p);
    
    void PrintAsm(Assembly& assembly);


private:
    #define BS_PROCESS(N) virtual void Visit(Ast::N* n);
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS

    void PrintBlock(Canon::Block& b);

    void PrintRegister(Canon::Register r);

    void PrintType(const TypeDesc* type);

    void Indent();

    PrintStringCallbackType mStr;
    PrintIntCallbackType    mInt;
    PrintFloatCallbackType  mFloat;
    int mScope;
};



}
}

#endif
