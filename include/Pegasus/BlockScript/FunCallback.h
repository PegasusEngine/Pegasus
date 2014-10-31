/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FunCallback.h
//! \author Kleber Garcia
//! \date   October 28th 2014
//! \brief  Runtime utilities to register c++ callback functions into blockscript

#ifndef BLOCKSCRIPT_FUNCALLBACK_H
#define BLOCKSCRIPT_FUNCALLBACK_H

namespace Pegasus
{
namespace BlockScript
{

typedef int (*PrintStringCallbackType)(const char *);
typedef int (*PrintIntCallbackType)(int);
typedef int (*PrintFloatCallbackType)(float);

class SystemCallbacks
{
public:
static PrintStringCallbackType gPrintStrCallback;
static PrintIntCallbackType    gPrintIntCallback;
static PrintFloatCallbackType  gPrintFloatCallback;
};

//fwd declarations
class BlockScriptBuilder;
class FunDesc;
class BsVmState;
class BsVm;
struct Assembly;

class FunCallbackContext
{
public:

    //! constructor
    //! \param state the virtual machine state, containing all the memory and register values required.
    //! \param funDesc piece of data containing function description
    //! \param inputBuffer the raw buffer (packed) of the function arguments.
    //! \param inputBufferSize the raw buffer size of the input arguments
    //! \param outputBuffer, the return value buffer.
    //! \param outputBufferSize, the buffer size of the return value
    FunCallbackContext(
        BsVmState* state, 
        const FunDesc* funDesc,
        void* inputBuffer,
        int inputBufferSize,
        void* outputBuffer,
        int outputBufferSize
    ) : mState(state), 
        mFunDesc(funDesc), 
        mInputBuffer(inputBuffer), 
        mInputBufferSize(inputBufferSize), 
        mOutputBuffer(outputBuffer), 
        mOutputBufferSize(outputBufferSize) 
        {
        }
    ~FunCallbackContext(){}


    //! \return the virtual machine state.
    BsVmState* GetVmState() { return mState; }

    //! \return the raw input buffer. Use offsets manually and reinterpret_cast to the correct types.
    void* GetRawInputBuffer() { return mInputBuffer; }

    //! \return the input buffer size of the packed inputs
    int  GetInputBufferSize() const { return mInputBufferSize; }

    //! \return the output buffer.
    void* GetRawOutputBuffer() { return mOutputBuffer; }

    //! \return the output buffer size
    int   GetOutputBufferSize() const { return mOutputBufferSize; }

private:

    BsVmState* mState;
    const FunDesc* mFunDesc;
    void* mInputBuffer;
    int   mInputBufferSize;
    void* mOutputBuffer;
    int   mOutputBufferSize;
};

//block script callback for c++
typedef void (*FunCallback)(FunCallbackContext& context);

// The function bind point
typedef int FunBindPoint;

const FunBindPoint FUN_INVALID_BIND_POINT = -1;

//! creates an intrinsic function that can be called from blockscript
//! \param builder the block script builder (context holdnig the AST)
//! \param the function name
//! \param a string list with argument types definitions
//! \param a string list of the argument names.
//! \param a string with the return type
//! \param callback the actual c++ callback
//! \return true if successful. False on failure. 
bool CreateIntrinsicFunction(
    BlockScriptBuilder* builder, 
    const char* funName, 
    const char** argTypes, 
    const char** argNames, 
    int argCount, 
    const char* returnType, 
    FunCallback callback
);

//! Gets a function bind point to be used to call.
//! builder - the ast builder, containing necessary meta-data
//! assembly - the assembly code, containing the mapping with function bind points
//! funName - the string name of the function
//! argTypes - the argument definitions of the function 
//! argumentListCount - the count of the arguments of this function
//! \return FUN_INVALID_BIND_POINT if the function does not exist, otherwise a valid bind point.
FunBindPoint GetFunctionBindPoint(
    const BlockScriptBuilder* builder, 
    const Assembly& assembly,
    const char* funName,
    const char** argTypes,
    int argumentListCount
);


//! Executes a function from a specific bind point.
//! bindPoint - the function bind point. If an invalid bind point is passed, we return false.
//! builder - the ast builder, containing necessary meta-data
//! assembly - the assembly instruction set.
//! bsVmState - the vm state.
//! vm - the vm that will run the function.
//! inputBuffer - the input buffer. Pack all function arguments on this structure. For heap arguments, such as strings,
//!               register them manually on the vm state and then get an identifier int. Pass this int then in the buffer.
//! inputBufferSize - the size of the input argument buffer. If this size does not match the input buffer size of the function then this function returns false.
//! outputBuffer - the output buffer to be used. 
//! outputBufferSize - the size of the return buffer. If this size does not match the return value size, then this function returns false.
bool ExecuteFunction(
    FunBindPoint bindPoint,
    BlockScriptBuilder* builder, 
    const Assembly& assembly,
    BsVmState& state,
    BsVm& vm,
    void* inputBuffer,
    int   inputBufferSize,
    void* outputBuffer,
    int   outputBufferSize
);





}
}

#endif