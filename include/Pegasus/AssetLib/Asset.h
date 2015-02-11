/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Asset.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  Asset Class.

#ifndef PEGASUS_ASSET_H
#define PEGASUS_ASSET_H

#define MAX_ASSET_PATH_STRING 512

//Fwd declarations
namespace Pegasus
{
    namespace Utils {
        class ByteStream;
    }
}

namespace Pegasus
{
namespace AssetLib
{

class Object;

//! Asset representation
class Asset
{
public:
    
    //! Constructor
    //! \param obj the root object used for this asset.
    explicit Asset(Object* obj);

    //! Destructor
    ~Asset();

    //! Gets the root object
    //! \return the root object
    Object* Root() { return mRoot; }

    //! Sets the path of this assets file
    //! \param the full path of this asset
    void SetPath(const char* path);

    //! Gets the path of this assets file
    //! \return the full path of this asset
    const char* GetPath() const { return mPathString; }

    //! dumps this asset (writes it) to a byte stream
    void DumpToStream(Utils::ByteStream& bs);

private:
    Object* mRoot;
    char mPathString[MAX_ASSET_PATH_STRING];
};

}
}

#endif
