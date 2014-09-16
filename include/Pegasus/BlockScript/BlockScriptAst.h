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

const int gMaxAluDimensions = 4;

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
    int   i[gMaxAluDimensions];
    float f[gMaxAluDimensions];
};

#define VISITOR_ACCESS void Access(IVisitor* visitor) { visitor->Visit(this); }
#define AST_IS_VALID_TYPE(x) (x != -1)
#define EXP_RTTI_DECL virtual int GetExpType() const { return sType; }

//! Expression base class
class Exp : public Node
{
public:

    Exp() : mTypeId(-1) {}

    virtual ~Exp(){}

    int GetTypeId() const { return mTypeId; }

    void SetTypeId(int id) { mTypeId = id; }

    virtual int GetExpType() const = 0;

    VISITOR_ACCESS

protected:
    int mTypeId;
};

//! tail recursive expression list base class
class ExpList : public Node
{
public:

    ExpList () : mExp(nullptr), mTail(nullptr) {}

    virtual ~ExpList() {}

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

    static const int sType;

    explicit Idd (const char * name) : mOffset(-1), mFrame(-1) { mName = name; }

    virtual ~Idd(){}

    const char * GetName() const { return mName; }

    int GetOffset() const { return mOffset; }

    void SetOffset(int offset) { mOffset = offset; }

    int GetFrame() const { return mFrame; }

    void SetFrame(int frame) { mFrame = frame; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:

    const char* mName;
    int  mOffset;
    int  mFrame;
};

//! binary operator expression class
class Binop : public Exp
{
public:

    static const int sType;

    Binop(Exp * lhs, int op, Exp * rhs)
    : mLhs(lhs), mOp(op), mRhs(rhs)
    {
    }

    virtual ~Binop(){}

    Exp * GetLhs() const { return mLhs; }

    Exp * GetRhs() const { return mRhs; }

    int   GetOp()  const { return mOp; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:

    Exp * mLhs;
    Exp * mRhs;
    int mOp;

};

class FunCall : public Exp
{
public:

    static const int sType;

    FunCall(ExpList* args, const char * name) : mDescId(-1) {mArgs = args; mName = name;}

    virtual ~FunCall(){}

    ExpList* GetArgs() const { return mArgs; }

    const char*     GetName() { return mName; }

    void     SetDescId(int id) { mDescId = id; }

    int      GetDescId() const { return mDescId; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:

    ExpList* mArgs;
    const char* mName;
    int  mDescId;
};

//! immediate value operator expression
class Imm : public Exp
{
public:

    static const int sType;

    explicit Imm(const Variant& v) 
    {
        mVariant = v;
    }

    virtual ~Imm(){}

    const Variant& GetVariant() const { return mVariant; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

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

    virtual ~StmtList() {}

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

    virtual ~Program(){}

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

    StmtFunDec(ArgList* argList, StmtList * stmtList, const char* ret, const char* name) : mArgList(argList), mStmtList(stmtList), mRet(ret), mName(name), mReturnType(-1), mFrame(-1), mDescId(-1) {}

    virtual ~StmtFunDec() {}

    ArgList  *  GetArgList()  const { return mArgList; }

    StmtList * GetStmtList() const { return mStmtList; }

    const char* GetName() const { return mName; }

    const char* GetRet() const { return mRet; }

    int GetReturnType() const { return mReturnType; }

    void SetReturnType(int retType) { mReturnType = retType; }

    int GetFrame() const { return mFrame; }

    void SetFrame(int frame) { mFrame = frame; }

    int GetDescId() const { return mDescId; }

    void SetDescId(int id) { mDescId = id; }

    VISITOR_ACCESS

private:

    ArgList  *  mArgList;

    StmtList * mStmtList;

    const char* mRet;

    const char* mName;

    int  mReturnType;

    int  mFrame;

    int  mDescId;
};

class StmtWhile : public Stmt
{
public:
    StmtWhile(Exp* exp, StmtList* stmtList) : mExp(exp), mStmtList(stmtList) {}
    ~StmtWhile(){}

    Exp* GetExp() const { return mExp; }

    StmtList* GetStmtList() const { return mStmtList; }

    VISITOR_ACCESS

private:
    
    Exp* mExp;
    
    StmtList* mStmtList;

};

class ElseTail : public Node
{
public:
    explicit ElseTail(StmtList* stmtList, ElseTail* tail) : mStmtList(stmtList), mTail(tail) {}
    ~ElseTail() {}
    StmtList* GetStmtList() const { return mStmtList; }

    ElseTail* GetTail() const { return mTail; }

    void SetTail(ElseTail* tail) { mTail = tail; }

    VISITOR_ACCESS

private:
    StmtList* mStmtList;
    ElseTail* mTail;
};

class ElseIfTail : public ElseTail
{
public:
    ElseIfTail (
        Exp* exp,
        StmtList* stmtList,
        ElseTail* tail
    ) :  ElseTail(stmtList, tail), mExp(exp) {}

    Exp* GetExp() const { return mExp; }
    VISITOR_ACCESS

private:
    Exp* mExp;
};

//! ifthenelse
class StmtIfElse : public Stmt
{
public:

    StmtIfElse(
        Exp * exp, 
        StmtList* ifBlock, 
        ElseTail* elseTail
    ) : mExp(exp), mIf(ifBlock), mTail(elseTail) {}

    virtual ~StmtIfElse() {}

    Exp * GetExp() const { return mExp; }

    StmtList * GetStmtList() const { return mIf; }

    ElseTail * GetTail() const { return mTail; }

    VISITOR_ACCESS

private:

    Exp* mExp;

    StmtList* mIf;

    ElseTail* mTail;
};

//! function argument description
class ArgDec : public Node 
{
public:

    ArgDec(const char* var, const char* type) : mVar(var), mType(type), mTypeId(-1), mFrame(-1), mOffset(-1) {}

    virtual ~ArgDec() {}

    const char* GetVar() const { return mVar; }

    const char* GetType() const { return mType; }

    void SetTypeId(int id)  { mTypeId = id; }

    int  GetTypeId() const { return mTypeId; }

    void SetFrame(int frame) { mFrame = frame; }
    
    int  GetFrame() const { return mFrame; }

    void SetOffset(int offset) { mOffset = offset; }

    int  GetOffset() const { return mOffset; }

    VISITOR_ACCESS
    
private:

    const char* mVar;
    const char* mType;
    int  mTypeId;
    int  mFrame;
    int  mOffset;
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
