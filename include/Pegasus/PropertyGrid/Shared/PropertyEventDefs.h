/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridEventDefs.h
//! \author	Kleber Garcia
//! \date	15
//! \brief	Script helper for scripting callbacks

#ifndef PROPERTYGRID_EVENT_DEFS_H
#define PROPERTYGRID_EVENT_DEFS_H

#include "Pegasus/Core/Shared/EventDefs.h"

namespace Pegasus {
namespace PropertyGrid {

    class IPropertyGridObjectProxy;

    //! Event containing the property index which event was fired.
    class ValueChangedEventIndexed
    {
    public:
        explicit ValueChangedEventIndexed(int index) : mIndex(index) {}
        ~ValueChangedEventIndexed(){}

        //! gets the index of the property whose value changed.
        int GetIndex() const { return mIndex; }

    private:
        int mIndex;
    };

    //! Event fired when this event gets destroyed
    class PropertyGridDestroyed
    {
    public:
        PropertyGridDestroyed() {}
        ~PropertyGridDestroyed() {}
    };

    
    PEGASUS_EVENT_BEGIN_REGISTRY (IPropertyListener)
        PEGASUS_EVENT_REGISTER(ValueChangedEventIndexed);
        PEGASUS_EVENT_REGISTER(PropertyGridDestroyed);
    PEGASUS_EVENT_END_REGISTRY

}//namespace PropertyGrid
}//namespace Pegasus

#endif //PROPERTYGRID_EVENT_DEFS_H
