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

namespace Pegasus
{


//! All forward declarations used in the render collection
namespace Wnd {
    class IWindowContext;
}
namespace Alloc {
    class IAllocator;
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

namespace Render {
    struct Buffer;
    struct RasterizerState;
    struct BlendingState;
    struct RenderTarget;
}

namespace Application {    
    class RenderCollectionImpl;
}

namespace Application
{

    //! container of all the node types. Used by timeline blocks.
    class RenderCollection {
    public:
        typedef int CollectionHandle;
        static const CollectionHandle INVALID_HANDLE = -1;

        //! Constructor
        //!\param alloc the allocator to use internally
        //!\param context pointer to the application context
        RenderCollection(Alloc::IAllocator* alloc, Wnd::IWindowContext* context);

        //! Destructor
        ~RenderCollection();
        
        //! Adds a reference to a program
        //!\param the program pointer
        //!\return gets the handle of this program
        CollectionHandle AddProgram(Shader::ProgramLinkage* program);

        //! Gets a pointer to a program
        //!\param the handle to such program
        //!\return the program pointer. This will assert if requesting an invalid id
        Shader::ProgramLinkage* GetProgram(CollectionHandle id);

        //!\return the count to the number of programs traced
        int GetProgramCount() const;

        //! Adds a reference to a shader 
        //!\param the shader pointer
        //!\return gets the handle of this shader 
        CollectionHandle AddShader(Shader::ShaderStage* shader);

        //! Gets a pointer to a shader 
        //!\param the handle to such shader
        //!\return the shader pointer. This will assert if requesting an invalid id
        Shader::ShaderStage* GetShader(CollectionHandle id);

        //!\return the count to the number of programs traced
        int GetShaderCount() const;

        //! Adds a reference to a texture
        //!\param the texture pointer
        //!\return gets the handle of this texture
        CollectionHandle AddTexture(Texture::Texture* texture);

        //! Gets a pointer to a  texture
        //!\param the handle to such texture 
        //!\return the texture pointer. This will assert if requesting an invalid id
        Texture::Texture* GetTexture(CollectionHandle id);

        //!\return the count to the number of textures traced
        int GetTextureCount() const;

        //! Adds a reference to a texture generator
        //!\param the texture generator pointer
        //!\return gets the handle of this texture generator
        CollectionHandle AddTextureGenerator(Texture::TextureGenerator* texGen);

        //! Gets a pointer to a texGenerator
        //!\param the handle to such  textureGenerator
        //!\return the texture generator pointer. This will assert if requesting an invalid id
        Texture::TextureGenerator* GetTextureGenerator(CollectionHandle id);

        //!\return  tex generator count
        int GetTextureGeneratorCount() const;

        //! Adds a reference to a texture operator
        //!\param the texture operator pointer
        //!\return gets the handle of this texture operator 
        CollectionHandle AddTextureOperator(Texture::TextureOperator* texOp);

        //! Gets a pointer to a texOperator
        //!\param the handle to such  textureOperator
        //!\return the texture Operator pointer. This will assert if requesting an invalid id
        Texture::TextureOperator* GetTextureOperator(CollectionHandle id);

        //! \return the count to the texture operator
        int GetTextureOperatorCount() const;

        //! Adds a reference to a mesh
        //!\param the  mesh  pointer
        //!\return gets the handle of this mesh
        CollectionHandle AddMesh(Mesh::Mesh* mesh);

        //! Gets a pointer to a mesh
        //!\param the handle to such mesh
        //!\return the texture mesh pointer. This will assert if requesting an invalid id
        Mesh::Mesh* GetMesh(CollectionHandle id);

        //! \return the count to the number of meshes
        int GetMeshCount() const;

        //! Adds a reference to a mesh generator
        //!\param the  mesh pointer
        //!\return gets the handle of this mesh generator
        CollectionHandle AddMeshGenerator(Mesh::MeshGenerator* mesh);

        //! \param id handle to mesh generator
        //! \return the mesh generator reference
        Mesh::MeshGenerator* GetMeshGenerator(CollectionHandle id);

        //! the total count of mesh generators    
        int GetMeshGeneratorCount();

        //! adds a buffer reference. Buffers are structs in render so the struct passed gets copied internally
        //! \param reference to the buffer created
        CollectionHandle AddBuffer(const Render::Buffer& buffer);

        //! \returns the buffer pointer stored internally, null if invalid
        Render::Buffer* GetBuffer(CollectionHandle id);

        //! the total count of buffers
        int GetBufferCount();

        //! adds a reference
        // \param the target reference
        CollectionHandle AddRenderTarget(const Render::RenderTarget& target);

        //! gets a reference from an id
        //! \param id
        //! \return the reference
        Render::RenderTarget* GetRenderTarget(CollectionHandle id);

        //! \return the count of this list
        int GetRenderTargetCount() const;

        //! adds a reference
        // \param the target reference
        CollectionHandle AddRasterizerState(const Render::RasterizerState& state);

        //! gets a reference from an id
        //! \param id
        //! \return the reference
        Render::RasterizerState* GetRasterizerState(CollectionHandle id);

        //! \return the count of this list
        int GetRasterizerStateCount() const;

        //! adds a reference
        // \param the target reference
        CollectionHandle AddBlendingState(const Render::BlendingState& state);

        //! gets a reference from an id
        //! \param id
        //! \return the reference
        Render::BlendingState* GetBlendingState(CollectionHandle id);

        //! \return the count of this list
        int GetBlendingStateCount() const;

        //! \return the application context pointer
        Wnd::IWindowContext* GetAppContext() { return mContext; }

        //! Cleans / Deletes all references to the shaders/nodes/meshes/rendertargets/blendingstates/rasterstates being used
        void Clean();

    private:
        //! internal memory allocator
        Alloc::IAllocator* mAlloc;

        //! internal context
        Wnd::IWindowContext* mContext;

        //! internal implementation
        RenderCollectionImpl* mImpl;
    };
}

}

#endif
