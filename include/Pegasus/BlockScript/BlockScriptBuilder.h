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
    explicit BlockScriptBuilder() : mOnError(nullptr) {}
	
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
    Ast::Binop* BuildBinop (Ast::Exp* lhs, int op, Ast::Exp* rhs);
    Ast::FunCall* BuildFunCall(Ast::ExpList* args, const char * name);
    Ast::Imm*   BuildImm   (Ast::Variant& v);
    Ast::Idd*   BuildIdd   (const char * name);
    Ast::StmtExp* BuildStmtExp(Ast::Exp* exp);
    Ast::StmtReturn* BuildStmtReturn(Ast::Exp* exp);
    Ast::StmtTreeModifier* BuildStmtTreeModifier(Ast::ExpList* expList, Ast::Idd* var);
    Ast::StmtFunDec* BuildStmtFunDec(Ast::ArgList* argList, Ast::StmtList* stmtList, const char * returnIdd, const char * nameIdd);
    Ast::StmtIfElse* BuildStmtIfElse(Ast::Exp* exp, Ast::StmtList* ifBlock, Ast::StmtList* elseBlock);
    Ast::ArgDec* BuildArgDec(Ast::Idd* var, Ast::Idd* type);
    
private:
    AstAllocator mAllocator;
    ErrorMsgCallback   mOnError;
	CompilationResult  mActiveResult;
};

}
}
#endif
