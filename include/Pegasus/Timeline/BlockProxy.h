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


    //! Set the position of the block in the lane
    //! \param position Position of the block, measured in beats (>= 0.0f)
    virtual void SetPosition(float position);

    //! Get the position of the block in the lane
    //! \return Position of the block, measured in beats (>= 0.0f)
    virtual float GetPosition() const;

    //! Set the length of the block
    //! \param length Length of the block, measured in beats (> 0.0f)
    virtual void SetLength(float length);

    //! Get the length of the block
    //! \return Length of the block, measured in beats (> 0.0f)
    virtual float GetLength() const;

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied timeline block object
    Block * const mBlock;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_BLOCKPROXY_H
