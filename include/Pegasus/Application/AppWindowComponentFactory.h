/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppWindowComponentFactory.h
//! \author Kleber Garcia
//! \date   9/9/2015
//! \brief  Master class that contains all the components of an application.

#ifndef PEGASUS_APP_WND_COMPONENT_H
#define PEGASUS_APP_WND_COMPONENT_H

#include "Pegasus/Application/Shared/ApplicationConfig.h"

namespace Pegasus {
    namespace Wnd {
        class Window;
        class IWindowComponent;
    }

    namespace Alloc {
        class IAllocator;
    }

    namespace Core {
        class IApplicationContext;
    }
}

namespace Pegasus {
namespace App {


class AppWindowComponentFactory
{
public:
    //! Constructor
    AppWindowComponentFactory(Alloc::IAllocator* allocator);

    //! Destructor
    ~AppWindowComponentFactory();

    //! Loads and creates all component instances.
    void LoadAllComponents(Core::IApplicationContext* appContext);

    //! Unloads / destroys all component instances.
    void UnloadAllComponents(Core::IApplicationContext* appContext);

    //! Attaches a set of component instances to a window.
    //! window - the window that will hold the components
    //! componentList - ordered list (rendering & update order) with the components to attach to this window.
    //! componentListCount - the count.    
    void AttachComponentsToWindow(Wnd::Window* window, ComponentTypeFlags components);

    //! Calls update inside all components
    void UpdateAllComponents(Core::IApplicationContext* appContext);

private:

    Wnd::IWindowComponent* mComponentInstances[COMPONENT_COUNT];
    Alloc::IAllocator* mAllocator;

};

}
}

#endif
