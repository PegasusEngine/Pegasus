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

class StackFrameInfo;

class TypeDesc;

class FunDesc;

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

    Exp() : mTypeDesc(nullptr) {}

    virtual ~Exp(){}

    const TypeDesc* GetTypeDesc() const { return mTypeDesc; }

    void SetTypeDesc(const TypeDesc* typeDesc) { mTypeDesc = typeDesc; }

    virtual int GetExpType() const = 0;

    VISITOR_ACCESS

protected:
    const TypeDesc* mTypeDesc;
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

    explicit Idd (const char * name) : mOffset(-1), mFrameOffset(-1), mIsGlobal(false) { mName = name; }

    virtual ~Idd(){}

    const char * GetName() const { return mName; }

    int GetOffset() const { return mOffset; }

    void SetOffset(int offset) { mOffset = offset; }

    int GetFrameOffset() const { return mFrameOffset; }

    void SetFrameOffset(int frameOffset) { mFrameOffset = frameOffset; }

    bool IsGlobal() const { return mIsGlobal; }

    void SetIsGlobal(bool isGlobal) { mIsGlobal = isGlobal; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:

    const char* mName;
    int  mOffset;
    int  mFrameOffset;
    bool mIsGlobal;
};

class Unop : public Exp
{
public:
    static const int sType;

    Unop(int op, Exp* exp) : mOp(op), mExp(exp) {}

    virtual ~Unop() {}

    Exp* GetExp() const { return mExp; } 
   
    int GetOp() const { return mOp; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:
    Exp* mExp;
    
    int  mOp;
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

    FunCall(ExpList* args, const char * name) : mDesc(nullptr) {mArgs = args; mName = name;}

    virtual ~FunCall(){}

    ExpList* GetArgs() const { return mArgs; }

    const char*     GetName() { return mName; }

    void     SetDesc(const FunDesc* d) { mDesc = d; }

    const FunDesc*   GetDesc() const { return mDesc; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:

    ExpList* mArgs;
    const char* mName;
    const FunDesc*  mDesc;
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

//! immediate string
class StrImm : public Exp
{
public:
    
    static const int sType;

    explicit StrImm(char* str) : mStr(str)
    {
    }

    virtual ~StrImm(){}

    char* GetStr() { return mStr; }

    VISITOR_ACCESS

    EXP_RTTI_DECL

private:
    char* mStr;
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

//! struct definition
class StmtStructDef : public Stmt
{
public:
    StmtStructDef(const char* structName, ArgList* argList)
        : mName(structName), mArgList(argList), mFrameInfo(nullptr)
    {
    }

    virtual ~StmtStructDef()
    {
    }

    const char* GetName() const { return mName; }

    ArgList* GetArgList() const { return mArgList; }

    StackFrameInfo* GetFrameInfo() const { return mFrameInfo; }

    void SetFrameInfo(StackFrameInfo* frameInfo) { mFrameInfo = frameInfo; }

    VISITOR_ACCESS

private:
    const char* mName;
    ArgList*    mArgList;
    StackFrameInfo* mFrameInfo;
};

//! function declarator
class StmtFunDec : public Stmt
{
public:

    StmtFunDec(ArgList* argList, const char* ret, const char* name) : mArgList(argList), mStmtList(nullptr), mRet(ret), mName(name), mReturnType(nullptr), mFrame(nullptr), mDesc(nullptr) {}

    virtual ~StmtFunDec() {}

    ArgList  *  GetArgList()  const { return mArgList; }

    StmtList * GetStmtList() const { return mStmtList; }

    const char* GetName() const { return mName; }

    const char* GetRet() const { return mRet; }

    const TypeDesc* GetReturnType() const { return mReturnType; }

    void SetReturnType(const TypeDesc* retType) { mReturnType = retType; }

    StackFrameInfo* GetFrame() const { return mFrame; }

    void SetFrame(StackFrameInfo* frame) { mFrame = frame; }

    FunDesc* GetDesc() const { return mDesc; }

    void SetDesc(FunDesc* d) { mDesc = d; }
    
    void SetStmtList(StmtList* stmtList) { mStmtList = stmtList; }

    VISITOR_ACCESS

private:

    ArgList  *  mArgList;

    StmtList * mStmtList;

    const char* mRet;

    const char* mName;

    const TypeDesc*  mReturnType;

    StackFrameInfo*  mFrame;

    FunDesc* mDesc;
};

class StmtWhile : public Stmt
{
public:
    StmtWhile(Exp* exp, StmtList* stmtList) : mExp(exp), mStmtList(stmtList), mFrame(nullptr) {}
    ~StmtWhile(){}

    Exp* GetExp() const { return mExp; }

    StmtList* GetStmtList() const { return mStmtList; }

    StackFrameInfo* GetFrame() const { return mFrame; }

    void SetFrame(StackFrameInfo* frame) { mFrame = frame; }

    VISITOR_ACCESS

private:
    
    Exp* mExp;
    
    StmtList* mStmtList;

    StackFrameInfo* mFrame;

};

//! ifthenelse
class StmtIfElse : public Stmt
{
public:

    StmtIfElse(
        Exp * exp, 
        StmtList* ifBlock, 
        StmtIfElse* elseTail,
        StackFrameInfo* frame
    ) : mExp(exp), mIf(ifBlock), mTail(elseTail), mFrame(frame) {}

    virtual ~StmtIfElse() {}

    Exp * GetExp() const { return mExp; }

    StmtList * GetStmtList() const { return mIf; }

    StmtIfElse* GetTail() const { return mTail; }

    void        SetTail(StmtIfElse* tail) { mTail = tail; }

    StackFrameInfo* GetFrame() const { return mFrame; }

    VISITOR_ACCESS

private:

    Exp* mExp;

    StmtList* mIf;

    StmtIfElse* mTail;

    StackFrameInfo* mFrame;
};

//! function argument description
class ArgDec : public Node 
{
public:

    ArgDec(const char* var, const TypeDesc* type) : mVar(var), mType(type), mOffset(-1) {}

    virtual ~ArgDec() {}

    const char* GetVar() const { return mVar; }

    const TypeDesc*  GetType() const { return mType; }

    void SetOffset(int offset) { mOffset = offset; }

    int  GetOffset() const { return mOffset; }

    VISITOR_ACCESS
    
private:

    const char* mVar;
    const TypeDesc*  mType;
    int              mOffset;
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
