/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderCollection.h
//! \author Kleber Garcia
//! \date   December 12th, 2014
//! \brief  Massive container for all the node types. Serves as a dynamic way of storing pegasus
//!         rendering graphs
#ifndef RENDER_COLLECTION_H
#define RENDER_COLLECTION_H
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Render/Render.h"

namespace Pegasus
{


//! All forward declarations used in the render collection
namespace Core {
    class IApplicationContext;
}
namespace Alloc {
    class IAllocator;
}

namespace Wnd {
    class Window;
}

namespace Shader {
    class ProgramLinkage;
    class ShaderStage;
}
namespace Texture {
    class Texture;
    class TextureGenerator;
    class TextureOperator;
}
namespace Mesh {
    class Mesh;
    class MeshGenerator;
    //class MeshOperator; TODO: implement any mesh operator! yet to be done :)
}

namespace Application {    
    class RenderCollectionImpl;
    class RenderCollection;
}

namespace Application
{

#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS 
    //! permission for script functions
    enum ScriptPermissions
    {
        PERMISSIONS_RENDER_GLOBAL_CACHE_READ  = (0x1 << 0x0), //!< allows reading the global cache
        PERMISSIONS_RENDER_GLOBAL_CACHE_WRITE = (0x1 << 0x1), //!< allows writting to the global cache
        PERMISSIONS_RENDER_API_CALL           = (0x1 << 0x2), //!< allows calls to the render API
        PERMISSIONS_ASSET_LOAD                = (0x1 << 0x3), //!< allows calls to the asset loading
        PERMISSIONS_DEFAULT                   = (0x0)         //!< default permissions
    };
#endif

    //! the global cache
    class GlobalCacheImpl;

    class GlobalCache
    {
    public:
        //! the listener callback to listen into cache events
        class IListener
        {
        public:
            virtual void OnGlobalCacheDirty() = 0;
        };

        //! Constructor
        explicit GlobalCache(Alloc::IAllocator* allocator);

        //! Destructor
        ~GlobalCache();

        //! string hash containing cache name
        typedef unsigned int CacheName;
        
        //! Registers an element of type T into global cache.
        //! If the name already exists, it will get replaced.
        //! \param cache - the cache that contains all resource elements
        //! \param name - the hashed name of the resource to register
        //! \param resource - the resource to store
        template<typename T>
        static void Register(GlobalCache* cache, GlobalCache::CacheName name, T* resource); 

        //! Find an element of type T in global cache.
        //! \param cache container
        //! \param name the hashed name of the resource to find
        //! \return If not found, returns null.
        template<typename T>
        static T* Find(GlobalCache* cache, GlobalCache::CacheName name);

        //! Adds listener to global cache
        //! \param listener the listener to register
        void AddListener(IListener* listener);

        //! Removes listener from global cache
        //! \param listener to remove the listener object registered.
        void RemoveListener(IListener* listener);

        //! Gets internal opaque implementation
        GlobalCacheImpl* GetImpl() { return mImpl; }

        //! Clears all internal resources and notifies listeners that it changed
        void Clear();

    private:

        void NotifyListeners();

        GlobalCacheImpl* mImpl;
        Utils::Vector<IListener*> mListeners;
        Alloc::IAllocator* mAlloc;
    };

    //! Implementation of a render collection factory
    class RenderCollectionFactory 
    {
    public:
        struct PropEntries
        {
            const char* mName;
            Utils::Vector<const char*> mProperties;
        };

        //! Constructor
        //!\param context - the application context
        //!\param alloc the allocator to use
        RenderCollectionFactory(Core::IApplicationContext* context, Alloc::IAllocator* alloc);

        //! Destructor
        ~RenderCollectionFactory();

        //! Registers a classes number of properties
        void RegisterProperties(const BlockScript::ClassTypeDesc& classDesc);

        //! Creates a render collection
        //!\param globalCache - the global cache that will hold resources shared across timeline blocks
        RenderCollection* CreateRenderCollection();

        //! Deletes a render collection
        void DeleteRenderCollection(RenderCollection* collection);

        //! Finds the layout definition for this node, cached from property grid meta type system
        //! \return pointer to entry describing the node's property layout
        const PropEntries* FindNodeLayoutEntry(const char* nodeTypeName) const;
    
    private:


        Utils::Vector<PropEntries> mPropLayoutEntries;
        
        Alloc::IAllocator* mAlloc;
    
        Core::IApplicationContext* mContext;
    };

    //! container of all the node types. Used by timeline blocks.
    class RenderCollection 
    {

        friend RenderCollectionFactory;

    public:

        typedef int CollectionHandle;
        static const CollectionHandle INVALID_HANDLE = -1;

        //! Destructor
        ~RenderCollection();

        //! \param R the resource type we want to get
        //! \param collection the collection containing the resources
        //! \param r the resource to store
        //! \return  the handle of the resource
        template<typename R>
        static CollectionHandle AddResource(RenderCollection* collection, R* r);

        //! \param R the resource type we want to get
        //! \param collection the collection containing the resources
        //! \return the count of these resource types
        template<typename R>
        static int ResourceCount(RenderCollection* collection);

        //! \param R the resource type we want to get
        //! \param handle the handle of such resource
        //! \return a pointer to the resource. If the resource is not found, null is returned
        template<typename R>
        static R* GetResource(RenderCollection* collection, CollectionHandle handle);

        //! \param R the typename R
        //! \param the name of the resource stored in the global cache
        template<typename R>
        static CollectionHandle ResolveResourceFromGlobalCache(RenderCollection* collection, GlobalCache::CacheName name);

        //! \param R the resource type we want to get
        //! \param collection which we want to extract the object
        //! \param handle the handle of such resource
        //! \param propertyId property offset which we want to use to access
        //! \return a pointer to the accessor.
        template<typename R>
        static const PropertyGrid::PropertyAccessor* GetPropAccessor(RenderCollection* collection, CollectionHandle objectHandle, int propertyId);

        //! updates all the internal resources.
        void UpdateAll();

        //! \return the application context pointer
        Core::IApplicationContext* GetAppContext() { return mContext; }

        //! Cleans / Deletes all references to the shaders/nodes/meshes/rendertargets/blendingstates/rasterstates being used
        void Clean();

        //! Set the window for this set of draw calls
        //! \param Window the window
        void SetWindow(Wnd::Window* wnd) { mCurrentWindow = wnd; }

        //! Get the current window
        //! \return the current window
        Wnd::Window* GetWindow() const { return mCurrentWindow; }

        //! Gets the factory
        RenderCollectionFactory* GetFactory() { return mFactory; }

        //! Gets the internal impl
        RenderCollectionImpl* GetImpl() { return mImpl; }

        //! Gets the internal allocator
        Alloc::IAllocator* GetAlloc() { return mAlloc; }

        //! Get reference to the global cache
        GlobalCache* GetGlobalCache() { return mGlobalCache; }

        //! Set the global cache.
        void SetGlobalCache(Application::GlobalCache* globalCache) { mGlobalCache = globalCache; }

        //! Sets this render collection to be using the global cache, registers a listener internally.
        //! note\ do not call if a global cache and a cache listener have not been set
        void SignalIsUsingGlobalCache();

        //! Sets the global cache listener
        void SetGlobalCacheListener(GlobalCache::IListener* listener) { mGlobalCacheListener = listener; }

#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
        //! Sets the permissions for this context call.
        void SetPermissions(ScriptPermissions permissions) { mPermissions = static_cast<unsigned>(permissions); }

        //! Gets the permissions for this context call.
        int GetPermissions() const { return mPermissions; }
#endif

    private:

        //! Constructor
        //!\param alloc the allocator to use internally
        //!\param context pointer to the application context
        RenderCollection(Alloc::IAllocator* alloc, RenderCollectionFactory* factory, Core::IApplicationContext* context);

        //! Removes any cache listener if there is one active
        void InternalRemoveGlobalCache();

        //! internal memory allocator
        Alloc::IAllocator* mAlloc;

        //! internal context
        Core::IApplicationContext* mContext;

        //! internal implementation
        RenderCollectionImpl* mImpl;

        //! factory pointer
        RenderCollectionFactory* mFactory;

        //! The current window
        Wnd::Window* mCurrentWindow;

        //! Reference to the global cache
        Application::GlobalCache* mGlobalCache;

        //! the global cache listener, for whant the global cache updates itself
        GlobalCache::IListener* mGlobalCacheListener;

#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
        //! Script permissions
        unsigned mPermissions;
#endif

        //! boolean that tags if this render collection is sensitive to cache changes or is pointing to a resource that 
        //! has been resolved in the global cache
        bool mIsUsingGlobalCache;
        
    };
}

}

#endif
