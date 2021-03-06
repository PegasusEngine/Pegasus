/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ScriptTracker.h
//! \author Kleber Garcia
//! \date   3rd November 2014
//! \brief  Class that keeps book-keeping of all block scripts active


#ifndef PEGASUS_SCRIPT_TRACKER_H
#define PEGASUS_SCRIPT_TRACKER_H

#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{

    //fwd declarations
    namespace Alloc
    {
        class IAllocator;
    }
    
    namespace Timeline
    {
        class TimelineScript;
    }
}
   
namespace Pegasus
{
namespace Timeline
{
     

//! Script tracker class
class ScriptTracker
{
public:

    //! Constructor
    explicit ScriptTracker(Alloc::IAllocator* alloc);

    //! Destructor
    ~ScriptTracker();

    //! gets a script
    //! \param the id of the script to get
    //! \return the script pointer
    TimelineScript* GetScriptById(int id);

    //! Gets the script count
    //! \return the count
    int GetScriptCount() const { return mData.GetSize(); }

    //! Registers a script
    void RegisterScript(TimelineScript* script);

    //! Removes script from tracker
    void UnregisterScript(TimelineScript* script);
    
private:
    Utils::Vector<TimelineScript*> mData;
    
};

}
}

#endif
