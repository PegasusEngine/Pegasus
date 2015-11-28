/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BsVm.cpp
//! \author Kleber Garcia
//! \date   20th September 2014
//! \brief  Pegasus blockscript virtual machine, runs a canonical pegasus tree

#ifndef BSVM_H
#define BSVM_H

#include "Pegasus/BlockScript/BlockScriptCanon.h"
#include "Pegasus/BlockScript/Container.h"

namespace Pegasus
{
namespace Alloc
{
    class IAllocator;
}

namespace BlockScript
{

//! Forward declarations
class BsVmState;
class IRuntimeListener;

// memory and register state of the current virtual machine
class BsVmState
{
    friend class BsVm;
public:
    
    // Constructor
    BsVmState();

    //! Destructor
    ~BsVmState();

    //! Initializes the allocator for this state structure
    void Initialize(Alloc::IAllocator* allocator);

    //! Resets the state of this structure
    void Reset();

    //! Sets the user context (to be acquired by callbacks in the need of so).
    void SetUserContext(void* userContext) { mUserContext = userContext; }

    //! Gets the user context 
    void* GetUserContext() const { return mUserContext; }

    //! Set the runtime event listener
    void SetRuntimeListener(IRuntimeListener* runtimeListener) { mRuntimeListener = runtimeListener; }

    //! Get the runtime event listener
    IRuntimeListener* GetRuntimeListener() const { return mRuntimeListener; }
    
    // gets registers
    int  GetReg(Canon::Register reg) const { return mR[reg]; }
    void  SetReg(Canon::Register reg, int val)  { mR[reg] = val; }

    int* GetRegBuffer() { return mR; }

    int GetRamSize() const { return mRamSize; }

    char* Ram() { return mRam; }


    // Grows the memory stack. Reallocates memory if not big enough
    void Grow(int bytes);
    void Shrink(int bytes);

    struct HeapElement
    {
        void* mObject;
        const TypeDesc* mTypeDesc;
    public:
        HeapElement() : mObject(nullptr), mTypeDesc(nullptr) {}
    };


    int PushHeapElement(void* object, const TypeDesc* typeDesc)
    {
        HeapElement& el = mHeapContainer.PushEmpty();
        el.mObject = object;
        el.mTypeDesc = typeDesc;
        return mHeapContainer.Size() - 1;
    }

    HeapElement& GetHeapElement(int indexPointer)
    {
        return mHeapContainer[indexPointer];
    }

    int GetStackLevels() const { return mStackLevels; }

    void IncStackLevels() { ++mStackLevels; }

    void DecStackLevels() { --mStackLevels; }

private:

    // the user context
    void* mUserContext;

    // memory ram (stack)
    char* mRam;
    int   mRamCount;
    int   mRamSize;

    // registers
    int  mR[Canon::R_COUNT];

    //stack metadata
    int mStackLevels;

    // allocator
    Alloc::IAllocator* mAllocator;

    //! heap random access lookup.
    //! every heap object reference has an id passed around.
    Container<HeapElement> mHeapContainer;

    //! Runtime listener
    IRuntimeListener* mRuntimeListener;
};

//actual virtual machine modifying the state
class BsVm
{
public:
    //! constructor
    BsVm(){}

    //! destructor
    ~BsVm(){}

    //! Runs this assembly and modifies the virtual machine state of such
    void Run(const Assembly& assembly, BsVmState& state) const;

    //! steps execution (one instruction).
    //! \param the actual state
    //! \return true if execution continues, false if exit requested
    bool StepExecution(const Assembly& assembly, BsVmState& state) const;
};

}
}

#endif
