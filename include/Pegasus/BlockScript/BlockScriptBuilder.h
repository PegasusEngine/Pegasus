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
    explicit BlockScriptBuilder() : mOnError(nullptr), mCurrentFrame(0), mErrorCount(0) {}
	
    struct CompilationResult
    {
        Ast::Program*    mAst;
    };

    void Initialize(Pegasus::Alloc::IAllocator* allocator, ErrorMsgCallback onError);
    ~BlockScriptBuilder(){}

    //! Begins construction of abstract syntax tree
    void BeginBuild();

    //! Ends construction of abstract syntax tree
    void EndBuild  (CompilationResult& r);

    //! destroys memory of compilation results
    void Reset ();

    //! Creation functions, of node general containers
    Ast::Program*  CreateProgram();
    Ast::ExpList*  CreateExpList();
    Ast::ArgList*  CreateArgList();
    Ast::StmtList* CreateStmtList();

    //! Node builders
    Ast::Exp* BuildBinop (Ast::Exp* lhs, int op, Ast::Exp* rhs);
    Ast::Exp* BuildFunCall(Ast::ExpList* args, const char * name);
    Ast::Exp*   BuildImmFloat   (float v);
    Ast::Exp*   BuildImmInt     (int   v);
    Ast::Idd*   BuildIdd   (const char * name);
    Ast::StmtExp* BuildStmtExp(Ast::Exp* exp);
    Ast::StmtReturn* BuildStmtReturn(Ast::Exp* exp);
    Ast::StmtTreeModifier* BuildStmtTreeModifier(Ast::ExpList* expList, Ast::Idd* var);
    Ast::StmtWhile*  BuildStmtWhile(Ast::Exp* exp, Ast::StmtList* stmtList);
    Ast::StmtFunDec* BuildStmtFunDec(Ast::ArgList* argList, Ast::StmtList* stmtList, const char * returnIdd, const char * nameIdd);
    Ast::StmtIfElse* BuildStmtIfElse(Ast::Exp* exp, Ast::StmtList* ifBlock, Ast::ElseTail* tail);
    Ast::ElseIfTail* BuildStmtElseIfTail(Ast::Exp* exp, Ast::StmtList* ifBlock, Ast::ElseTail* tail);
    Ast::ElseTail*   BuildStmtElseTail(Ast::StmtList* ifBlock);
    Ast::ArgDec* BuildArgDec(const char* var, const char* type);

    void IncErrorCount() { ++mErrorCount; }

    int GetErrorCount() const { return mErrorCount; }

    void StartNewFrame();
    void PopFrame();

private:

    // registers a member into the stack. Returns the offset of the current stack frame.
    int RegisterStackMember(const char* name, int type);

    // attempts a stack push when entering a function declaration through arguments or argument list
    void AttemptFunctionStackPush();
    
    Alloc::IAllocator* mGeneralAllocator;
    AstAllocator       mAllocator;
    TypeTable          mTypeTable;
    FunTable           mFunTable;
    ErrorMsgCallback   mOnError;
	CompilationResult  mActiveResult;

    int                mCurrentFrame;
    int                mErrorCount;

    Container<StackFrameInfo> mStackFrames;
};

}
}
#endif
