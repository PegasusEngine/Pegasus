/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderCollection.cpp
//! \author Kleber Garcia
//! \date   December 12th, 2014
//! \brief  Massive container for all the node types. Serves as a dynamic way of storing pegasus
//!         rendering graphs

#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"

using namespace Pegasus::Utils;

namespace Pegasus
{

namespace Application
{

    //! Structure representing a property information for a particular object
    struct PropertyInfo
    {
        PropertyGrid::PropertyAccessor mCachedAccessor;
        int   mSize;
        bool  mValid;
        PropertyInfo() : mSize(0), mValid(false) {}
    };

    template<typename NodeType, bool hasProperties>
    struct PropertyExtraInfo
    {
        void Initialize(const char* nodeName, const RenderCollectionFactory* factory, NodeType* node, Alloc::IAllocator* allocator) {}
        void Reset() {}
        PropertyInfo* GetPropertyInfo(int propertyId) { return nullptr; }
    };

    //specialization
    template<typename NodeType>
    struct PropertyExtraInfo<NodeType, true>
    {
        PropertyExtraInfo()
        : mCachedInfos(nullptr), mCachedInfoCount(0), mAllocator(nullptr) {}

        void Initialize(const char* nodeName, const RenderCollectionFactory* factory, NodeType* node, Alloc::IAllocator* allocator)
        {
            mAllocator = allocator;

            //find the processed description of this node.
            const RenderCollectionFactory::PropEntries* entryLayout = factory->FindNodeLayoutEntry(nodeName);
            
            if (entryLayout != nullptr && entryLayout->mProperties.GetSize() > 0)
            {
                mCachedInfos = PG_NEW_ARRAY(allocator, -1, "NodePropCache", Pegasus::Alloc::PG_MEM_TEMP, PropertyInfo, entryLayout->mProperties.GetSize());
                mCachedInfoCount = entryLayout->mProperties.GetSize();
            }
    
            //walk the node's properties and cache them.
            for (unsigned int i = 0; i < node->GetNumClassProperties(); ++i)
            {
                const PropertyGrid::PropertyRecord& record = node->GetClassPropertyRecord(i);
                for (unsigned int propId = 0; propId < entryLayout->mProperties.GetSize(); ++propId)
                {
                    if (!Utils::Strcmp(record.name, entryLayout->mProperties[propId]))
                    {
                        mCachedInfos[propId].mValid = true;
                        mCachedInfos[propId].mSize  = record.size;
                        mCachedInfos[propId].mCachedAccessor = node->GetClassPropertyAccessor(static_cast<unsigned>(i));
                        break;
                    }
                }
            }
        }

        void Reset()
        {
            if (mCachedInfoCount > 0)
            {
                PG_DELETE_ARRAY(mAllocator, mCachedInfos);
                mCachedInfos = nullptr;
                mCachedInfoCount = 0;
            }
        }

        PropertyInfo* GetPropertyInfo(int propertyId)
        {
            PG_ASSERT(propertyId < mCachedInfoCount); 
            if (propertyId >= 0 && propertyId < mCachedInfoCount)
            {
                return mCachedInfos[propertyId].mValid ? &mCachedInfos[propertyId] : nullptr;
            }
            return nullptr;
        }

        ~PropertyExtraInfo()
        {
            Reset();
        }

        PropertyInfo* mCachedInfos;
        int mCachedInfoCount;
        Pegasus::Alloc::IAllocator* mAllocator;
        
    };

    template<class T, bool hasProperties>
    struct ObjectPropertyCache
    {
        Core::Ref<T> mObject;
        Alloc::IAllocator* mAllocator;
        PropertyExtraInfo<T, hasProperties> mInfo;
        
        ObjectPropertyCache()
        : mAllocator(nullptr) {}

        void Initialize(const char* nodeName, const RenderCollectionFactory* factory,  T* node, Alloc::IAllocator* allocator)
        {
            mObject = node;
            mInfo.Initialize(nodeName, factory, node, allocator);
        }

        void Reset()
        {
            mObject = nullptr;
            mInfo.Reset();
        }

        ~ObjectPropertyCache()
        {
            Reset();
        }
    
    };

    
    RenderCollectionFactory::RenderCollectionFactory(Core::IApplicationContext* context, Alloc::IAllocator* alloc)
        :mAlloc(alloc), mPropLayoutEntries(alloc), mContext(context)
    {
    }

    RenderCollectionFactory::~RenderCollectionFactory()
    {
    }

    void RenderCollectionFactory::RegisterProperties(const BlockScript::ClassTypeDesc& classDesc)
    {
        RenderCollectionFactory::PropEntries& entry = mPropLayoutEntries.PushEmpty();
        entry.mName = classDesc.classTypeName;
        for (int i = 0; i < classDesc.propertyCount; ++i)
        {
            entry.mProperties.PushEmpty() = classDesc.propertyDescriptors[i].propertyName;
        }
    }

    RenderCollection* RenderCollectionFactory::CreateRenderCollection()
    {
        return PG_NEW(mAlloc, -1, "RenderCollection", Pegasus::Alloc::PG_MEM_TEMP) RenderCollection(mAlloc, this, mContext);
    }

    void RenderCollectionFactory::DeleteRenderCollection(RenderCollection* toDelete)
    {
        PG_DELETE(mAlloc, toDelete);
    }

    const RenderCollectionFactory::PropEntries* RenderCollectionFactory::FindNodeLayoutEntry(const char* nodeTypeName) const
    {
        for (unsigned int i = 0; i < mPropLayoutEntries.GetSize(); ++i)
        {
            const RenderCollectionFactory::PropEntries& entry = mPropLayoutEntries[i];
            if (!Utils::Strcmp(nodeTypeName, entry.mName))
            {
                return &entry;
            }
        }
        return nullptr;
    }

    class RenderCollectionImpl
    {
    public:
        explicit RenderCollectionImpl(Alloc::IAllocator * alloc);
        ~RenderCollectionImpl() { Clean(); }
        void Clean();
    
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) Utils::Vector< ObjectPropertyCache<type, hasProperties> > instance;
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS
        Pegasus::Alloc::IAllocator* mAlloc;
    };

    template<typename T, bool hasProperties>
    static Utils::Vector<ObjectPropertyCache<T, hasProperties> >* GetContainer(RenderCollectionImpl* impl)
    {
        return nullptr;
    }
    
    #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) \
        template<> static Utils::Vector< ObjectPropertyCache<type,hasProperties > >* GetContainer<type, hasProperties>(RenderCollectionImpl* impl) \
        { \
            return &impl->instance; \
        }
    #include "../Source/Pegasus/Application/RenderResources.inl"
    #undef RES_PROCESS
    
    template<typename T>
    struct ResourceTrait
    {
        static const char* GetMetaName() { return "???" };
        static const bool HasProperties = false;
        static const bool CanUpdate = false;
    };
    #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) \
        template<>\
        struct ResourceTrait<type>\
        {\
            static const char* GetMetaName() { return metaname; }\
            static const bool HasProperties = hasProperties;\
            static const bool CanUpdate = canUpdate;\
        };
    #include "../Source/Pegasus/Application/RenderResources.inl"
    #undef RES_PROCESS

    RenderCollectionImpl::RenderCollectionImpl(Alloc::IAllocator * alloc)
        :
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) instance(alloc),
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS
        mAlloc(alloc)
    {
    }

    void RenderCollectionImpl::Clean()
    {
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) instance.Clear();
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS
    }

    RenderCollection::RenderCollection(Alloc::IAllocator* alloc, RenderCollectionFactory* factory, Core::IApplicationContext* context)
    : mAlloc(alloc),
      mContext(context),
      mFactory(factory),
      mIsUsingGlobalCache(false),
      mCurrentRenderInfo(nullptr),
      mGlobalCache(nullptr),
      mGlobalCacheListener(nullptr)
#if PEGASUS_ENABLE_SCRIPT_PERMISSIONS
      ,mPermissions(PERMISSIONS_DEFAULT)
#endif
    {
        mImpl = PG_NEW(alloc, -1, "RenderCollectionImpl", Alloc::PG_MEM_TEMP) RenderCollectionImpl(alloc);
    }

    RenderCollection::~RenderCollection()
    {
        InternalRemoveGlobalCache();
        PG_DELETE(mAlloc, mImpl);
    }

    void RenderCollection::InternalRemoveGlobalCache()
    {
        if (mIsUsingGlobalCache)
        {
            PG_ASSERTSTR(mGlobalCache != nullptr, "There must be a global cache on this container.");
            PG_ASSERTSTR(mGlobalCacheListener != nullptr, "there must be a global cache listener on this container");
            mIsUsingGlobalCache = false;
            mGlobalCache->RemoveListener(mGlobalCacheListener);
            mGlobalCache = nullptr;
            mGlobalCacheListener = nullptr;
        }
    }

    void RenderCollection::Clean()
    {
        InternalRemoveGlobalCache();
        mImpl->Clean();
    }

    void RenderCollection::SignalIsUsingGlobalCache()
    {
        if (!mIsUsingGlobalCache)
        {
            PG_ASSERTSTR(mGlobalCache != nullptr, "There must be a global cache on this container.");
            PG_ASSERTSTR(mGlobalCacheListener != nullptr, "there must be a global cache listener on this container");
            mGlobalCache->AddListener(mGlobalCacheListener);
            mIsUsingGlobalCache = true;
        }
    }

    //define some traits
    
    template<typename R>
    static RenderCollection::CollectionHandle AddResourceInternal(RenderCollection* collection, R* r)
    {
        auto* container = GetContainer<R,ResourceTrait<R>::HasProperties>(collection->GetImpl());
        RenderCollection::CollectionHandle h = container->GetSize(); 
        container->PushEmpty().Initialize(ResourceTrait<R>::GetMetaName(), collection->GetFactory(),  r, collection->GetAlloc());
        return h;
    }
    
    template<typename R>
    static int ResourceCountInternal(RenderCollection* collection)
    {
        auto* container = GetContainer<R, ResourceTrait<R>::HasProperties>(collection->GetImpl());
        return container->GetSize();
    }
    
    template<typename R>
    static R* GetResourceInternal(RenderCollection* collection, RenderCollection::CollectionHandle handle)
    {
        auto* container = GetContainer<R,ResourceTrait<R>::HasProperties>(collection->GetImpl());
        if (handle < 0 || handle >= static_cast<RenderCollection::CollectionHandle>(container->GetSize()))
        {
            return nullptr;
        }
        return (*container)[handle].mObject;
    }

    template<typename R>
    static const PropertyGrid::PropertyAccessor* GetPropAccessorInternal(RenderCollection* collection, RenderCollection::CollectionHandle objectHandle, int propertyId)
    {
        if (objectHandle == RenderCollection::INVALID_HANDLE) 
        {
            PG_LOG('ERR_', "Property requested from invalid object.");
            return nullptr;
        }

        auto* container = GetContainer<R,ResourceTrait<R>::HasProperties>(collection->GetImpl());
        auto& cachedInfo = (*container)[objectHandle];

        PropertyInfo* info = cachedInfo.mInfo.GetPropertyInfo(propertyId);
        return info ? &info->mCachedAccessor : nullptr;
    }

    template<typename R> 
    static RenderCollection::CollectionHandle ResolveResourceFromGlobalCacheInternal(RenderCollection* collection, GlobalCache::CacheName name)
    {
        
        collection->SignalIsUsingGlobalCache();

        if (collection->GetGlobalCache() == nullptr)
        {
            return RenderCollection::INVALID_HANDLE;
        }

        R* resource = GlobalCache::Find<R>(collection->GetGlobalCache(), name);
        if (resource == nullptr)
        {
            return RenderCollection::INVALID_HANDLE;
        }


        return RenderCollection::AddResource<R>(collection, resource);
    }

    template<typename R, bool canUpdate>
    struct Updater {
        static void TemplateUpdateAll(Utils::Vector<ObjectPropertyCache<R, ResourceTrait<R>::HasProperties> >& resList)
        {
            for (unsigned i = 0; i < resList.GetSize(); ++i)
            {
                resList[i].mObject->Update();
            }
        }
    };

    template<typename R >
    struct Updater<R, false> { 
        static void TemplateUpdateAll(Utils::Vector<ObjectPropertyCache<R, ResourceTrait<R>::HasProperties> >& resList)
        {
            //no update
        }
    };

    void RenderCollection::UpdateAll()
    {
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) \
            Updater<type,canUpdate>::TemplateUpdateAll(mImpl->instance);
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS
    }

    #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) \
        template<>\
        RenderCollection::CollectionHandle RenderCollection::AddResource<type>(RenderCollection* collection, type* r) { return AddResourceInternal<type>(collection, r);}\
        template<>\
        int RenderCollection::ResourceCount<type>(RenderCollection* collection) { return ResourceCountInternal<type>(collection);}\
        template<>\
        type* RenderCollection::GetResource<type>(RenderCollection* collection, RenderCollection::CollectionHandle handle) { return GetResourceInternal<type>(collection, handle);}\
        template<>\
        const PropertyGrid::PropertyAccessor* RenderCollection::GetPropAccessor<type>(RenderCollection* collection, RenderCollection::CollectionHandle objectHandle, int propertyId) {\
            return GetPropAccessorInternal<type>(collection, objectHandle, propertyId); }\
        template<>\
        RenderCollection::CollectionHandle RenderCollection::ResolveResourceFromGlobalCache<type>(RenderCollection* collection, GlobalCache::CacheName name)\
            { return ResolveResourceFromGlobalCacheInternal<type>(collection, name); }
    #include "../Source/Pegasus/Application/RenderResources.inl"
    #undef RES_PROCESS
   
    template<typename T>
    struct GlobalCacheSlot
    {
        GlobalCacheSlot() {}
        GlobalCache::CacheName mName;
        Core::Ref<T> mObj;
    }; 

    class GlobalCacheImpl
    {
    public:
        explicit GlobalCacheImpl(Alloc::IAllocator* alloc)
        : mAlloc(alloc) 
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) ,instance(alloc)
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS
        {
        }
    
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) Utils::Vector< GlobalCacheSlot<type> > instance;
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS

        void Clear()
        {
        #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) instance.Clear();
        #include "../Source/Pegasus/Application/RenderResources.inl"
        #undef RES_PROCESS
        }

        Alloc::IAllocator* mAlloc;
    };

    template<typename T>
    Utils::Vector<GlobalCacheSlot<T> >* GetGlobalCacheInternalContainer(GlobalCacheImpl* impl)
    {
        return nullptr;
    }

    #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) \
        template<> Utils::Vector<GlobalCacheSlot<type> >* GetGlobalCacheInternalContainer<type>(GlobalCacheImpl* impl)\
        {\
            return &impl->instance;\
        }
    #include "../Source/Pegasus/Application/RenderResources.inl"
    #undef RES_PROCESS

    GlobalCache::GlobalCache(Alloc::IAllocator* alloc)
    : mAlloc(alloc)
    {
        mImpl = PG_NEW(alloc, -1, "GlobalCacheImpl",Alloc::PG_MEM_PERM) GlobalCacheImpl(alloc);
    }

    GlobalCache::~GlobalCache()
    {
        PG_ASSERTSTR(mListeners.GetSize() == 0, "Not all listeners of global cache removed");
        PG_DELETE(mAlloc, mImpl);
    }

    void GlobalCache::AddListener(GlobalCache::IListener* listener)
    {
        mListeners.PushEmpty() = listener;
    }

    void GlobalCache::Clear()
    {
        mImpl->Clear();
    }

    void GlobalCache::NotifyListeners()
    {
        //tag all resource listeners as dirty
        for (unsigned i = 0; i < mListeners.GetSize(); ++i)
        {
            mListeners[i]->OnGlobalCacheDirty();
        }
    }

    void GlobalCache::RemoveListener(GlobalCache::IListener* listener)
    {
        for (unsigned i = 0; i < mListeners.GetSize(); ++i)
        {
            if (mListeners[i] == listener)
            {
                mListeners.Delete(i);
                return;
            }
        }
    }

    template<typename T>
    void GlobalCacheRegisterInternal(GlobalCache* cache, GlobalCache::CacheName name, T* resource)
    {
        auto* container = GetGlobalCacheInternalContainer<T>(cache->GetImpl());
        auto& slot = container->PushEmpty();
        slot.mName = name;
        slot.mObj = resource;
    }

    template<typename T>
    T* GlobalCacheFindInternal(GlobalCache* cache, GlobalCache::CacheName name)
    {
        auto* container = GetGlobalCacheInternalContainer<T>(cache->GetImpl());
        for (unsigned i = 0; i < container->GetSize(); ++i)
        {
            auto& slot = (*container)[i];
            if (slot.mName == name)
            {
                return slot.mObj;
            }
        }
        return nullptr;
    }
    #define RES_PROCESS(type, instance, metaname, hasProperties, canUpdate) \
        template<> void GlobalCache::Register<type>(GlobalCache* cache, GlobalCache::CacheName name, type* resource)\
            {\
                GlobalCacheRegisterInternal<type>(cache, name, resource);\
                cache->NotifyListeners();\
            }\
        template<> type* GlobalCache::Find<type>(GlobalCache* cache, GlobalCache::CacheName name) { return GlobalCacheFindInternal<type>(cache, name);}
    #include "../Source/Pegasus/Application/RenderResources.inl"
    #undef RES_PROCESS
}
}
