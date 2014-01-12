/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.h
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#ifndef PEGASUS_TIMELINE_BLOCK_H
#define PEGASUS_TIMELINE_BLOCK_H

#include "Pegasus/Window/IWindowContext.h"

namespace Pegasus {
    namespace Timeline {
        class BlockProxy;
    }

    namespace Wnd {
        class Window;
    }
}

namespace Pegasus {
namespace Timeline {


//! Timeline block, describing the instance of an effect on the timeline
class Block
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    Block(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~Block();


    //! Set the position of the block in the lane
    //! \param position Position of the block, measured in beats (>= 0.0f)
    void SetPosition(float position);

    //! Get the position of the block in the lane
    //! \return Position of the block, measured in beats (>= 0.0f)
    inline float GetPosition() const { return mPosition; }

    //! Set the length of the block
    //! \param length Length of the block, measured in beats (> 0.0f)
    void SetLength(float length);

    //! Get the length of the block
    //! \return Length of the block, measured in beats (> 0.0f)
    inline float GetLength() const { return mLength; }

#if PEGASUS_ENABLE_PROXIES

    //! Set the color of the block for the editor
    //! \param red Red component (0-255)
    //! \param green Green component (0-255)
    //! \param blue Blue component (0-255)
    void SetColor(unsigned char red, unsigned char green, unsigned char blue);

    //! Get the color of the block for the editor
    void GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const;


    //! Get the proxy associated with the block
    inline BlockProxy * GetProxy() const { return mProxy; }

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

public:

    // Functions called in derived classes

    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Render(float beat, Wnd::Window * window) = 0;

    //------------------------------------------------------------------------------------

protected:

    // Accessors to the global manager for the derived classes
    
    //! Get the global IO manager
    //! \return Global IO manager
    inline Io::IOManager * GetIOManager() const { return mAppContext->GetIOManager(); }

    //! Get the node manager
    //! \return Global node manager
    inline Graph::NodeManager * GetNodeManager() const { return mAppContext->GetNodeManager(); }

    //! Get the shader manager
    //! \return Global shader manager
    inline Shader::ShaderManager * GetShaderManager() const { return mAppContext->GetShaderManager(); }

    //! Get the texture manager
    //! \return Global texture manager
    inline Texture::TextureManager * GetTextureManager() const { return mAppContext->GetTextureManager(); }

    //! Get the timeline
    //! \return Global timeline
    inline Timeline * GetTimeline() const { return mAppContext->GetTimeline(); }

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(Block)


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Wnd::IWindowContext * mAppContext;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the block
    BlockProxy * mProxy;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Position of the block in the lane, measured in beats (>= 0.0f)
    float mPosition;

    //! Length of the block, measured in beats (> 0.0f)
    float mLength;

#if PEGASUS_ENABLE_PROXIES

    //! Red component of the color for the editor (0-255)
    unsigned char mColorRed;

    //! Green component of the color for the editor (0-255)
    unsigned char mColorGreen;

    //! Blue component of the color for the editor (0-255)
    unsigned char mColorBlue;

#endif  // PEGASUS_ENABLE_PROXIES
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_BLOCK_H
