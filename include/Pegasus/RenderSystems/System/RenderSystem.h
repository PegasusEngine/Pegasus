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

#include "Pegasus/Render/Render.h"
#include "Pegasus/Utils/Vector.h"

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

    namespace Wnd {
        class Window;
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
    struct ShaderGlobalConstantDesc
    {
        const char* constantName;
        Render::BufferRef buffer;
    };

    //! Constructor
    //! \param allocator the actual allocator used to register a system
    explicit RenderSystem(Alloc::IAllocator* allocator) : mAllocator(allocator), mId(sNextId++) {}

    //! destructor
    virtual ~RenderSystem() {}

    //! Internal function, called to initialize and load any render resources required.
    virtual void Load(Core::IApplicationContext* appContext) {}

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

    //! Use this call to register global shader constants.
    virtual void OnRegisterShaderGlobalConstants(Utils::Vector<ShaderGlobalConstantDesc>& outConstants) {}

    //! Use this call to perform window specific computations.
    //! width - the window's width
    //! height - the window's height
    virtual void WindowUpdate(unsigned int width, unsigned int height) {}

    //! Gets the id of a render system.
    int GetId() const { return mId; }

protected:
    Alloc::IAllocator* mAllocator;
    static int sNextId;
    int mId;

};

}
}

#endif
