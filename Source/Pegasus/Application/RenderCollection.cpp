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
#include "Pegasus/Window/IWindowContext.h"
#include "Pegasus/Application/RenderCollection.h"

using namespace Pegasus::Utils;

namespace Pegasus
{

namespace Application
{

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
    };

    RenderCollectionImpl::RenderCollectionImpl(Alloc::IAllocator * alloc)
        :
        mPrograms(alloc),
        mShaders(alloc),
        mTextures(alloc),
        mTextureGenerators(alloc),
        mTextureOperators(alloc),
        mMeshes(alloc),
        mMeshGenerators(alloc)
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
        
        mPrograms.Clear();
        mShaders.Clear();
        mTextures.Clear();
        mTextureGenerators.Clear();
        mTextureOperators.Clear();
        mMeshes.Clear();
        mMeshGenerators.Clear();
    }

    RenderCollection::RenderCollection(Alloc::IAllocator* alloc, Wnd::IWindowContext* context)
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

    int RenderCollection::GetMeshCount()
    {
        return mImpl->mMeshes.GetSize();
    }

    RenderCollection::CollectionHandle RenderCollection::AddMeshGenerator(Mesh::MeshGenerator* mesh)
    {
        mImpl->mMeshGenerators.PushEmpty() = mesh;
        return mImpl->mMeshGenerators.GetSize() - 1; 
    }

    int RenderCollection::GetMeshGeneratorCount()
    {
        return mImpl->mMeshGenerators.GetSize();
    }

    void RenderCollection::Clean()
    {
        mImpl->Clean();
    }
}
}

