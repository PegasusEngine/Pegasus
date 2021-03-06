/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.h
//! \author	Karolyn Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#ifndef PEGASUS_TIMELINE_BLOCK_H
#define PEGASUS_TIMELINE_BLOCK_H

#include "Pegasus/Timeline/Shared/BlockDefs.h"
#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/TimelineScriptRunner.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/Category.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"

namespace Pegasus {
    namespace AssetLib {
        class AssetLib;
        class Asset;
        class Object;
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
        class Category; 
#endif
    }

    namespace Timeline {
        class Lane;
        class Timeline;
        struct UpdateInfo;
        struct RenderInfo;
    }

    namespace Wnd {
        class Window;
    }
}

namespace Pegasus {
namespace Timeline {


//! Timeline block, describing the instance of an effect on the timeline
class Block : public PropertyGrid::PropertyGridObject
{
    DECLARE_TIMELINE_BLOCK(Block, "Block");


    BEGIN_DECLARE_PROPERTIES_BASE(Block)
        DECLARE_PROPERTY(Math::Color8RGB, Color, Math::Color8RGB(128,128,128));
        DECLARE_PROPERTY(unsigned int, Beat, 0);
        DECLARE_PROPERTY(unsigned int, Duration, 1);
    END_DECLARE_PROPERTIES()

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    Block(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~Block();

    //! Get the lane the block belongs to
    //! \return Lane the block belongs to, nullptr when the block is not associated with a lane yet
    inline Lane * GetLane() const { return mLane; }

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the block
    //! \return Proxy associated with the block
    //@{
    inline BlockProxy * GetProxy() { return &mProxy; }
    inline const BlockProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Attempts to open and compile a script. True if success, false otherwise.
    //! \param script reference to attach
    void AttachScript(TimelineScriptInOut script);

    //! Removes a script
    void ShutdownScript();

    //! Returns the script of this block, null if none is attached.
    //! \return the script object, null if not attached
    TimelineScriptReturn GetScript();

    //! Update the content of the block, called once at the beginning of each rendered frame
    //! \param updateInfo Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    virtual void Update(const UpdateInfo& updateInfo);

    //! Render the content of the block
    //! \param renderInfo - beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    virtual void Render(const RenderInfo& renderInfo);

    //! Callback for when a window is created.
    virtual void OnWindowCreated(int windowIndex);

    //! Callback for when a window is destroyed.
    virtual void OnWindowDestroyed(int windowIndex);

    //! Gets the internal script runner of this block.
    TimelineScriptRunner& GetScriptRunner() { return mScriptRunner; }

    //------------------------------------------------------------------------------------
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    AssetLib::Category* GetAssetCategory() { return &mCategory; } 
#endif

#if PEGASUS_ENABLE_PROXIES
    unsigned GetGuid() const { return mGuid; }

    //! Overrides a guid for a block. Only to be used for proper behaviour of doing / undoing state.
    //! \param newGuid the guid to use for this object
    //! \warning there is no check for duplicate of guids. So make sure the right one is used.
    void OverrideGuid(unsigned int newGuid);

    //! dumps the contents of a block to a single asset.
    //! Note- this is only supported for the use case of Undo/Redo, which serializes the entire state of an object as json
    //! \param assetProxy target asset to dump state into
    virtual void DumpToAsset(Pegasus::AssetLib::Asset* asset);

    //! loads the contents of a block from a single asset.
    //! Note- this is only supported for the use case of Undo/Redo, which serializes the entire state of an object as json
    //! \param assetProxy target asset to load state from
    virtual void LoadFromAsset(const Pegasus::AssetLib::Asset* asset);
#endif

protected:

    //! Get the asset manager
    inline AssetLib::AssetLib* GetAssetLib() const { return mAppContext->GetAssetLib(); }
    
    //! Get the global IO manager
    //! \return Global IO manager
    inline Io::IOManager * GetIOManager() const { return mAppContext->GetIOManager(); }

    //! Get the node manager
    //! \return Global node manager
    inline Graph::NodeManager * GetNodeManager() const { return mAppContext->GetNodeManager(); }

    //! Get the shader manager
    //! \return Global shader manager
    inline Shader::ShaderManager * GetShaderManager() const { return mAppContext->GetShaderManager(); }

    //! Get the texture manager
    //! \return Global texture manager
    inline Texture::TextureManager * GetTextureManager() const { return mAppContext->GetTextureManager(); }

    //! Get the mesh manager
    //! \return Global mesh manager
    inline Mesh::MeshManager * GetMeshManager() const { return mAppContext->GetMeshManager(); }

    //! Get the bs manager 
    //! \return Global bs manager
    inline BlockScript::BlockScriptManager * GetBlockScriptManager() const { return mAppContext->GetBlockScriptManager(); }

    //! Get the timeline
    //! \return Global timeline
    inline TimelineManager * GetTimelineManager() const { return mAppContext->GetTimelineManager(); }

    //! Callback that reads from an asset root and populates properties / creates objects based on such.
    //! \param lib the asset library.
    //! \param owner the asset containing the root object
    //! \param root the root object of the structured asset
    //! \return true if successful, false otherwise
    bool OnReadObject(Pegasus::AssetLib::AssetLib* lib, const AssetLib::Asset* owner, const AssetLib::Object* root);

    //! Callback that reads from an asset root and populates properties / creates objects based on such.
    //! \param lib the asset library.
    //! \param owner the asset containing the root object
    //! \param root the root object of the structured asset
    //! \return true if successful, false otherwise
    void OnWriteObject(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* owner, AssetLib::Object* root);


    //------------------------------------------------------------------------------------

private:

    // Functions accessible only from the Lane object, to prevent the sorted linked list
    // of the owner lane to lose its sorting
    friend class Lane;

    //! Set the lane the block belongs to
    //! \param lane Lane the block belongs to (!= nullptr)
    void SetLane(Lane * lane);

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(Block)


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Core::IApplicationContext* mAppContext;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the block
    BlockProxy mProxy;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Position of the block in the lane, measured in ticks
    Beat mBeat;

    //! Duration of the block, measured in ticks (> 0)
    Duration mDuration;

    //! Lane the block belongs to, nullptr when the block is not associated with a lane yet
    Lane * mLane;

    //! Coordinator for loading / running of scripts
    TimelineScriptRunner mScriptRunner;

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    AssetLib::Category mCategory; 
#endif

#if PEGASUS_ENABLE_PROXIES
    unsigned mGuid;
    bool mWindowIsInitialized[PEGASUS_MAX_WORLD_WINDOW_COUNT];
#endif
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_BLOCK_H
