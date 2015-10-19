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
BEGIN_IMPLEMENT_PROPERTIES2(WindowComponentState)
    IMPLEMENT_PROPERTY2(WindowComponentState, Enable)
END_IMPLEMENT_PROPERTIES2(WindowComponentState)

WindowComponentState::WindowComponentState()
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(BoxGenerator)
        INIT_PROPERTY2(Enable)
    END_INIT_PROPERTIES()
}

}
}
