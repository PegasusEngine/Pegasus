/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScript.h
//! \author Kleber Garcia
//! \date   1st September 2014
//! \brief  BlockScript master class. Main api to control / run / halt /disect a blockscript

#ifndef PEGASUS_BLOCKSCRIPT_BS_H
#define PEGASUS_BLOCKSCRIPT_BS_H
#include "Pegasus/BlockScript/BlockScriptBuilder.h"
#include "Pegasus/BlockScript/IddStrPool.h"

namespace Pegasus
{

//forward declarations begin
    namespace Alloc
    {
        class IAllocator;
    }

    namespace Io
    {
        class FileBuffer;
    }
    
    namespace BlockScript
    {
    
		class IddStrPool;
    
        namespace Ast
        {
            class Program;
        }
    }

//forward declarations end

namespace BlockScript
{

//! Main API entry point for blockscript
class BlockScript
{

public:
    //! BlockScript constructor
    //! \param allocator the master allocator
    BlockScript(Alloc::IAllocator* allocator);

    //! Destructor
    ~BlockScript();

    //! Compiles a file string buffer into block script
    //! \param fb the file buffer containing the script
    //! \return true if successful, false otherwise
    bool Compile(const Io::FileBuffer* fb);

    //! Resets all memory. Call this if Compile is going to be called again
    void Reset();

    //! Runs the block script
    void Run(); 

    //! Gets the abstract syntax tree constructed from Compile
    //! \return the abstract syntax tree
    Ast::Program* GetAst() { return mAst; }

    //! utilities for lexer
    //! \return the idd pool
    IddStrPool* GetIddPool() { return &mIddStrPool; }

private:
    Alloc::IAllocator* mAllocator;
    BlockScriptBuilder mBuilder;
    IddStrPool         mIddStrPool;
    Ast::Program*      mAst;
    
};

}
}

#endif
