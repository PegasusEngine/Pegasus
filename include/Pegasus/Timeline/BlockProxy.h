/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockProxy.h
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline blocks

#ifndef PEGASUS_TEXTURE_BLOCKPROXY_H
#define PEGASUS_TEXTURE_BLOCKPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/IBlockProxy.h"

namespace Pegasus {
    namespace Timeline {
        class Block;
    }
}

namespace Pegasus {
namespace Timeline {


//! Proxy object, used by the editor to interact with the timeline blocks
class BlockProxy : public IBlockProxy
{
public:

    // Constructor
    //! \param block Proxied timeline block object, cannot be nullptr
    BlockProxy(Block * block);

    //! Destructor
    virtual ~BlockProxy();

    //! Get the block associated with the proxy
    //! \return Block associated with the proxy (!= nullptr)
    inline Block * GetBlock() const { return mBlock; }


    //! Get the position of the block in the lane
    //! \return Position of the block, measured in ticks
    virtual Beat GetBeat() const;

    //! Get the duration of the block
    //! \return Duration of the block, measured in beats (> 0)
    virtual Duration GetDuration() const;

    //! Get the lane the block belongs to
    //! \return Lane the block belongs to, nullptr when the block is not associated with a lane yet
    virtual ILaneProxy * GetLane() const;


    //! Get the string displayed by the editor (usually class name without the "Block" suffix)
    //! \warning To be defined in each derived class, using the DECLARE_TIMELINE_BLOCK macro
    //! \return String displayed by the editor
    virtual const char * GetEditorString() const;

    //! Set the color of the block
    //! \param red Red component (0-255)
    //! \param green Green component (0-255)
    //! \param blue Blue component (0-255)
    virtual void SetColor(unsigned char red, unsigned char green, unsigned char blue);

    //! Get the color of the block
    virtual void GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const;

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied timeline block object
    Block * const mBlock;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_BLOCKPROXY_H
