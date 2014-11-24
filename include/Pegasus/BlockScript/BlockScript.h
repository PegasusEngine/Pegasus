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
#include "Pegasus/BlockScript/BlockScriptCanon.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/Canonizer.h"
#include "Pegasus/BlockScript/BsVm.h"

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
        class IBlockScriptCompilerListener;
    
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
    void Run(BsVmState* vmState); 

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
    //! funName - the string name of the function
    //! argTypes - the argument definitions of the function 
    //! argumentListCount - the count of the arguments of this function
    //! \return FUN_INVALID_BIND_POINT if the function does not exist, otherwise a valid bind point.
    FunBindPoint GetFunctionBindPoint(
        const char* funName,
        const char** argTypes,
        int argumentListCount
    ) const;

    //! Executes a function from a specific bind point.
    //! vmState - the state of the VM to run
    //! bindPoint - the function bind point. If an invalid bind point is passed, we return false.
    //! inputBuffer - the input buffer. Pack all function arguments on this structure. For heap arguments, such as strings,
    //!               register them manually on the vm state and then get an identifier int. Pass this int then in the buffer.
    //! inputBufferSize - the size of the input argument buffer. If this size does not match the input buffer size of the function then this function returns false.
    //! outputBuffer - the output buffer to be used. 
    //! outputBufferSize - the size of the return buffer. If this size does not match the return value size, then this function returns false.
    bool ExecuteFunction(
        BsVmState*   vmState,
        FunBindPoint functionBindPoint,
        void* inputBuffer,
        int   inputBufferSize,
        void* outputBuffer,
        int   outputBufferSize
    );

private:
    Alloc::IAllocator*       mAllocator;
    BlockScriptBuilder       mBuilder;
    Ast::Program*            mAst;
    Assembly                 mAsm;

    // Virtual machine (state of this vm is pushed by the user through BsVmState class)
    BsVm      mVm;
};

}
}

#endif
