/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ScriptTracker.cpp
//! \author Kleber Garcia
//! \date   3rd November 2014
//! \brief  Class that keeps book-keeping of all block scripts active
//! \brief  BlockScript master class. Main api to control / run / halt /disect a blockscript

#include "Pegasus/Timeline/ScriptTracker.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Allocator/IAllocator.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::BlockScript;
using namespace Pegasus::Utils;

ScriptTracker::ScriptTracker(Alloc::IAllocator* alloc)
    : mData(alloc)
{
}

//! Destructor
ScriptTracker::~ScriptTracker() {}

//! gets a script
//! \param the id of the script to get
//! \return the script pointer
TimelineScript* ScriptTracker::GetScriptById(int id)
{
    return mData[id];
}

//! Registers a script
void ScriptTracker::RegisterScript(TimelineScript* script)
{
    mData.PushEmpty() = script;
}

//! Removes script from tracker
void ScriptTracker::UnregisterScript(TimelineScript* script)
{
    for (int i = 0; i < mData.GetSize(); ++i)
    {
        if (mData[i] == script)
        {
            mData.Delete(i);
            return;
        }
    }
}
