/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowComponentState.h
//! \author Kleber Garcia
//! \date   10/15/2015
//! \brief  State of a window component

#ifndef PEGASUS_WND_COMPONENT_STATE
#define PEGASUS_WND_COMPONENT_STATE

#include "Pegasus/PropertyGrid/PropertyGridObject.h"

namespace Pegasus 
{
namespace Wnd
{

//! Struct representing the state of this context.
//! Every component that requires state per window needs to implement / create a state for itself.
class WindowComponentState : public PropertyGrid::PropertyGridObject
{
    BEGIN_DECLARE_PROPERTIES_BASE(WindowComponentState)
        DECLARE_PROPERTY(bool, Enable, true)
    END_DECLARE_PROPERTIES()

public:
    WindowComponentState();
    virtual ~WindowComponentState() {}
};

}
}

#endif
