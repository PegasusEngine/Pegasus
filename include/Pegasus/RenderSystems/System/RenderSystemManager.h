/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderSystemManager.h
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  Main app entry point to manage systems. Use this class to register custom systems from the app

#ifndef PEGASUS_RENDER_SYSTEM_MANAGER_H
#define PEGASUS_RENDER_SYSTEM_MANAGER_H

#include "Pegasus/Utils/Vector.h"

//! forward declarations

namespace Pegasus {

    namespace RenderSystems {
        class RenderSystem;
    }

    namespace Core {
        class IApplicationContext;
    }

    namespace BlockScript {
        class BlockLib;
    }

    namespace Alloc {
        class IAllocator;
    }

    namespace Mesh {
        class MeshManager;
    }

    namespace Texture {
        class TextureManager;
    }
}

namespace Pegasus
{
namespace RenderSystems
{

//! Master manager class that holds references to the systems.
class RenderSystemManager
{

public:

    
    //Render system container.
    struct RenderSystemContainer
    {
        unsigned int hash;
        RenderSystem* system;
        Alloc::IAllocator* allocator;
        BlockScript::BlockLib* blockscriptLib;
    };

    //! Constructor.
    RenderSystemManager(Alloc::IAllocator* allocator, Core::IApplicationContext* appContext) : mAllocator(allocator), mAppContext(appContext), mSystems(allocator) {}

    //! Destructor.
    ~RenderSystemManager();

    //! API call that adds the internal systems defined here. Use Config.h to remove / add pre-existing systems 
    //! for example, some apps might not want to use all the systems to save code space.
    void AddInternalSystems();

    //! Call this function externally from the user app in order to add any new systems that come externally.
    //! \param ownerAlloc the owner allocator that instantiated this sytem.
    //! \param system the system to register
    void RegisterSystem(Alloc::IAllocator* ownerAlloc, RenderSystem* system);

    //! Find a system by its name. If not found, return null.
    //! \param systemName the system name to find.
    //! \return the found system. If not found, returns null.
    const RenderSystemContainer* FindSystem(const char* systemName);

    //! Call that initializes all the systems internally, registers its nodes and blockscript calls.
    //! \param appContext the application context so we can initialize the systems.
    void InitializeSystems(Core::IApplicationContext* appContext);

    //! Destroys the desired systems.
    void DestroyAllSystems();

    //! Gets a reference to a render system.
    RenderSystem* GetSystem(unsigned int i) { return mSystems[i].system; }

    //! Gets the total count of registered systems.
    unsigned int GetSystemCount() { return mSystems.GetSize(); }

    //! Gets the list of exported libs
    Utils::Vector<BlockScript::BlockLib*>& GetLibs() { return mLibs; }

private:
    Alloc::IAllocator* mAllocator;
    Core::IApplicationContext* mAppContext;

    Utils::Vector<RenderSystemContainer> mSystems;
    Utils::Vector<BlockScript::BlockLib*> mLibs;

};

}
}

#endif
