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

    template<class T>
    struct ObjectPropertyCache
    {
        Core::Ref<T> mObject;
        PropertyInfo* mCachedInfos;
        Alloc::IAllocator* mAllocator;
        int mCachedInfoCount;

        ObjectPropertyCache()
        : mCachedInfos(nullptr), mCachedInfoCount(0), mAllocator(nullptr) {}

        void Initialize(const char* nodeName, const RenderCollectionFactory* factory,  T* node, Alloc::IAllocator* allocator)
        {
            mAllocator = allocator;
            mObject = node;

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
            mObject = nullptr;
            if (mCachedInfoCount > 0)
            {
                PG_DELETE_ARRAY(mAllocator, mCachedInfos);
                mCachedInfos = nullptr;
                mCachedInfoCount = 0;
            }
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
    
        Vector<Shader::ProgramLinkageRef>    mPrograms;
        Vector<Shader::ShaderStageRef>       mShaders;
        Vector< Texture::TextureRef >        mTextures;
        Vector< Mesh::MeshRef >                 mMeshes;
        Vector< ObjectPropertyCache<Mesh::MeshGenerator> >       mMeshGenerators;
        Vector< ObjectPropertyCache<Texture::TextureGenerator> >  mTextureGenerators;
        Vector< ObjectPropertyCache<Texture::TextureOperator> >   mTextureOperators;
        Vector<Render::Buffer>               mBuffers;
        Vector<Render::RasterizerState>      mRasterizerStates;
        Vector<Render::BlendingState>        mBlendingStates;
        Vector<Render::RenderTarget>         mRenderTargets;
    };

    RenderCollectionImpl::RenderCollectionImpl(Alloc::IAllocator * alloc)
        :
        mPrograms(alloc),
        mShaders(alloc),
        mTextures(alloc),
        mTextureGenerators(alloc),
        mTextureOperators(alloc),
        mMeshes(alloc),
        mMeshGenerators(alloc),
        mBuffers(alloc),
        mRasterizerStates(alloc),
        mBlendingStates(alloc),
        mRenderTargets(alloc)
    {
    }

    void RenderCollectionImpl::Clean()
    {
        //cleanup of buffers using render API
        for (unsigned int i = 0; i < mBuffers.GetSize(); ++i) 
        {
            Render::DeleteBuffer(mBuffers[i]);
        }
        
        for (unsigned int i = 0; i < mRasterizerStates.GetSize(); ++i)
        {
            Render::DeleteRasterizerState(mRasterizerStates[i]);     
        }

        for (unsigned int i = 0; i < mBlendingStates.GetSize(); ++i)
        {
            Render::DeleteBlendingState(mBlendingStates[i]);
        }

        for (unsigned int i = 0; i < mRenderTargets.GetSize(); ++i)
        {
            Render::DeleteRenderTarget(mRenderTargets[i]);
        }
        
        mPrograms.Clear();
        mShaders.Clear();
        mTextures.Clear();
        mTextureGenerators.Clear();
        mTextureOperators.Clear();
        mMeshes.Clear();
        mMeshGenerators.Clear();
        mBuffers.Clear();
        mRasterizerStates.Clear();
        mBlendingStates.Clear();
        mRenderTargets.Clear();

        //remove any references if they exist of shaders / programs and meshes internally
        Render::CleanInternalState();
    }

    RenderCollection::RenderCollection(Alloc::IAllocator* alloc, RenderCollectionFactory* factory, Core::IApplicationContext* context)
    : mAlloc(alloc),
      mContext(context),
      mFactory(factory),
      mCurrentWindow(nullptr)
    {
        mImpl = PG_NEW(alloc, -1, "RenderCollectionImpl", Alloc::PG_MEM_TEMP) RenderCollectionImpl(alloc);
    }

    RenderCollection::~RenderCollection()
    {
        PG_DELETE(mAlloc, mImpl);
    }
    
    RenderCollection::CollectionHandle RenderCollection::AddProgram(Shader::ProgramLinkage* program)
    {
        mImpl->mPrograms.PushEmpty() = program;
        return mImpl->mPrograms.GetSize() - 1;
    }

    Shader::ProgramLinkage* RenderCollection::GetProgram(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(*mImpl->mPrograms[id]);
    }

    int RenderCollection::GetProgramCount() const
    {
        return mImpl->mPrograms.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddShader(Shader::ShaderStage* shader)
    {
        mImpl->mShaders.PushEmpty() = shader;
        return mImpl->mShaders.GetSize() - 1;
    }

    Shader::ShaderStage* RenderCollection::GetShader(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(*mImpl->mShaders[id]);
    }

    int RenderCollection::GetShaderCount() const
    {
        return mImpl->mShaders.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddTexture(Texture::Texture* texture)
    {
        mImpl->mTextures.PushEmpty() = texture;
        return mImpl->mTextures.GetSize() - 1;
    }

    Texture::Texture* RenderCollection::GetTexture(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(*mImpl->mTextures[id]);
    }

    int RenderCollection::GetTextureCount() const
    {
        return mImpl->mTextures.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddTextureGenerator(Texture::TextureGenerator* texGen)
    {
        mImpl->mTextureGenerators.PushEmpty().Initialize("TextureGenerator", mFactory, texGen, mAlloc);
        return mImpl->mTextureGenerators.GetSize() - 1;
    }

    Texture::TextureGenerator* RenderCollection::GetTextureGenerator(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(*mImpl->mTextureGenerators[id].mObject);
    }

    const PropertyGrid::PropertyAccessor* RenderCollection::GetTextureGeneratorAccessor(RenderCollection::CollectionHandle objectHandle, int propertyId)
    {
        if (objectHandle == INVALID_HANDLE) 
        {
            PG_LOG('ERR_', "Property requested from invalid object.");
            return nullptr;
        }
        PG_ASSERT(propertyId < mImpl->mTextureGenerators[objectHandle].mCachedInfoCount);

        PropertyInfo& info = mImpl->mTextureGenerators[objectHandle].mCachedInfos[propertyId];
        
        if (info.mValid)
        {
            return &info.mCachedAccessor;
        } 

        return nullptr;
    }

    int RenderCollection::GetTextureGeneratorCount() const
    {
        return mImpl->mTextureGenerators.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddTextureOperator(Texture::TextureOperator* texOp)
    {
        mImpl->mTextureOperators.PushEmpty().Initialize("TextureOperator", mFactory, texOp, mAlloc);
        return mImpl->mTextureOperators.GetSize() - 1 ;
    }

    Texture::TextureOperator* RenderCollection::GetTextureOperator(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(*mImpl->mTextureOperators[id].mObject);
    }

    const PropertyGrid::PropertyAccessor* RenderCollection::GetTextureOperatorAccessor(RenderCollection::CollectionHandle objectHandle, int propertyId)
    {
        if (objectHandle == INVALID_HANDLE) 
        {
            PG_LOG('ERR_', "Property requested from invalid object.");
            return nullptr;
        }

        PG_ASSERT(propertyId < mImpl->mTextureOperators[objectHandle].mCachedInfoCount);

        PropertyInfo& info = mImpl->mTextureOperators[objectHandle].mCachedInfos[propertyId];
        
        if (info.mValid)
        {
            return &info.mCachedAccessor;
        } 

        return nullptr;
    }

    int RenderCollection::GetTextureOperatorCount() const
    {
        return mImpl->mTextureOperators.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddMesh(Mesh::Mesh* mesh)
    {
        mImpl->mMeshes.PushEmpty() = mesh;
        return mImpl->mMeshes.GetSize() - 1;
    }

    Mesh::Mesh* RenderCollection::GetMesh(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(*mImpl->mMeshes[id]);
    }

    int RenderCollection::GetMeshCount() const
    {
        return mImpl->mMeshes.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddMeshGenerator(Mesh::MeshGenerator* mesh)
    {
        mImpl->mMeshGenerators.PushEmpty().Initialize("MeshGenerator", mFactory, mesh, mAlloc);
        return mImpl->mMeshGenerators.GetSize() - 1; 
    }

    const PropertyGrid::PropertyAccessor* RenderCollection::GetMeshGeneratorAccessor(RenderCollection::CollectionHandle objectHandle, int propertyId)
    {
        if (objectHandle == INVALID_HANDLE) 
        {
            PG_LOG('ERR_', "Property requested from invalid object.");
            return nullptr;
        }

        PG_ASSERT(propertyId < mImpl->mMeshGenerators[objectHandle].mCachedInfoCount);

        PropertyInfo& info = mImpl->mMeshGenerators[objectHandle].mCachedInfos[propertyId];
        
        if (info.mValid)
        {
            return &info.mCachedAccessor;
        } 

        return nullptr;
    }

    Mesh::MeshGenerator* RenderCollection::GetMeshGenerator(RenderCollection::CollectionHandle id)
    {
        return &(*mImpl->mMeshGenerators[id].mObject);
    }

    int RenderCollection::GetMeshGeneratorCount()
    {
        return mImpl->mMeshGenerators.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddBuffer(const Render::Buffer& buffer)
    {
        mImpl->mBuffers.PushEmpty() = buffer;
        return mImpl->mBuffers.GetSize() - 1;
        
    }

    Render::Buffer* RenderCollection::GetBuffer(RenderCollection::CollectionHandle id)
    {
        if (INVALID_HANDLE == id) return nullptr;
        return &(mImpl->mBuffers[id]);
    } 

    int RenderCollection::GetBufferCount()
    {
        return mImpl->mBuffers.GetSize();
    }

    void RenderCollection::Clean()
    {
        mImpl->Clean();
    }

    RenderCollection::CollectionHandle RenderCollection::AddRenderTarget(const Render::RenderTarget& target)
    {
        mImpl->mRenderTargets.PushEmpty() = target;        
        return mImpl->mRenderTargets.GetSize() - 1;
    }

    Render::RenderTarget* RenderCollection::GetRenderTarget(RenderCollection::CollectionHandle id)
    {
        if (INVALID_HANDLE == id) return nullptr;
        return &(mImpl->mRenderTargets[id]);
    }

    int RenderCollection::GetRenderTargetCount() const
    {
        return mImpl->mRenderTargets.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddRasterizerState(const Render::RasterizerState& state)
    {
        mImpl->mRasterizerStates.PushEmpty() = state;
        return mImpl->mRasterizerStates.GetSize() - 1;
    }

    Render::RasterizerState* RenderCollection::GetRasterizerState(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(mImpl->mRasterizerStates[id]);
    }

    int RenderCollection::GetRasterizerStateCount() const
    {
        return mImpl->mRasterizerStates.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddBlendingState(const Render::BlendingState& state)
    {
        mImpl->mBlendingStates.PushEmpty() = state;        
        return mImpl->mBlendingStates.GetSize() - 1;
    }

    Render::BlendingState* RenderCollection::GetBlendingState(RenderCollection::CollectionHandle id)
    {
        if (id == INVALID_HANDLE) return nullptr;
        return &(mImpl->mBlendingStates[id]);
    }

    int RenderCollection::GetBlendingStateCount() const
    {
        return mImpl->mBlendingStates.GetSize();
    }
}
}

