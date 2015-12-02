/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockRuntimeScriptListener.h
//! \author	Kleber Garcia
//! \date	29th November 2015
//! \brief	class containing callbacks that act on a block.
//!         these callbacks are triggered from a script running.

#ifndef PEGASUS_TIMELINE_BLOCK_SCRIPT_RUNTIME_LISTENER_H
#define PEGASUS_TIMELINE_BLOCK_SCRIPT_RUNTIME_LISTENER_H

#include "Pegasus/BlockScript/EventListeners.h"
#include "Pegasus/Utils/Vector.h"

//! forward declarations
namespace Pegasus {
    namespace Timeline {
        class Block;
    }

    namespace PropertyGrid {
        class PropertyGridObject;
    }

    namespace BlockScript {
        class BlockScript;
        class BsVmState;
    }
}

namespace Pegasus {
namespace Timeline {

//forward declaration
struct VarTranslation;

//! class that listens for runtime events in blockscript.
class BlockRuntimeScriptListener : public BlockScript::IRuntimeListener
{
public:

    enum UpdateType
    {
        NOTHING,       //do no update
        RERUN_GLOBALS, //this means we destroy everything, and rerun the entire script (global scope).
        RERUN_RENDER   //this means we just call render and refresh the frame.
    };

    BlockRuntimeScriptListener() 
    : mPropGrid(nullptr)
    , mScript(nullptr) {}

    virtual ~BlockRuntimeScriptListener() {}

    //! Initializes this object.
    void Initialize(PropertyGrid::PropertyGridObject* prop, Pegasus::BlockScript::BlockScript* script) 
    {
        mPropGrid = prop;
        mScript = script;
    }

    //! Shut downs this listener
    void Shutdown()
    {
        mPropGrid = nullptr;
        mScript = nullptr;
    }

    //! Triggered when the runtime has not triggered the first instruction
    //! \param state the runtime vm state
    virtual void OnRuntimeBegin(Pegasus::BlockScript::BsVmState& state);

    //! Triggered when the first stack command has been triggered
    //! \param state - the runtime state that has been initialized
    virtual void OnStackInitalized(Pegasus::BlockScript::BsVmState& state);

    //! Triggered when the runtime has exited. The global scope is still alive at this point
    //! so functions can be called even after exiting the Vm.
    //! \param state the runtime vm state
    virtual void OnRuntimeExit(Pegasus::BlockScript::BsVmState& state);

    //! Triggered when there is a crash.
    //! \param state the state on the vm
    //! \param crash information structure
    virtual void OnCrash(Pegasus::BlockScript::BsVmState& state, const Pegasus::BlockScript::CrashInfo& crashInfo);

    //! Copy the property in the current index to a valid property inside the list of globals in blockscript.
    //! index - property to flush (index of property grid)
    //! \return update type to display the changes in properties done by the timeline
    UpdateType FlushProperty(Pegasus::BlockScript::BsVmState& state, unsigned int index);

    //! \return true if ready, false otherwise
    bool IsReady() const { return mPropGrid != nullptr && mScript != nullptr; }

private:
    //! varDef - definition of the variable (type name size etc)
    //! pgIndex - property grid index (source data)
    //! state - the blockscript vm state (where the memory to write to lies)
    //! bsIndex - the blockscript index so we can write to it
    void SetScriptVariable(
        const VarTranslation& varDef,
        unsigned int pgIndex,
        Pegasus::BlockScript::BsVmState& state,
        int bsIndex
    );

    PropertyGrid::PropertyGridObject* mPropGrid;
    Pegasus::BlockScript::BlockScript* mScript;
    Utils::Vector<int> mGridToGlobalMap;

};

}
}

#endif//define PEGASUS_TIMELINE_BLOCK_SCRIPT_RUNTIME_LISTENER_H
