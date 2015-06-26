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

using namespace Pegasus::Utils;

namespace Pegasus
{

namespace Application
{
    
    RenderCollectionFactory::RenderCollectionFactory(Core::IApplicationContext* context, Alloc::IAllocator* alloc)
        :mAlloc(alloc), mPropLayoutEntries(alloc), mContext(context)
    {
    }

    RenderCollectionFactory::~RenderCollectionFactory()
    {
    }

    void RenderCollectionFactory::RegisterPropertyCount(const char* name, int numOfProperties)
    {
        RenderCollectionFactory::PropEntries& entry = mPropLayoutEntries.PushEmpty();
        entry.mName = name;
        entry.mPropertyCount = numOfProperties;
    }

    class RenderCollectionImpl
    {
    public:
        explicit RenderCollectionImpl(Alloc::IAllocator * alloc);
        ~RenderCollectionImpl() { Clean(); }
        void Clean();
    
        Vector<Shader::ProgramLinkageRef>    mPrograms;
        Vector<Shader::ShaderStageRef>       mShaders;
        Vector<Texture::TextureRef>          mTextures;
        Vector<Texture::TextureGeneratorRef> mTextureGenerators;
        Vector<Texture::TextureOperatorRef>  mTextureOperators;
        Vector<Mesh::MeshRef>                mMeshes;
        Vector<Mesh::MeshGeneratorRef>       mMeshGenerators;
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
        for (int i = 0; i < mPrograms.GetSize(); ++i)         { mPrograms[i] = nullptr; }
        for (int i = 0; i < mShaders.GetSize(); ++i)          { mShaders[i] = nullptr; }
        for (int i = 0; i < mTextures.GetSize(); ++i)         { mTextures[i] = nullptr; }
        for (int i = 0; i < mTextureGenerators.GetSize(); ++i){ mTextureGenerators[i] = nullptr; }
        for (int i = 0; i < mTextureOperators.GetSize(); ++i) { mTextureOperators[i] = nullptr; }
        for (int i = 0; i < mMeshes.GetSize(); ++i)           { mMeshes[i] = nullptr; }
        for (int i = 0; i < mMeshGenerators.GetSize(); ++i)   { mMeshGenerators[i] = nullptr; }
        
        //cleanup of buffers using render API
        for (int i = 0; i < mBuffers.GetSize(); ++i) 
        {
            Render::DeleteBuffer(mBuffers[i]);
        }
        
        for (int i = 0; i < mRasterizerStates.GetSize(); ++i)
        {
            Render::DeleteRasterizerState(mRasterizerStates[i]);     
        }

        for (int i = 0; i < mBlendingStates.GetSize(); ++i)
        {
            Render::DeleteBlendingState(mBlendingStates[i]);
        }

        for (int i = 0; i < mRenderTargets.GetSize(); ++i)
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

    RenderCollection::RenderCollection(Alloc::IAllocator* alloc, Core::IApplicationContext* context)
    : mAlloc(alloc),
      mContext(context)
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
        return &(*mImpl->mTextures[id]);
    }

    int RenderCollection::GetTextureCount() const
    {
        return mImpl->mTextures.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddTextureGenerator(Texture::TextureGenerator* texGen)
    {
        mImpl->mTextureGenerators.PushEmpty() = texGen;
        return mImpl->mTextureGenerators.GetSize() - 1;
    }

    Texture::TextureGenerator* RenderCollection::GetTextureGenerator(RenderCollection::CollectionHandle id)
    {
        return &(*mImpl->mTextureGenerators[id]);
    }

    int RenderCollection::GetTextureGeneratorCount() const
    {
        return mImpl->mTextureGenerators.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddTextureOperator(Texture::TextureOperator* texOp)
    {
        mImpl->mTextureOperators.PushEmpty() = texOp;
        return mImpl->mTextureOperators.GetSize() - 1 ;
    }

    Texture::TextureOperator* RenderCollection::GetTextureOperator(RenderCollection::CollectionHandle id)
    {
        return &(*mImpl->mTextureOperators[id]);
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
        return &(*mImpl->mMeshes[id]);
    }

    int RenderCollection::GetMeshCount() const
    {
        return mImpl->mMeshes.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddMeshGenerator(Mesh::MeshGenerator* mesh)
    {
        mImpl->mMeshGenerators.PushEmpty() = mesh;
        return mImpl->mMeshGenerators.GetSize() - 1; 
    }

    Mesh::MeshGenerator* RenderCollection::GetMeshGenerator(RenderCollection::CollectionHandle id)
    {
        return &(*mImpl->mMeshGenerators[id]);
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
        return &(mImpl->mBlendingStates[id]);
    }

    int RenderCollection::GetBlendingStateCount() const
    {
        return mImpl->mBlendingStates.GetSize();
    }
}
}

