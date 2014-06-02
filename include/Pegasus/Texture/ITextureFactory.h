/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ITextureFactory.h
//! \author Kleber Garcia
//! \date   1st June 2014
//! \brief  Texture Factory interface which creates texture GPU data

#ifndef PEGASUS_ITEXTUREFACTORY_H
#define PEGASUS_ITEXTUREFACTORY_H

namespace Pegasus
{

namespace Alloc {
    class IAllocator;
}

namespace Graph {
    class NodeData;
}

namespace Texture
{

    class TextureData;

    class ITextureFactory
    {
    public:
        ITextureFactory(){}
        virtual ~ITextureFactory(){}

        //! callback to be called at application constructor. Sets the right allocators for the texture GPU data.
        //! \param allocator the allocator to set
        virtual void Initialize(Pegasus::Alloc::IAllocator * allocator) = 0;

        //! Generates GPU data for a texture data node. 
        //! \param nodeData 
        virtual void GenerateTextureGPUData(TextureData * nodeData) = 0;

        //! Destroys GPU data for a texture data node. 
        //! \param nodeData 
        virtual void DestroyNodeGPUData(TextureData * nodeData) = 0;
    };


} //namespace Texture
} //namespace Pegasus

#endif //PEGASUS_ITEXTUREFACTORY_H
