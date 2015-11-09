/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowComponentState.cpp
//! \author Kleber Garcia
//! \date   10/15/2015
//! \brief  State of a window component

#include "Pegasus/Window/WindowComponentState.h"

namespace Pegasus
{
namespace Wnd
{

//! Property implementations
BEGIN_IMPLEMENT_PROPERTIES(WindowComponentState)
    IMPLEMENT_PROPERTY(WindowComponentState, Enable)
END_IMPLEMENT_PROPERTIES(WindowComponentState)

WindowComponentState::WindowComponentState()
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(BoxGenerator)
        INIT_PROPERTY(Enable)
    END_INIT_PROPERTIES()
}

}
}
