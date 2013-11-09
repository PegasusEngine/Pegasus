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

namespace Pegasus {
    namespace Timeline {
        class BlockProxy;
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
    Block(Alloc::IAllocator * allocator);

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

    //! Get the proxy associated with the block
    inline BlockProxy * GetProxy() const { return mProxy; }

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(Block)


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the block
    BlockProxy * mProxy;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Position of the block in the lane, measured in beats (>= 0.0f)
    float mPosition;

    //! Length of the block, measured in beats (> 0.0f)
    float mLength;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_BLOCK_H
