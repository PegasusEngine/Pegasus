/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptCanon.h
//! \author Kleber Garcia
//! \date   15th September 2014
//! \brief  Canonical tree, which will run on the virtual machine. A canonical tree is such
//!         that certain expressions (particularly function calls) are flattened out. In 
//!         canonical form, function calls are removed and labels are introduced. Its sort
//!         of a pseudo assembly form of the original AST.

#ifndef PEGASUS_BLOCKSCRIPT_CANON_H
#define PEGASUS_BLOCKSCRIPT_CANON_H

#include "Pegasus/BlockScript/Container.h"

#define CANON_REGISTER_BYTESIZE 4

namespace Pegasus
{
namespace BlockScript
{

class StackFrameInfo;
struct PropertyNode;

//forward declarations
namespace Ast
{
#define BS_PROCESS(N) class N;
#include "Pegasus/BlockScript/Ast.inl"
#undef BS_PROCESS
}

namespace Canon
{

enum Register
{
    R_RET, // return register
    R_G,   // global pointer register
    R_A,   // general purpose 1 
    R_B,   // general purpose 2 
    R_C,   // general purpose 3
    R_IP,  // instruction register
    R_SBP, // base stack pointer register
    R_ESP,  // top of stack pointer register
    R_COUNT
};

//canon tree definition of types
enum CanonTypes
{
    T_JMP,
    T_JMPCOND,
    T_RET,
    T_FUNGO,
    T_SAVE,
    T_SAVE_TO_ADDR,
    T_LOAD,
    T_LOAD_ADDR,
    T_MOVE,
    T_INSERT_DATA_TO_HEAP,
    T_PUSHFRAME,
    T_POPFRAME,
    T_COPY_TO_ADDR,
    T_CAST,
    T_READ_OBJ_PROP,
    T_WRITE_OBJ_PROP,
    T_EXIT,
    T_CHECK_MEM_ACESS,
};

// basic canonical node structure
class CanonNode
{
public:
    //! constructor
    CanonNode() {}
    
    //! destructor
    virtual ~CanonNode()  {}

    //! \return the type enumeration
    virtual CanonTypes GetType() const = 0;
};


// jump canonical command
class Jmp : public CanonNode
{
public:
    //! constructor
    //! \param label the label to jump to
    Jmp(int label) : mLabel(label) {}

    //! destructor
    virtual ~Jmp() {}

    //! the actual label to jump for this canonical node
    //! \return the label to jump to
    int GetLabel() const { return mLabel; }

    //! RTTI information
    virtual CanonTypes GetType() const { return T_JMP; }

private:
    int mLabel;
};


// jump conditional canonical command
class JmpCond : public CanonNode
{
public:
    //! constructor
    //! \param exp the expression to evaluate.
    //! \param comparison if equal to this value, jump to the label signaled
    JmpCond(Ast::Exp* exp, int comparison) : mComparison(comparison), mExp(exp), mLabelTrue(-1) {}

    //! the destructor
    virtual ~JmpCond(){}

    //! \return the label to jump to when true
    int GetLabel() const { return mLabelTrue; }

    void SetLabel(int l) { mLabelTrue = l; }

    Ast::Exp* GetExp() const { return mExp; }

    int GetComparison() const { return mComparison; }

    //! RTTI information
    virtual CanonTypes GetType() const { return T_JMPCOND; }

private:
    int mLabelTrue;
    int mComparison;
    Ast::Exp* mExp;

};

// function call. Sets variable to the R register.
class FunGo : public CanonNode
{
public:
    explicit FunGo(Ast::FunCall* funCall, int label) : mFunCall(funCall), mLabel(label) { }

    virtual ~FunGo(){}

    Ast::FunCall* GetFunCall() const { return mFunCall; }

    int GetLabel() const { return mLabel; }

    //! RTTI information
    virtual CanonTypes GetType() const { return T_FUNGO; }

private:
    Ast::FunCall* mFunCall;
    int           mLabel;
};

// return to previous label
class Ret : public CanonNode
{
public:
    //! constructor
    Ret() {}

    //! the destructor
    virtual ~Ret(){}

    //! RTTI information
    virtual CanonTypes GetType() const { return T_RET; }

private:
    Ast::Exp* mExp;

};

// Save register state to a piece of memory
class Save : public CanonNode
{
public:
    Save(Ast::Idd* tmp, Register r) : mTmp(tmp), mRegister(r) {}

    virtual ~Save(){}

    Ast::Idd* GetTmp() const { return mTmp; }

    Register GetRegister() const { return mRegister; }

    virtual CanonTypes GetType() const { return T_SAVE; } 

private:
    Ast::Idd* mTmp;
    Register  mRegister;
    
};

// save to a register
class Load : public CanonNode
{
public:
    Load(Register r, Ast::Exp* exp) : mExp(exp), mRegister(r) {}

    virtual ~Load(){}

    Ast::Exp* GetExp() const { return mExp; }

    Register GetRegister() const { return mRegister; }

    virtual CanonTypes GetType() const { return T_LOAD; } 

private:
    Ast::Exp* mExp;
    Register  mRegister;
    
};

// save to a register the address of the idd
class LoadAddr : public CanonNode
{
public:
    LoadAddr(Register r, Ast::Exp* exp) : mExp(exp), mRegister(r) {}

    virtual ~LoadAddr(){}

    Ast::Exp* GetExp() const { return mExp; }

    Register GetRegister() const { return mRegister; }

    virtual CanonTypes GetType() const { return T_LOAD_ADDR; } 

private:
    Ast::Exp* mExp;
    Register  mRegister;
    
};

// save register to the address inside the lhs register
class SaveToAddr : public CanonNode
{
public:
    SaveToAddr(Register lhs, Register rhs) : mLhs(lhs), mRhs(rhs) {}
    virtual ~SaveToAddr(){}

    Register GetLhs() const { return mLhs; }

    Register GetRhs() const { return mRhs; }

    virtual CanonTypes GetType() const { return T_SAVE_TO_ADDR; }

private:
    Register mLhs;
    Register mRhs;
};

class CopyToAddr : public CanonNode
{
public:
    CopyToAddr(Register r, Ast::Exp* exp, int byteSize) : mRegister(r), mExp(exp), mByteSize(byteSize) {}
    virtual ~CopyToAddr(){}
    
    Ast::Exp* GetExp() const { return mExp; }
    
    Register GetRegister() const { return mRegister;}

    int GetByteSize() const { return mByteSize; }

    virtual CanonTypes GetType() const { return T_COPY_TO_ADDR; }

private:
    Ast::Exp* mExp;
    Register mRegister;
    int  mByteSize;
};

class InsertDataToHeap : public CanonNode
{
public:
    InsertDataToHeap(Ast::Idd* tmpWithNewIndex, void* pointer) : mIdd(tmpWithNewIndex), mPtr(pointer) {}
    
    virtual ~InsertDataToHeap() {}

    Ast::Idd* GetTmp() const { return mIdd; }

    void* GetPointer() const { return mPtr; }

    virtual CanonTypes GetType() const { return T_INSERT_DATA_TO_HEAP; }

private:
    Ast::Idd*  mIdd;
    void*      mPtr;
};

// move from memory to memory, or from imm to memory
class Move : public CanonNode
{
public:
    Move (Ast::Idd* lhs, Ast::Exp* rhs) : mLhs(lhs), mRhs(rhs) {}

    virtual ~Move() {}

    Ast::Idd* GetLhs() const { return mLhs; }

    Ast::Exp* GetRhs() const { return mRhs; }

    virtual CanonTypes GetType() const { return T_MOVE; } 

private:
    Ast::Idd* mLhs;
    Ast::Exp* mRhs;

};

class Cast : public CanonNode
{
public:
    Cast(bool isIntToFloat, Register r) : mIsIntToFloat(isIntToFloat), mRegister(r)
    {
    } 

    bool IsIntToFloat() const { return mIsIntToFloat; }

    Register GetRegister() const { return mRegister; }

    virtual CanonTypes GetType() const { return T_CAST; } 

private:
    bool mIsIntToFloat;
    Register mRegister;
    
};

class Exit : public CanonNode
{
public:
    Exit(){}
    virtual ~Exit(){}
    
    virtual CanonTypes GetType() const { return T_EXIT; }
};

class PushFrame : public CanonNode
{
public:
    explicit PushFrame(const StackFrameInfo* info) : mFrameInfo(info) {}
    virtual ~PushFrame() {}
    
    virtual CanonTypes GetType() const { return T_PUSHFRAME; }

    const StackFrameInfo* GetInfo() const { return mFrameInfo; }

private:
    const StackFrameInfo* mFrameInfo;
};

class PopFrame : public CanonNode
{
public:
    PopFrame() {}
    virtual ~PopFrame() {} 
    virtual CanonTypes GetType() const { return T_POPFRAME; }
};

class ReadObjProp : public CanonNode
{
public:
    ReadObjProp(Ast::Exp* location, Ast::Exp* objRef, const PropertyNode* propertyNode)
    : mLoc(location), mObj(objRef), mProp(propertyNode) {}

    virtual ~ReadObjProp() {}

    Ast::Exp* GetLoc() const { return mLoc; }

    Ast::Exp* GetObj() const { return mObj; }

    const PropertyNode* GetProp() const { return mProp; }

    virtual CanonTypes GetType() const { return T_READ_OBJ_PROP; }
private:
    Ast::Exp* mLoc;
    Ast::Exp* mObj;
    const PropertyNode* mProp;
};

class WriteObjProp : public CanonNode
{
public:
    WriteObjProp(Ast::Exp* objRef, const PropertyNode* propertyNode, Ast::Exp* location)
    : mLoc(location), mObj(objRef), mProp(propertyNode) {}

    virtual ~WriteObjProp() {}

    Ast::Exp* GetLoc() const { return mLoc; }

    Ast::Exp* GetObj() const { return mObj; }

    const PropertyNode* GetProp() const { return mProp; }

    virtual CanonTypes GetType() const { return T_WRITE_OBJ_PROP; }

private:
    Ast::Exp* mLoc;
    Ast::Exp* mObj;
    const PropertyNode* mProp;
};

// code block declaration, containing a label header and a list of commands
class Block
{
public:

    //! constructor
    //! \param alloc the allocator
    //! \param label the label of this block
    Block() : mLabel(-1), mNextBlock(-1) { }

    //! destructor
    ~Block() {}

    //! initializes this block
    //! \param the allocator
    //! \param the label
    void Initialize(Alloc::IAllocator* alloc, int label) { mStmts.Initialize(alloc); mLabel = label; }

    //! \return the label of this block
    int GetLabel() const { return mLabel; }

    //! \return the statement list of this block
    Container<CanonNode*>& GetStmts() { return mStmts; }

    const Container<CanonNode*>& GetStmts() const { return mStmts; }

    //! gets the index of the next block
    int NextBlock() const { return mNextBlock; }

    //! sets the index of the next block
    void SetNextBlock(int b) { mNextBlock = b; }

private:
    Container<CanonNode*> mStmts;
    int mLabel;
    int mNextBlock;
};

}

}
}

#endif
