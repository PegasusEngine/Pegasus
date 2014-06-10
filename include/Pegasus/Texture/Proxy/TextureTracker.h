/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureTracker.h
//! \author	Kevin Boulanger
//! \date	09th June 2014
//! \brief	Container class tracking all currently allocated textures

#ifndef PEGASUS_TEXTURE_PROXY_TEXTURETRACKER_H
#define PEGASUS_TEXTURE_PROXY_TEXTURETRACKER_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {

class Texture;


//! Container class tracking all currently allocated textures
//! \todo Implement a faster container, the insertion and deletion operations are O(n)
class TextureTracker
{
public:

    // Constructor
    TextureTracker();

    //! Destructor
    ~TextureTracker();

    //! Maximum number of textures that can be tracked
    static const unsigned int MAX_NUM_TEXTURES = 2048;


    //! Add a texture to the tracker
    //! \param texture Pointer to the texture to track (!= nullptr)
    void InsertTexture(Texture * texture);

    //! Remove a texture from the tracker
    //! \param texture Pointer to the tracked texture to remove
    void DeleteTexture(Texture * texture);


    //! Get the number of textures currently tracked
    //! \return Number of textures currently tracked (< MAX_NUM_TEXTURES)
    inline unsigned int GetNumTextures() const { return mNumTextures; }

    //! Get one of the tracked textures
    //! \param index Index of the tracked texture to get (< GetNumTextures())
    //! \return Desired tracked texture, nullptr in case of error
    Texture * GetTexture(unsigned int index) const;

    //------------------------------------------------------------------------------------
    
private:

    //! Number of textures currently tracked (< MAX_NUM_TEXTURES)
    unsigned int mNumTextures;

    //! List of tracked textures (!= nullptr if defined,
    //! only the first mNumTextures ones are defined)
    Texture * mTextures[MAX_NUM_TEXTURES];
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_PROXY_TEXTURETRACKER_H
