/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderSystem.h
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  Interface that defines the callbacks that a system requires.

#ifndef PEGASUS_RENDER_SYSTEMS_SYSTEM_H
#define PEGASUS_RENDER_SYSTEMS_SYSTEM_H

//! forward declarations
namespace Pegasus {

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

//! RenderSystem interface, inherit from this interface to create a new system
class RenderSystem
{

public:
    //! Constructor
    //! \param allocator the actual allocator used to register a system
    explicit RenderSystem(Alloc::IAllocator* allocator) : mAllocator(allocator) {}

    //! destructor
    virtual ~RenderSystem() {}

    //! API function to see if this system requires block script lib.
    //! \return true if we want to create a new blockscript api, false otherwise. If true, then OnRegisterBlockscriptApi gets created, and the new lib gets passed.
    virtual bool CanCreateBlockScriptApi() const { return false; }

    //! GetSystemNode
    //! \return the name of this system. Will be used to get included in the blockscript lib.
    virtual const char* GetSystemName() const = 0;

    //! Called only if this system can create a blockscript api.
    //! \param blocklib the target lib api to use to fill in any custom blockscript function.
    //! \param appContext app context to get used in case of accessing any other system while registering block lib.
    //! \note this function will only get called if CanCreateBlockScriptApi returns true.
    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext) {}

    //! Use this call to register any custom meshes via the "RegisterNode" function available on the meshManager
    //! \param meshManager the manager to use to register any custom mesh nodes.
    virtual void OnRegisterCustomMeshNodes(Mesh::MeshManager* meshManager) {}

    //! Use this call to register any custom texture nodes via the "RegisterNode" function available in the texture manager.
    virtual void OnRegisterCustomTextureNodes(Texture::TextureManager* textureManager) {}

protected:
    Alloc::IAllocator* mAllocator;

};

}
}

#endif
