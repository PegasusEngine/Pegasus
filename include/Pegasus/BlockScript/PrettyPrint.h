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

namespace Pegasus
{
namespace BlockScript
{

class PrettyPrint : public IVisitor
{
public:

typedef void (*PrintStringCallback)(const char *);
typedef void (*PrintIntCallback)(int);
typedef void (*PrintFloatCallback)(float);

PrettyPrint(PrintStringCallback str, PrintIntCallback pint, PrintFloatCallback pfloat) :mStr(str), mInt(pint), mFloat(pfloat), mScope(0) {}
virtual ~PrettyPrint(){}    

void Print(Ast::Program* p);


private:
    #define BS_PROCESS(N) virtual void Visit(Ast::N* n);
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS

    void Indent();

    PrintStringCallback mStr;
    PrintIntCallback    mInt;
    PrintFloatCallback  mFloat;
    int mScope;
};



}
}

#endif
