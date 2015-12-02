/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IBlockScriptCompilerListener.h
//! \author	Kleber Garcia
//! \date	November 5th
//! \brief	Callback for compiler errors

#ifndef PEGASUS_BLOCKSCRIPT_COMPILER_ERRORS_H
#define PEGASUS_BLOCKSCRIPT_COMPILER_ERRORS_H

namespace Pegasus
{
namespace BlockScript
{
//forward declaration
class BsVmState;

//! Callback for compilation errors
class IBlockScriptCompilerListener
{
public:

    //! Triggered when compilation starts
    virtual void OnCompilationBegin() = 0;

    //! Triggered when there is a compilation error
    //! \param line the actual
    //! \param errorMessage the actual error message
    virtual void OnCompilationError(int line, const char* errorMessage, const char* token) = 0;

    //! Called at the end of a compilation
    //! \param success true if it was successful, false otherwise
    virtual void OnCompilationEnd(bool success) = 0;
};

//! Empty Structure for now
//! /* TODO: Implement Crash logic */
struct CrashInfo
{
public:
    CrashInfo() : unused(0) {}
    int unused;
};

// runtime listener. This class has callbacks on the runtime, when certain events have been triggered
class IRuntimeListener
{
public:
    //! Destructor
    virtual ~IRuntimeListener(){}

    //! Triggered when the runtime has not triggered the first instruction
    //! \param state the runtime vm state
    virtual void OnRuntimeBegin(BsVmState& state) = 0;

    //! Triggered when the first stack command has been triggered
    //! \param state - the runtime state that has been initialized
    virtual void OnStackInitalized(BsVmState& state) = 0;

    //! Triggered when the runtime has exited. The global scope is still alive at this point
    //! so functions can be called even after exiting the Vm.
    //! \param state the runtime vm state
    virtual void OnRuntimeExit(BsVmState& state) = 0;

    //! Triggered when there is a crash.
    //! \param state the state on the vm
    //! \param crash information structure
    virtual void OnCrash(BsVmState& state, const CrashInfo& crashInfo) = 0;
};

}
}

#endif
