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

#include "Pegasus/BlockScript/BlockScriptCompiler.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{

namespace BlockScript
{

class BlockLib;

//! Main API entry point for blockscript executables
class BlockScript : public BlockScriptCompiler
{

public:
    //! BlockScript constructor
    //! \param allocator the master allocator
    //! \param core runtime library containing core definitions for types
    BlockScript(Alloc::IAllocator* allocator, BlockLib* runtimeLibrary);

    //! Destructor
    virtual ~BlockScript();

    //! includes a library AST to be used for compilation.
    //! \param library - the actual library
    void IncludeLib(BlockLib* library);

    //! Runs the block script
    void Run(BsVmState* vmState); 

    //! Compiles a file string buffer into block script
    //! \param fb the file buffer containing the script
    //! \return true if successful, false otherwise
    virtual bool Compile(const Io::FileBuffer* fb);

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

    //! Read the global value stored in a handle.
    //! \param vmState - the virtual machine state containing all memory.
    //! \param bindPoint - the bind point of the global
    //! \param destBuffer - the buffer where the global values will be copied into
    //! \param destBufferRead - output value, the bytes read.
    //! \param destBufferSize - the size of the buffer passed in destBuffer. Function will assert and NOP
    //!                         if the size of the global underneath is bigger than the dest buffer. Use
    //!                         GetGlobalTypeDesc to get the proper type description and size of this global.
    void ReadGlobalValue(
        BsVmState*   vmState,
        GlobalBindPoint bindPoint,
        void* destBuffer,
        int&  destBufferRead,
        int   destBufferSize
    );

    //! Write to a global value through its handle.
    //! \param vmState - the virtual machine state containing all memory.
    //! \param bindPoint - the bind point of the global
    //! \param srcBuffer - contents to write into the global in the script.
    //! \param srcBufferSize - The size of the srcBuffer passed
    void WriteGlobalValue (
        BsVmState*   vmState,
        GlobalBindPoint bindPoint,
        const void* srcBuffer,
        int srcBufferSize
    );


private:
    // Virtual machine (state of this vm is pushed by the user through BsVmState class)
    BsVm      mVm;
    BlockLib* mRuntimeLib;
    Utils::Vector<BlockLib*> mLibs;
};

} //namespace BlockScript
} //namespace Pegasus

#endif
