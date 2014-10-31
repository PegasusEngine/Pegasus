/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptBuilder.h
//! \author Kleber Garcia
//! \date   28th August 2014
//! \brief  Blockscript abstract syntax tree builder, also type checker. 
//!         returns respective runtime ready structures to run the script.

#ifndef BLOCK_SCRIPT_BUILDER_H
#define BLOCK_SCRIPT_BUILDER_H

#include "Pegasus/BlockScript/AstAllocator.h"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/FunTable.h"
#include "Pegasus/BlockScript/StackFrameInfo.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/Canonizer.h"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/BlockScript/BlockScriptCanon.h"

namespace Pegasus
{


    //forward declarations begin
    namespace BlockScript
    {
        namespace Ast
        {
        //fwd declarations
        #define BS_PROCESS(N) class N;
        #include "Pegasus/BLockScript/Ast.inl"
        #undef BS_PROCESS
        union Variant;
        }
    }

    namespace Alloc
    {
    class IAllocator; 
    }
    //forward declarations end

namespace BlockScript
{


//! Block Script Builder, type checker and constructor of script abstract syntax tree
class BlockScriptBuilder
{
public:
    typedef void (*ErrorMsgCallback) (const char*);
    explicit BlockScriptBuilder() : mOnError(nullptr), mCurrentFrame(0), mErrorCount(0), mInFunBody(false) {}
	
    struct CompilationResult
    {
        Ast::Program*    mAst;
        Assembly         mAsm;
    };

    void Initialize(Pegasus::Alloc::IAllocator* allocator, ErrorMsgCallback onError);
    ~BlockScriptBuilder(){}

    //! Begins construction of abstract syntax tree
    void BeginBuild();

    //! Ends construction of abstract syntax tree
    void EndBuild  (CompilationResult& r);

    //! destroys memory of compilation results
    void Reset ();

    //! true if we can start a new function. False otherwise
    bool StartNewFunction();

    //! Creation functions, of node general containers
    Ast::Program*  CreateProgram();
    Ast::ExpList*  CreateExpList();
    Ast::ArgList*  CreateArgList();
    Ast::StmtList* CreateStmtList();

    //! Node builders
    Ast::Exp* BuildBinop (Ast::Exp* lhs, int op, Ast::Exp* rhs);
    Ast::Exp* BuildUnop  (int op, Ast::Exp* exp);
    Ast::Exp* BuildExplicitCast  (Ast::Exp* exp, const char* type);
    Ast::Exp* BuildFunCall(Ast::ExpList* args, const char * name);
    Ast::Exp*   BuildImmFloat   (float v);
    Ast::Exp*   BuildImmInt     (int   v);
    Ast::Idd*   BuildIdd   (const char * name);
    Ast::StmtExp* BuildStmtExp(Ast::Exp* exp);
    Ast::StmtReturn* BuildStmtReturn(Ast::Exp* exp);
    Ast::StmtTreeModifier* BuildStmtTreeModifier(Ast::ExpList* expList, Ast::Idd* var);
    Ast::StmtWhile*  BuildStmtWhile(Ast::Exp* exp, Ast::StmtList* stmtList);
    Ast::StmtFunDec* BuildStmtFunDec(Ast::ArgList* argList, const char * returnIdd, const char * nameIdd);
    Ast::StmtFunDec* BindFunImplementation(Ast::StmtFunDec* funDec, Ast::StmtList* stmts);
    Ast::StmtIfElse* BuildStmtIfElse(Ast::Exp* exp, Ast::StmtList* ifBlock, Ast::StmtIfElse* tail, const StackFrameInfo* frame);
    Ast::StmtStructDef* BuildStmtStructDef(const char* name, Ast::ArgList* definitions);
    Ast::ArgDec* BuildArgDec(const char* var, const TypeDesc* type);
    Ast::Exp* BuildStrImm(const char* strToCopy);

    void IncErrorCount() { ++mErrorCount; }

    int GetErrorCount() const { return mErrorCount; }

    const StackFrameInfo* StartNewFrame();
    void PopFrame();

    TypeTable* GetTypeTable() { return &mTypeTable; }

    FunTable* GetFunTable() { return &mFunTable; }

    const TypeTable* GetTypeTable() const { return &mTypeTable; }

    const FunTable* GetFunTable() const { return &mFunTable; }

    void BindIntrinsic(Ast::StmtFunDec* funDec, FunCallback callback);

    IddStrPool& GetStringPool() { return mStrPool; }

    char* AllocateBigString(int size);

private:

    // registers a member into the stack. Returns the offset of the current stack frame.
    //! returns the offset of such member
    int RegisterStackMember(const char* name, int type);

    //! builds a binary operator for array access. Determines whether its an array or not.
    //! returns the expression corresponding to such array element.
    Ast::Exp* BuildBinopArrayAccess(Ast::Exp* lhs, int op, Ast::Exp* rhs); 

    //! \return true if operation is valid for this type, false otherwise
    bool IsBinopValid(const TypeDesc* type, int op);
    
    //! Attempts a type promotion. Creates an implicit cast if successful
    //! \param exp the expression
    //! \param targetType the target type to promote to.
    //! \return a new expression if success, otherwise returns the same expression passed.
    Ast::Exp* AttemptTypePromotion(Ast::Exp* exp, const TypeDesc* targetType);

    Alloc::IAllocator* mGeneralAllocator;
    AstAllocator       mAllocator;
    TypeTable          mTypeTable;
    FunTable           mFunTable;
    ErrorMsgCallback   mOnError;
	CompilationResult  mActiveResult;
    IddStrPool         mStrPool;

    int                mCurrentFrame;
    int                mErrorCount;

    Container<StackFrameInfo> mStackFrames;

    Canonizer mCanonizer;

    bool mInFunBody;
};

}
}
#endif
