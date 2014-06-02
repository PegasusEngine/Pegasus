/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLTextureFactory.cpp
//! \author Kleber Garcia
//! \date   1st June 2014
//! \brief  Texture factory implementation. Provided to the texture package

#if PEGASUS_GAPI_GL

#include "../Source/Pegasus/Render/GL/GLGPUDataDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Render/TextureFactory.h"
#include "Pegasus/Texture/TextureData.h"

//! internal definition of texture factory API
class GLTextureFactory : public Pegasus::Texture::ITextureFactory
{
public:
    GLTextureFactory() : mAllocator(nullptr){}

    virtual ~GLTextureFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator)
    {
        mAllocator = allocator;
    }

    //! Generates GPU data for a texture data node. 
    //! \param nodeData 
    virtual void GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData); 

    //! Destroys GPU data for a texture data node. 
    //! \param nodeData 
    virtual void DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData);

private:
    Pegasus::Alloc::IAllocator * mAllocator;
};


void GLTextureFactory::GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData)
{
}

void GLTextureFactory::DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData)
{
}
//! define a global static mesh factory api
static GLTextureFactory gGlobalTextureFactory;

Pegasus::Texture::ITextureFactory * Pegasus::Render::GetRenderTextureFactory()
{
    return &gGlobalTextureFactory;
}

#endif //PEGASUS_GAPI_GL
