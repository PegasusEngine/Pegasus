/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptAst.h
//! \author Kleber Garcia
//! \date   27th August 2014
//! \brief  Blockscript abstract syntax tree

#ifndef BLOCK_SCRIPT_AST_H
#define BLOCK_SCRIPT_AST_H

#include "Pegasus/BlockScript/IVisitor.h"

namespace Pegasus
{
namespace BlockScript
{
namespace Ast
{

const int gMaxStringSz = 64;

//fwd declarations
#define BS_PROCESS(N) class N;
#include "Pegasus/BlockScript/Ast.inl"
#undef BS_PROCESS

class Node
{
public:
    Node(){}
    virtual ~Node(){}
    virtual void Access(IVisitor* visitor) = 0;
};

union Variant
{
    int   i[4];
    float f[4];
};

#define VISITOR_ACCESS void Access(IVisitor* visitor) { visitor->Visit(this); }

//! Expression base class
class Exp : public Node
{
public:
    Exp(){}
    virtual ~Exp(){}
    VISITOR_ACCESS
};

//! tail recursive expression list base class
class ExpList : public Node
{
public:
    ExpList () : mExp(nullptr), mTail(nullptr) {}
    ~ExpList() {}
    Exp * GetExp()      const { return mExp; }
    ExpList * GetTail() const { return mTail; }
    void SetExp(Exp* exp) { mExp = exp; }
    void SetTail(ExpList * tail) { mTail = tail; }
    VISITOR_ACCESS

private:
    Exp *     mExp;
    ExpList * mTail;
};

//! identifier expression class
class Idd : public Exp
{
public:
    explicit Idd (const char * name) { mName = name; }
    ~Idd(){}
    const char * GetName() const { return mName; }
    int GetResolvedIndex() const { return mResolution; }
    void SetResolveIndex(int resolution) { mResolution = resolution; }
    VISITOR_ACCESS

private:
    const char* mName;
    int  mResolution;
};

//! binary operator expression class
class Binop : public Exp
{
public:
    Binop(Exp * lhs, int op, Exp * rhs)
    : mLhs(lhs), mOp(op), mRhs(rhs)
    {
    }
    virtual ~Binop(){}
    Exp * GetLhs() const { return mLhs; }
    Exp * GetRhs() const { return mRhs; }
    int   GetOp()  const { return mOp; }
    VISITOR_ACCESS

private:
    Exp * mLhs;
    Exp * mRhs;
    int mOp;

};

class FunCall : public Exp
{
public:
    FunCall(ExpList* args, Idd * idd) {mArgs = args; mIdd = idd;}
    virtual ~FunCall(){}
    ExpList* GetArgs() const { return mArgs; }
    Idd*     GetIdd() { return mIdd; }
    VISITOR_ACCESS

private:
    ExpList* mArgs;
    Idd* mIdd;
};

//! immediate value operator expression
class Imm : public Exp
{
public:

    explicit Imm(const Variant& v) 
    {
        mVariant = v;
    }
    virtual ~Imm(){}
    const Variant& GetVariant() const { return mVariant; }
    VISITOR_ACCESS

private:
    Variant mVariant;
};

//! statement base class
class Stmt : public Node
{
public:
    Stmt(){}
    virtual ~Stmt(){}
    VISITOR_ACCESS

private:
};

//! statement list class
class StmtList : public Node
{
public:
    StmtList() : mStmt(nullptr), mTail(nullptr) {}
    ~StmtList() {}
    Stmt * GetStmt()   const { return mStmt; }
    StmtList * GetTail() const { return mTail; }
    void SetTail(StmtList * tail) { mTail = tail; }
    void SetStmt(Stmt * stmt) { mStmt = stmt; }
    VISITOR_ACCESS

private:
    Stmt * mStmt;
    StmtList * mTail;
};

//! master program ast class
class Program : public Node
{
public:
    explicit Program() : mStmtList(nullptr) {}
    ~Program(){}
    StmtList * GetStmtList() const { return mStmtList; }
    void SetStmtList(StmtList* stmtList) { mStmtList = stmtList; }
    VISITOR_ACCESS

private:
    StmtList * mStmtList;
};

//! orphan expression cclass
class StmtExp : public Stmt
{
public:
    explicit StmtExp(Exp * exp) : mExp(exp) {}
    virtual ~StmtExp(){}
    Exp * GetExp() const { return mExp; }
    VISITOR_ACCESS

private:
    Exp * mExp;
};

//! tree modifier statement (for tree node properties)
class StmtTreeModifier : public Stmt
{
public:
    StmtTreeModifier(ExpList * expList, Idd * var) : mIdd(var), mExpList(expList) {}
    virtual ~StmtTreeModifier(){}
    ExpList * GetExpList() const { return mExpList; }
    Idd     * GetIdd()     const { return mIdd; }
    VISITOR_ACCESS

private:
    Idd     * mIdd;
    ExpList * mExpList;
};

//! return statement
class StmtReturn : public Stmt
{
public:
    explicit StmtReturn(Exp * exp) : mExp(exp) {}
    virtual ~StmtReturn() {}
    Exp * GetExp() const { return mExp; }
    VISITOR_ACCESS

private:
    Exp * mExp;
};

//! function declarator
class StmtFunDec : public Stmt
{
public:
    StmtFunDec(ArgList* argList, StmtList * stmtList, Idd* ret, Idd* name) : mArgList(argList), mStmtList(stmtList), mRet(ret), mName(name) {}
    virtual ~StmtFunDec() {}
    ArgList  *  GetArgList()  const { return mArgList; }
    StmtList * GetStmtList() const { return mStmtList; }
    Idd* GetName() const { return mName; }
    Idd* GetRet() const { return mRet; }
    VISITOR_ACCESS

private:
    ArgList  *  mArgList;
    StmtList * mStmtList;
    Idd* mRet;
    Idd* mName;
};

//! ifthenelse
class StmtIfElse : public Stmt
{
public:
    StmtIfElse(Exp * exp, StmtList* ifBlock, StmtList* elseBlock) : mExp(exp), mIf(ifBlock), mElse(elseBlock) {}
    ~StmtIfElse() {}
    Exp * GetExp() const { return mExp; }
    StmtList * GetIfBlock() const { return mIf; }
    StmtList * GetElseBlock() const { return mElse; }
    VISITOR_ACCESS

private:
    Exp* mExp;
    StmtList* mIf;
    StmtList* mElse;
};

//! function argument description
class ArgDec : public Exp
{
public:
    ArgDec(Idd* var, Idd* type) : mVar(var), mType(type) {}
    virtual ~ArgDec() {}
    Idd* GetVar() const { return mVar; }
    Idd* GetType() const { return mType; }
    VISITOR_ACCESS
    
private:
    Idd* mVar;
    Idd* mType;
};

//! argument list
class ArgList : public Node
{
public:
    ArgList() : mArgDec(nullptr), mTail(nullptr) {}
    virtual ~ArgList() {}
    ArgDec* GetArgDec() const { return mArgDec; }
    ArgList* GetTail () const { return mTail;   }
    void SetArgDec(ArgDec* argDec) { mArgDec = argDec; }
    void SetTail(ArgList* tail) { mTail = tail; }
    VISITOR_ACCESS

private:
    ArgDec* mArgDec;
    ArgList* mTail;
    
};

}
}
}

#endif
