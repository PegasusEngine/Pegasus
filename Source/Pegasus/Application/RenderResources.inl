//! Inline file to process render resources
//RES_PROCESS(type, membername, metaname, (bool?)hasProperties, (bool) canUpdate)
RES_PROCESS(Pegasus::Shader::ProgramLinkage, mPrograms, "ProgramLinkage", false, true)
RES_PROCESS(Pegasus::Shader::ShaderStage, mShaders, "ShaderStage", false, true)
RES_PROCESS(Pegasus::Texture::Texture, mTextures, "Texture", false, true)
RES_PROCESS(Pegasus::Mesh::Mesh, mMeshes, "Mesh", false, true)
RES_PROCESS(Pegasus::Mesh::MeshGenerator, mMeshGenerators, "MeshGenerator", true, false)
RES_PROCESS(Pegasus::Mesh::MeshOperator, mMeshOperators, "MeshOperator", true, false)
RES_PROCESS(Pegasus::Texture::TextureGenerator, mTextureGenerators, "TextureGenerator", true, false)
RES_PROCESS(Pegasus::Texture::TextureOperator, mTextureOperators, "TextureOperator", true, false)
#if PEGASUS_ENABLE_RENDER_API
RES_PROCESS(Pegasus::Render::Buffer, mBuffers, "Buffer", false, false)
RES_PROCESS(Pegasus::Render::RasterizerState, mRasterizers, "RasterizerState", false, false)
RES_PROCESS(Pegasus::Render::BlendingState, mBlendingStates, "BlendingState", false, false)
RES_PROCESS(Pegasus::Render::SamplerState, mSamplerStates, "SamplerState", false, false)
RES_PROCESS(Pegasus::Render::RenderTarget, mRenderTargets, "RenderTarget", false, false)
RES_PROCESS(Pegasus::Render::VolumeTexture, mVolumeTextures, "VolumeTexture", false, false)
RES_PROCESS(Pegasus::Render::CubeMap, mCubeMaps, "CubeMap", false, false)
RES_PROCESS(Pegasus::Render::DepthStencil, mDepthStencil, "DepthStencil", false, false)
#endif
RES_PROCESS(Pegasus::Application::GenericResource, mGenericResources, "GenericResource", false, true) //properties are handled different for a generic resource (see implementation of generic resource).
