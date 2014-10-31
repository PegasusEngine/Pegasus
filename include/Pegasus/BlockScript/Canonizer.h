/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Canonizer.h
//! \author Kleber Garcia
//! \date   15th September 2014
//! \brief  Converts an AST into a canonical tree. Duplicates the nodes that it needs to, leaving
//!         the original AST intact

#ifndef CANONIZER_H
#define CANONIZER_H

#include "Pegasus/BlockScript/IVisitor.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/AstAllocator.h"
#include "Pegasus/BlockScript/BlockScriptCanon.h"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/BlockScript/StackFrameInfo.h"
#include "Pegasus/BlockScript/FunDesc.h"

namespace Pegasus
{

//fwd declarations
namespace Alloc
{
    class IAllocator;
}


namespace BlockScript
{

class TypeTable;
class FunTable;
class TypeDesc;

// function map entry that contains a function id mapped to a block in the assembly
struct FunMapEntry
{
    int mFunId;
    int mAssemblyBlock;
};

// structure that represents the assembly construction of blockscript
struct Assembly
{
    Container<Canon::Block>* mBlocks;
    Container<FunMapEntry>*  mFunBlockMap;
};

// Canonizer class
class Canonizer : private IVisitor
{
public:
    //Constructor
    Canonizer() : 
        mInternalAllocator(nullptr),
        mCurrentBlock(0), 
        mCurrentTempAllocationSize(0),
        mRebuiltExpression(nullptr), 
        mRebuiltExpList(nullptr), 
        mCurrentTypeTable(nullptr),
        mCurrentFunTable(nullptr),
        mCurrFrames(nullptr),
        mCurrentStackFrame(0),
        mNextLabel(0)
    {
    }

    //Destructor
    virtual ~Canonizer() {}

    //! \param alloc initial allocator
    void Initialize(Alloc::IAllocator* alloc);

    //! resets the state, does not free memory
    void Reset();

    //! Does its job, convert the program into a canon tree
    //! \param program the program to canonize
    //! \param typeTable the type table storing all the types of the program
    //! \param funTable the function table containing all the function mappings
    //! \param stackFrameInfos the set of stack frames
    void Canonize(
        Ast::Program* program,
        TypeTable* typeTable,
        FunTable*  funTable,
        Container<StackFrameInfo>* stackFrameInfos
    );

    //! Gets the assembly generated from the canonizer step.
    //! \return the assembly generated from the assembly step.
    Assembly GetAssembly() { Assembly a = { &mBlocks, &mFunBlockMap }; return a; }

private:
    // visitor functions
    #define BS_PROCESS(N) virtual void Visit(Ast::N*);
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS


    //! create a new block
    int CreateBlock();

    //! AddBlock adds a current block to the block list
    void AddBlock(int id);

    //! inserts a canonical node to the current block
    void PushCanon(Canon::CanonNode* n);

    //! pushes one temporal allocation for the current stack frame
    Ast::Idd* AllocateTemporal(const TypeDesc* type); 
   
    //! resets the temporal memory counter
    void ResetTemporals();

    //! registers a function label
    void RegisterFunLabel(const FunDesc* funDesc, int label);

    //! returns a label. If it doesnt exist creates one for the corresponding function
    int GetLabel(const FunDesc* funDesc);

    void BuildFunctionAsm();

    bool IsContinuousSwizzle(Ast::Idd* swizzle) const;

    //! only saves the Ret register if the current context is a function greater than 4 bytes.
    //! this is because Ret contains at this point the address of the output value for this function
    //! \return the address of the place where the register was stored.
    Ast::Idd* BeginSaveRet();

    //! only recovers the Ret register if the current context is a function greater than 4 bytes.
    //! this is because Ret contains at this point the address of the output value for this function
    //! \param  the address where the register is saved
    void EndSaveRet(Ast::Idd* idd);


    //! Processes a function call
    void ProcessFunCall(Ast::FunCall* funCall);

    //! swizzle helper
    void HandleNonContinuousSwizzle(Ast::Exp* targetExp, Ast::Idd* swizzle, const TypeDesc* targetType);

    //! handles array access operator
    void HandleArrayAccessOperator(Ast::Binop* op);
    void HandleSetOperator(Ast::Binop* op);
    void HandleDotOperator(Ast::Binop* op);

    Alloc::IAllocator* mInternalAllocator;
    TypeTable* mCurrentTypeTable;
    FunTable*  mCurrentFunTable;
    Container<StackFrameInfo> * mCurrFrames;
    Ast::Exp* mRebuiltExpression;
    Ast::ExpList* mRebuiltExpList;

    const StackFrameInfo* mCurrentStackFrame;
    const FunDesc*        mCurrentFunDesc;
    int mCurrentBlock;
    int mCurrentTempAllocationSize;
    int mNextLabel;

    AstAllocator mAllocator;
    Container<Canon::Block> mBlocks;
    Container<FunMapEntry>  mFunBlockMap;

    struct FunDescIntPair
    {
        const FunDesc* mFunDesc;
        int            mInt;
    };

    Container<FunDescIntPair> mLabelMap;
    IddStrPool mStrPool;

};

}
}

#endif
