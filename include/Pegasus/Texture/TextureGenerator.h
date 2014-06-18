/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGenerator.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base texture generator node class

#ifndef PEGASUS_TEXTURE_TEXTUREGENERATOR_H
#define PEGASUS_TEXTURE_TEXTUREGENERATOR_H

#include "Pegasus/Graph/GeneratorNode.h"
#include "Pegasus/Texture/Shared/TextureEvent.h"
#include "Pegasus/Texture/TextureConfiguration.h"
#include "Pegasus/Texture/TextureData.h"
#include "Pegasus/Texture/TextureDeclaration.h"
#include "Pegasus/Texture/Proxy/TextureNodeProxy.h"

namespace Pegasus {
namespace Texture {


//! Base texture generator node class
//! \warning IMPORTANT! When deriving from this class, update TextureManager::RegisterAllTextureNodes()
//!                     so the generator node can be instantiated
class TextureGenerator : public Graph::GeneratorNode
{

    //Generating code to allow texture handle macros
    GRAPH_EVENT_DECLARE_DISPATCHER(ITextureEventListener);

public:

    //! Default constructor, uses the default texture configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    TextureGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Constructor
    //! \param configuration Configuration of the generator, such as the resolution and pixel format
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    TextureGenerator(const TextureConfiguration & configuration,
                     Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Set the configuration of the texture generator
    //! \warning Can be done only after the constructor has been called
    void SetConfiguration(const TextureConfiguration & configuration);

    //! Get the configuration of the generator
    //! \return Configuration of the generator, such as the resolution and pixel format
    inline const TextureConfiguration & GetConfiguration() const { return mConfiguration; }


    //! Return the texture generator up-to-date data.
    //! \note Defines the standard behavior of all generator nodes.
    //!       Calls GenerateData() if the node data is dirty.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, cannot be a null reference.
    //! \note When asking for the data of a generator node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //virtual NodeDataReturn GetUpdatedData(bool & updated);


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the texture generator
    //! \return Proxy associated with the texture generator
    //@{
    inline TextureNodeProxy * GetProxy() { return &mProxy; }
    inline const TextureNodeProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~TextureGenerator();

    //! Allocate the data associated with the texture generator
    //! \warning Do not override in derived classes since all generators
    //!          use the same texture data class
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual Graph::NodeData * AllocateData() const;


    //! Generate the content of the data associated with the texture generator
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(TextureGenerator)

    //! Configuration of the generator, such as the resolution and pixel format
    TextureConfiguration mConfiguration;


#if PEGASUS_ENABLE_PROXIES
    //! Proxy associated with the texture generator
    TextureNodeProxy mProxy;
#endif  // PEGASUS_ENABLE_PROXIES
};

//----------------------------------------------------------------------------------------

//! Reference to a TextureGenerator, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<TextureGenerator>   TextureGeneratorRef;

//! Const reference to a reference to a TextureGenerator, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<TextureGenerator> & TextureGeneratorIn;

//! Reference to a reference to a TextureGenerator, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<TextureGenerator> & TextureGeneratorInOut;

//! Reference to a TextureGenerator, typically used as the return value of a function
typedef       Pegasus::Core::Ref<TextureGenerator>   TextureGeneratorReturn;


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREGENERATOR_H
