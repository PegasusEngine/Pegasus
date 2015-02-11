/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetBuilder.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  Asset Builder (parser helper)

#ifndef PEGASUS_ASSET_BUILDER_H
#define PEGASUS_ASSET_BUILDER_H

#include "Pegasus/Memory/BlockAllocator.h"
#include "Pegasus/Utils/Vector.h"

//fwd declarations
namespace Pegasus
{
    namespace Alloc {
        class IAllocator;
    }

    namespace AssetLib {
        class Object;
        class Array;
        class Asset;
    }
}

namespace Pegasus
{

namespace AssetLib
{

class AssetBuilder
{
public:

    explicit AssetBuilder(Alloc::IAllocator* allocator);
    ~AssetBuilder();

    void Reset();

    Array* BeginArray();

    Array* GetArray();

    void   EndArray();

    Object* BeginObject();
    
    Object* GetObject();
    
    void    EndObject();

    Asset* BuildAsset(Object* obj);

    const char* CopyString(const char* string);

    void IncLine() { ++mCurrentLine; }

    int GetCurrentLine() const { return mCurrentLine; }

    Asset* GetBuiltAsset() { return mFinalAsset; }

private:
    Alloc::IAllocator* mAllocator;
    Memory::BlockAllocator mAstAllocator;
    Memory::BlockAllocator mStringAllocator;
    Utils::Vector<Array*>  mArrStack;
    Utils::Vector<Object*> mObjStack;
    Asset*  mFinalAsset;
    int     mCurrentLine;
};

}

}

#endif
