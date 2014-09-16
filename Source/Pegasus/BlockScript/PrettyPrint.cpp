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
        if (n->GetTail() != nullptr)
        {
            mStr("\n");
            n->GetTail()->Access(this);
        }
    }
}

void PrettyPrint::Visit(ArgDec* n)
{
    mStr(n->GetVar()); mStr(" : "); mStr(n->GetType());
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
	mStr(n->GetName());
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
    Indent();
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

void PrettyPrint::Visit(StmtWhile* n)
{
    Indent();
    mStr("while ( ");
    n->GetExp()->Access(this);
    mStr(") {\n");
    ++mScope;
    n->GetStmtList()->Access(this);
    --mScope;
    mStr("\n");
    Indent();
    mStr("}\n");
}

void PrettyPrint::Visit(StmtFunDec* n)
{
    Indent();
    mStr(n->GetRet());
    mStr(" ");
    mStr(n->GetName());
    mStr("(");
    n->GetArgList()->Access(this);
	Ast::StmtList* sl = n->GetStmtList();
    if (sl == nullptr)
    {
        mStr(") ;");
    }
	else
	{
        mStr(") {\n");
        ++mScope;
        n->GetStmtList()->Access(this);
        mStr("\n}\n");
        --mScope;
	}
}

void PrettyPrint::Visit(StmtIfElse* n)
{
    Indent();
    mStr("if (");
    n->GetExp()->Access(this);
    mStr(") {\n");
    ++mScope;
    n->GetStmtList()->Access(this);
    --mScope;
    mStr("\n");
    Indent();
    mStr("}\n");
    if (n->GetTail() != nullptr)
    {
        n->GetTail()->Access(this);
    }
}

void PrettyPrint::Visit(ElseIfTail* n)
{
    Indent();
    mStr("elif ( ");
    n->GetExp()->Access(this);
    mStr(") {\n");
    ++mScope;
    n->GetStmtList()->Access(this);
    --mScope;
    mStr("\n");
    Indent();
    mStr("}\n");
    if (n->GetTail() != nullptr)
    {
        n->GetTail()->Access(this);
    }
}

void PrettyPrint::Visit(ElseTail* n)
{
    Indent();
    mStr("else {\n");
    ++mScope;
    n->GetStmtList()->Access(this);
    --mScope;
    mStr("\n");
    Indent();
    mStr("}\n");
}

void PrettyPrint::Visit(StmtReturn* n)
{
    mStr("return ");
    n->GetExp()->Access(this);
    mStr(";");
}


