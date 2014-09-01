/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   prettyprint.cpp
//! \author kleber garcia
//! \date   1st september 2014
//! \brief  blockscript ast tree pretty printer implementation

#include "Pegasus/BlockScript/PrettyPrint.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/bs.parser.hpp"
#include "Pegasus/Core/Assertion.h"

using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

void PrettyPrint::Indent()
{
    for (int i = 0; i < mScope; ++i)
    {
        mStr(" ");
    }
}

void PrettyPrint::Print(Program* p)
{
    mScope = 0;
    Visit(p);
    mStr("\n");
}

void PrettyPrint::Visit(Program* p)
{
    p->GetStmtList()->Access(this);
}


void PrettyPrint::Visit(Exp* n)
{
    PG_FAILSTR("unreachable node [Exp].");
}

void PrettyPrint::Visit(ExpList* n)
{
    if (n->GetExp() != nullptr)
    {
        n->GetExp()->Access(this);
        if (n->GetTail() != nullptr)
        {
            mStr(", ");
            n->GetTail()->Access(this);
        }
    }
}

void PrettyPrint::Visit(Stmt* n)
{
    PG_FAILSTR("unreachable node [Stmt].");
}

void PrettyPrint::Visit(StmtList* n)
{
    if (n->GetStmt() != nullptr)
    {
        Indent();
        n->GetStmt()->Access(this);
        mStr("\n");
        if (n->GetTail() != nullptr)
            n->GetTail()->Access(this);
    }
}

void PrettyPrint::Visit(ArgDec* n)
{
    mStr(n->GetVar()->GetName()); mStr(" : "); mStr(n->GetType()->GetName());
}

void PrettyPrint::Visit(ArgList* n)
{
    if (n->GetArgDec() != nullptr)
    {
        n->GetArgDec()->Access(this);
        if (n->GetTail() != nullptr)
        {
            mStr(", ");
            n->GetTail()->Access(this);
        }
    }
}

void PrettyPrint::Visit(Idd* n)
{
    mStr(n->GetName());
}

void PrettyPrint::Visit(Binop* n)
{
    n->GetLhs()->Access(this);
    switch(n->GetOp())
    {
    case O_PLUS: mStr(" + ");
        break;
    case O_MUL: mStr(" * ");
        break;
    case O_DIV: mStr(" / ");
        break;
    case O_MINUS: mStr(" - ");
        break;
    case O_SET: mStr(" = ");
        break;
    default:
        break;
    }
    n->GetRhs()->Access(this);
    
}

void PrettyPrint::Visit(FunCall* n)
{
    n->GetIdd()->Access(this);
    mStr("(");
    n->GetArgs()->Access(this);
    mStr(")");
}

void PrettyPrint::Visit(Imm* n)
{
    mInt(n->GetVariant().i[0]);
}

void PrettyPrint::Visit(StmtExp* n)
{
    n->GetExp()->Access(this);
    mStr(";");
}

void PrettyPrint::Visit(StmtTreeModifier* n)
{
    mStr(n->GetIdd()->GetName()); 
    mStr("->(");
    n->GetExpList()->Access(this);
    mStr(");");
}

void PrettyPrint::Visit(StmtFunDec* n)
{
    Indent();
    n->GetRet()->Access(this);
    mStr(" ");
    n->GetName()->Access(this);
    mStr("(");
    n->GetArgList()->Access(this);
    mStr(") {\n");
    ++mScope;
    n->GetStmtList()->Access(this);
    mStr("}\n");
    --mScope;
}

void PrettyPrint::Visit(StmtIfElse* n)
{
    PG_FAILSTR("not implemented yet.");
}

void PrettyPrint::Visit(StmtReturn* n)
{
    mStr("return ");
    n->GetExp()->Access(this);
    mStr(";");
}


