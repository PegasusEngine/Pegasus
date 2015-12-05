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
#include "Pegasus/BlockScript/StackFrameInfo.h"
#include "Pegasus/BlockScript/bs.parser.hpp"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/Canonizer.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/Vector.h"

using namespace Pegasus::Utils;
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

void PrettyPrint::PrintAsm(Assembly& assembly)
{
    Container<Canon::Block>* blockList = assembly.mBlocks;
    if (blockList->Size() > 0)
    {
        mScope = 0;
        int currBlock = 0;
        while (currBlock != -1)
        {
            Canon::Block& b = (*blockList)[currBlock];
            mStr("LABEL ");
            mInt(b.GetLabel());
            mStr(":\n");
            ++mScope;
            
            PrintBlock(b);
            
            --mScope;
            currBlock = b.NextBlock();
        }
    }
}

void PrettyPrint::PrintRegister(Canon::Register r)
{
    switch(r)
    {
    case Canon::R_RET: // return register
        mStr("$ret");break;
    case Canon::R_G:   // general purpose 1
        mStr("$g");break;
    case Canon::R_A:   // general purpose 2
        mStr("$a");break;
    case Canon::R_B:   // general purpose 2
        mStr("$b");break;
    case Canon::R_C:   // general purpose 3
        mStr("$c");break;
    case Canon::R_IP:  // instruction register
        mStr("$ip");break;
    case Canon::R_SBP: // base stack pointer register
        mStr("$ebp");break;
    case Canon::R_ESP:  // top of stack pointer register
        mStr("$esp");break;
    }
}

void PrettyPrint::PrintBlock(Canon::Block& b)
{
    Container<Canon::CanonNode*>& canons = b.GetStmts();
    int numCanons = canons.Size();
    for (int c = 0; c < numCanons; ++c)
    {
        Canon::CanonNode* node = canons[c];
        Canon::CanonTypes nodeType = node->GetType();
        switch(nodeType)
        {
        case Canon::T_MOVE:
            {
                Canon::Move* mov = static_cast<Canon::Move*>(node);
                Indent(); mStr("MOVE ");
                mStr("[");
                mInt(mov->GetLhs()->GetOffset());
                mStr("] {");
                mov->GetRhs()->Access(this);
                mStr("}\n");
            }
            break;
        case Canon::T_INSERT_DATA_TO_HEAP:
            {
                Canon::InsertDataToHeap* isdh = static_cast<Canon::InsertDataToHeap*>(node);
                Indent();
                mStr("INSERT_DATA_TO_HEAP ");
                isdh->GetTmp()->Access(this);
                mStr(" <native-pointer>\n");
            }
            break;
        case Canon::T_SAVE:
            {
                Canon::Save* save = static_cast<Canon::Save*>(node);
                Indent();
                mStr("SAVE ");
                save->GetTmp()->Access(this);
                mStr(" ");
                PrintRegister(save->GetRegister());
                mStr("\n");
            }
            break;
        case Canon::T_LOAD:
            {
                Canon::Load* load = static_cast<Canon::Load*>(node);
                Indent();
                mStr("LOAD ");
                PrintRegister(load->GetRegister());
                mStr(" ");
                load->GetExp()->Access(this);
                mStr("\n");
            }
            break;
        case Canon::T_JMP:
            {
                Canon::Jmp* jmp = static_cast<Canon::Jmp*>(node);
                Indent();
                mStr("JMP ");
                mInt(jmp->GetLabel());
                mStr("\n");
            }
            break;
        case Canon::T_JMPCOND:
            {
                Canon::JmpCond* jmp = static_cast<Canon::JmpCond*>(node);
                Indent();
                mStr("JMPCOND { ");
                jmp->GetExp()->Access(this);
                mStr(" == ");
                mInt(jmp->GetComparison());
                mStr(" } ");
                mInt(jmp->GetLabel());
                mStr("\n");
            }
            break;
        case Canon::T_RET:
            {
                Indent();
                mStr("RET\n");
            }
            break;
        case Canon::T_PUSHFRAME:
            {
                Indent();
                mStr("PUSHFRAME ");
                mInt(static_cast<Canon::PushFrame*>(node)->GetInfo()->GetTotalFrameSize());
                mStr("\n");
            }
            break;
        case Canon::T_POPFRAME:
            {
                Indent();
                mStr("POPFRAME\n");
            }
            break;
        case Canon::T_FUNGO:
            {
                Canon::FunGo* fungo = static_cast<Canon::FunGo*>(node);
                Indent();
                mStr("FUNGO(");
                mInt(fungo->GetLabel());
                mStr(") {");
                fungo->GetFunCall()->Access(this);
                mStr("}\n");
            }
            break;
        case Canon::T_LOAD_ADDR:
            {
                Indent();
                Canon::LoadAddr* ladr = static_cast<Canon::LoadAddr*>(node);
                mStr("LADR ");
                PrintRegister(ladr->GetRegister());
                mStr(" ");
                ladr->GetExp()->Access(this);
                mStr("\n");
            }
            break;
        case Canon::T_COPY_TO_ADDR:
            {
                Indent();
                Canon::CopyToAddr* ca = static_cast<Canon::CopyToAddr*>(node);
                mStr("COPYTOADDR ");
                PrintRegister(ca->GetRegister());
                mStr(" ");
                ca->GetExp()->Access(this);
                mStr("\n");
            }
            break;
        case Canon::T_CAST:
            {
                Indent();
                Canon::Cast* ca = static_cast<Canon::Cast*>(node);
                mStr("CAST<"); mStr(ca->IsIntToFloat() ? "int->float>" : "float->int>");
                PrintRegister(ca->GetRegister());
                mStr("\n");
            }
            break;
        case Canon::T_WRITE_OBJ_PROP:
            {
                Indent();
                Canon::WriteObjProp* cmd = static_cast<Canon::WriteObjProp*>(node);
                mStr("WRITEOBJPROP ");
                mStr(cmd->GetProp()->mName);
                mStr("\n");
            }
            break;
        case Canon::T_READ_OBJ_PROP:
            {
                Indent();
                Canon::ReadObjProp* cmd = static_cast<Canon::ReadObjProp*>(node);
                mStr("READOBJPROP ");
                mStr(cmd->GetProp()->mName);
                mStr("\n");
            }
            break;
        case Canon::T_EXIT:
            {
                Indent();mStr("EXIT\n");
            }
            break;
        default:
            PG_FAILSTR("Invalid assembly node");
        }
    }
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

void PrettyPrint::Visit(ArrayConstructor* n)
{
    mStr("static_array<"); PrintType(n->GetTypeDesc()); mStr(">");
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

void PrettyPrint::Visit(Annotations* n)
{
    mStr("@(");
    n->GetExpList()->Access(this);
    mStr(")");
}

void PrettyPrint::PrintType(const TypeDesc* type)
{
    if (type->GetModifier() != TypeDesc::M_ARRAY)
    {
        mStr(type->GetName());
    }
    else
    {
        PrintType(type->GetChild());
        mStr("["); mInt(type->GetModifierProperty().ArraySize); mStr("]");
    }
}

void PrettyPrint::Visit(ArgDec* n)
{
    mStr(n->GetVar()); mStr(" : "); 
    PrintType(n->GetType());
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
    //print annotations first
    if (n->GetAnnotations() != nullptr)
    {
        n->GetAnnotations()->Access(this);
    }
    mStr("\n");

    const char * name = n->GetName();
    if (name == nullptr || name[0] == '$') //this is a temp, print its offset instead
    {
        mStr("[");
        mInt(n->GetOffset());
        mStr("]");
    }
    else
    {
        mStr(n->GetName());
    }
}

void PrettyPrint::Visit(Binop* n)
{
    n->GetLhs()->Access(this);
    switch(n->GetOp())
    {
    case O_PLUS: mStr(" + ");
        break;
    case O_MINUS: mStr(" - ");
        break;
    case O_MUL: mStr(" * ");
        break;
    case O_DIV: mStr(" / ");
        break;
    case O_MOD: mStr(" % ");
        break;
    case O_LT: mStr(" < ");
        break;
    case O_GT: mStr(" > ");
        break;
    case O_GTE: mStr(" >= ");
        break;
    case O_LTE: mStr(" <= ");
        break;
    case O_EQ: mStr(" == ");
        break;
    case O_NEQ: mStr(" != ");
        break;
    case O_LAND: mStr(" && ");
        break;
    case O_LOR: mStr(" || ");
        break;
    case O_SET: mStr(" = ");
        break;
    case O_ACCESS: mStr("[");
        break;
    case O_DOT: mStr(".");
        break;
    default:
        break;
    }
    n->GetRhs()->Access(this);
    
    if (n->GetOp() == O_ACCESS)
    {
        mStr("]");
    }
    
}

void PrettyPrint::Visit(Unop* n)
{
    switch(n->GetOp())
    {
    case O_MINUS: mStr(" -");
        break;
    case O_EXPLICIT_CAST:
        mStr("(");mStr(n->GetTypeDesc()->GetName());mStr(")");
        break;
    default:
        break;
    }
    n->GetExp()->Access(this);
}
void PrettyPrint::Visit(FunCall* n)
{
    if (n->IsMethod())
    {
        n->GetArgs()->GetExp()->Access(this);
	    mStr(n->GetName());
        mStr("->(");
        n->GetArgs()->GetTail()->Access(this);
        mStr(")");
    }
    else
    {
	    mStr(n->GetName());
        mStr("(");
        n->GetArgs()->Access(this);
        mStr(")");
    }
}

void PrettyPrint::Visit(Imm* n)
{
    mInt(n->GetVariant().i[0]);
}

void PrettyPrint::Visit(StrImm* n)
{
    mStr("\"");
    mStr(n->GetStr());
    mStr("\"");
}

void PrettyPrint::Visit(StmtExp* n)
{
    Indent();
    n->GetExp()->Access(this);
    mStr(";");
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
    mStr(n->GetReturnType()->GetName());
    mStr(" ");
    mStr(n->GetName());
    mStr("(");
    n->GetArgList()->Access(this);
	Ast::StmtList* sl = n->GetStmtList();
    if (sl == nullptr)
    {
        mStr(") ;\n");
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
    mStr("} ");
    StmtIfElse* tail = n->GetTail();
    while (tail != nullptr)
    {
        if (tail->GetExp() != nullptr)
        {
            mStr("elif (");
            ++mScope;
            tail->GetExp()->Access(this);
            --mScope;
            mStr(") {\n");
        }
        else
        {
            mStr("else {\n");
        }

        ++mScope;
        tail->GetStmtList()->Access(this);
        --mScope;
        mStr("\n} ");
        tail = tail->GetTail();
    }
    mStr("\n");
}

void PrettyPrint::Visit(StmtReturn* n)
{
    Indent();
    mStr("return ");
    n->GetExp()->Access(this);
    mStr(";\n");
}


void PrettyPrint::Visit(StmtEnumTypeDef* enumDef)
{
    Indent();
    mStr("enum "); mStr(enumDef->GetEnumType()->GetName()); mStr("{\n");
    const EnumNode* node = enumDef->GetEnumType()->GetEnumNode();
    while (node != nullptr)
    {
        Indent();Indent();
        mStr(node->mIdd);
        mStr( node->mNext == nullptr ? "\n" : ",\n");
        node = node->mNext;
    }
    mStr("};\n");
}

void PrettyPrint::Visit(StmtStructDef* structDef)
{
    Indent();mStr("struct\n"); mStr(structDef->GetName());
    Indent();mStr("{");
    ArgList* argList = structDef->GetArgList();
    ++mScope;
    while(argList != nullptr)
    {
        ArgDec* argDec = argList->GetArgDec();
        mStr("\n");Indent(); argDec->Access(this); mStr(";");
        argList = argList->GetTail();
    }
    --mScope;
    mStr("\n");
    Indent();
    mStr("};\n");
}

