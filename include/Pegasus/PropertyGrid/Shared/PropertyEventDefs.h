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
        explicit ValueChangedEventIndexed(PropertyCategory category, int index) : mCategory(category), mIndex(index) {}
        ~ValueChangedEventIndexed(){}

        //! Gets the category of the property whose value changed.
        PropertyCategory GetCategory() const { return mCategory; }

        //! Gets the index of the property whose value changed.
        int GetIndex() const { return mIndex; }

    private:
        PropertyCategory mCategory;
        int mIndex;
    };

    //!  Event signaling that the layout of the object properties has changed (properties added or removed).
    class ObjectPropertiesLayoutChanged
    {
    public:
        ObjectPropertiesLayoutChanged(){}
        ~ObjectPropertiesLayoutChanged(){}
    };

    //! Event fired when this event gets destroyed
    class PropertyGridDestroyed
    {
    public:
        PropertyGridDestroyed() {}
        ~PropertyGridDestroyed() {}
    };

    //! Event fired when a property requests a redraw of the app
    class PropertyGridRenderRequest
    {
    public:
        PropertyGridRenderRequest() {}
        ~PropertyGridRenderRequest() {}
    };

    
    PEGASUS_EVENT_BEGIN_REGISTRY (IPropertyListener)
        PEGASUS_EVENT_REGISTER(ValueChangedEventIndexed);
        PEGASUS_EVENT_REGISTER(ObjectPropertiesLayoutChanged);
        PEGASUS_EVENT_REGISTER(PropertyGridDestroyed);
        PEGASUS_EVENT_REGISTER(PropertyGridRenderRequest);
    PEGASUS_EVENT_END_REGISTRY

}//namespace PropertyGrid
}//namespace Pegasus

#endif //PROPERTYGRID_EVENT_DEFS_H
