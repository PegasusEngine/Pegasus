/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IBlockProxy.h
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline blocks

#ifndef PEGASUS_TEXTURE_SHARED_IBLOCKPROXY_H
#define PEGASUS_TEXTURE_SHARED_IBLOCKPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


//! Proxy interface, used by the editor to interact with the timeline blocks
class IBlockProxy
{
public:

    //! Destructor
    virtual ~IBlockProxy() {};


    //! Set the position of the block in the lane
    //! \param position Position of the block, measured in beats (>= 0.0f)
    virtual void SetPosition(float position) = 0;

    //! Get the position of the block in the lane
    //! \return Position of the block, measured in beats (>= 0.0f)
    virtual float GetPosition() const = 0;

    //! Set the length of the block
    //! \param length Length of the block, measured in beats (> 0.0f)
    virtual void SetLength(float length) = 0;

    //! Get the length of the block
    //! \return Length of the block, measured in beats (> 0.0f)
    virtual float GetLength() const = 0;

    //! Set the color of the block
    //! \param red Red component (0-255)
    //! \param green Green component (0-255)
    //! \param blue Blue component (0-255)
    virtual void SetColor(unsigned char red, unsigned char green, unsigned char blue) = 0;

    //! Get the color of the block
    virtual void GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const = 0;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_IBLOCKPROXY_H
