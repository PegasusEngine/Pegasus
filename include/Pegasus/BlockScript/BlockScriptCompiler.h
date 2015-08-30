/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptCompiler.h
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Base class defining the basic compiler operations

#ifndef PEGASUS_BLOCKSCRIPT_COMPILER_H
#define PEGASUS_BLOCKSCRIPT_COMPILER_H

#include "Pegasus/BlockScript/BlockScriptBuilder.h"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/BlockScript/BlockScriptCanon.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/Canonizer.h"

//forward declarations begin
namespace Pegasus {
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
    
        class BlockLib;
		class IddStrPool;
        class IBlockScriptCompilerListener;
        class IFileIncluder;
    
        namespace Ast
        {
            class Program;
        }
    }
}
//forward declarations end

namespace Pegasus
{

namespace BlockScript
{

// compiler definition
class BlockScriptCompiler
{

public:
    //! BlockScriptCompiler constructor
    //! \param allocator the master allocator
    BlockScriptCompiler(Alloc::IAllocator* allocator);

    //! Destructor
    virtual ~BlockScriptCompiler();

    //! Compiles a file string buffer into block script
    //! \param fb the file buffer containing the script
    //! \return true if successful, false otherwise
    virtual bool Compile(const Io::FileBuffer* fb);

    //! Resets all memory. Call this if Compile is going to be called again
    void Reset();

    //! Gets the abstract syntax tree constructed from Compile
    //! \return the abstract syntax tree
    Ast::Program* GetAst() { return mAst; }

    //! Gets the virtual machine assembly produced from the block script source
    //! \param the virtual machine state
    //! \return list of blocks
    Assembly GetAsm() { return mAsm; }

    //! Sets the compiler event listener, to be used to listen to internal blockscript compiler events
    //! \param eventListener the listener to push
    void SetCompilerEventListener(IBlockScriptCompilerListener* eventListener);

    //! Gets a function bind point to be used to call.
    //! \param funName - the string name of the function
    //! \param argTypes - the argument definitions of the function 
    //! \param argumentListCount - the count of the arguments of this function
    //! \return FUN_INVALID_BIND_POINT if the function does not exist, otherwise a valid bind point.
    FunBindPoint GetFunctionBindPoint(
        const char* funName,
        const char** argTypes,
        int argumentListCount
    ) const;

    //! Gets the bind point of a global. Globals must be marked as "extern" in code in order to be exposed to the API
    //! \param globalName. the name of the global to find.
    GlobalBindPoint GetGlobalBindPoint(const char* globalName) const;

    //! Get the type description of a global variable marked as extend
    //! \param the bind point of the global
    //! \return the type description
    const TypeDesc* GetTypeDesc(GlobalBindPoint bindPoint) const;

    //! Sets a file includer interface for compilation.
    //! \param includer - the includer used for #include directives
    void SetFileIncluder(IFileIncluder* includer) { mFileIncluder = includer; }

    //! Gets the file includer set.
    //! \return the includer to get.
    IFileIncluder* GetFileIncluder() const { return mFileIncluder; }

protected:
    BlockScriptBuilder       mBuilder;

private:
    Alloc::IAllocator*       mAllocator;
    Ast::Program*            mAst;
    Assembly                 mAsm;
    IFileIncluder*            mFileIncluder;

};

}

}

#endif
