/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureTracker.cpp
//! \author	Kevin Boulanger
//! \date	09th June 2014
//! \brief	Container class tracking all currently allocated textures

#include "Pegasus/Texture/TextureTracker.h"
#include "Pegasus/Utils/Memset.h"

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


//! Container class tracking all currently allocated textures
//! \todo Implement a faster container, the insertion and deletion operations are O(n)
TextureTracker::TextureTracker()
:   mNumTextures(0)
{
    // Reset the array, so we can detect errors later
    Utils::Memset8(mTextures, 0, MAX_NUM_TEXTURES * sizeof(Texture *));
}

//----------------------------------------------------------------------------------------

TextureTracker::~TextureTracker()
{
    // Test to check that all textures have been deallocated, used to find leaks
    PG_ASSERTSTR(mNumTextures == 0, "Memory leak detected, %d textures are still allocated when the tracker is deleted", mNumTextures);
    for (unsigned int t = 0; t < MAX_NUM_TEXTURES; ++t)
    {
        if (mTextures[t] != nullptr)
        {
            PG_FAILSTR("Invalid data in the texture tracker: texture at index %d should be nullptr", t);
        }
    }
}

//----------------------------------------------------------------------------------------

void TextureTracker::InsertTexture(Texture * texture)
{
    if (mNumTextures < MAX_NUM_TEXTURES)
    {
        mTextures[mNumTextures++] = texture;
    }
    else
    {
        PG_FAILSTR("Maximum number of tracked textures reached (%d), increase the size of the container", MAX_NUM_TEXTURES);
    }
}

//----------------------------------------------------------------------------------------

void TextureTracker::DeleteTexture(Texture * texture)
{
    if (texture != nullptr)
    {
        if (mNumTextures > 0)
        {
            // Look for the texture
            unsigned int index = 0;
            for (; index < mNumTextures; ++index)
            {
                if (mTextures[index] == texture)
                {
                    // Texture found, move the list on the right of the deleted texture
                    // one step to the left, with an extra step to delete the last texture
                    // old slot.
                    --mNumTextures;
                    for (unsigned int i = index; i < mNumTextures; ++i)
                    {
                        mTextures[i] = mTextures[i + 1];
                    }
                    mTextures[mNumTextures] = nullptr;
                    break;
                }
            }

            PG_ASSERTSTR(index <= mNumTextures, "Trying to delete a texture from the tracker but it has not been found in the list");
        }
        else
        {
            PG_FAILSTR("Trying to delete a texture from the tracker but the latter is empty");
        }
    }
    else
    {
        PG_FAILSTR("Trying to delete a nullptr texture from the tracker");
    }
}

//----------------------------------------------------------------------------------------

Texture * TextureTracker::GetTexture(unsigned int index) const
{
    if (index < mNumTextures)
    {
        PG_ASSERTSTR(mTextures[index] != nullptr, "Invalid content for the texture tracker, it contains a nullptr texture at index %d", index);
        return mTextures[index];
    }
    else
    {
        PG_FAILSTR("Invalid index (%d) when trying to access the texture tracker, it must be < %d", index, mNumTextures);
        return nullptr;
    }
}


}   // namespace Texture
}   // namespace Pegasus

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif  // PEGASUS_ENABLE_PROXIES
