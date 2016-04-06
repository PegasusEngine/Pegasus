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
    namespace Io {
        class FileBuffer;
    }

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

//! Asset compiler helper class
class AssetBuilder
{
public:
    friend class AssetLib;

    //! Constructor 
    //! \param allcator to use
    explicit AssetBuilder(Alloc::IAllocator* allocator);

    //! Destructor
    ~AssetBuilder();

    //! Resets the builder state. Does not deallocate memory. Use this to reparse
    void Reset();

    //! Begins construction of an array
    //! \return the array
    Array* BeginArray();

    //! Gets the current array state
    //! \return array in current stack
    Array* GetArray();

    //! Pops the array that was created / modified
    void   EndArray();

    //! Pushes a new Object, created on the block allocator
    //! \return the object that has been created
    Object* BeginObject();
    
    //! Gets the current object in the stack
    //! \return the object created
    Object* GetObject();
    
    //! ends object construction
    void    EndObject();
     
    //! Begins compilation of the main asset
    void BeginCompilation(Asset* asset);

    //! Copies a string into the block allocator string, so we dont have to worry about destruction
    //! \return the string copied
    const char* CopyString(const char* string);

    //! Increments the current line being processed in the asset script file (json)
    void IncLine() { ++mCurrentLine; }

    //! Gets the current line processed
    //! \return the line processed
    int GetCurrentLine() const { return mCurrentLine; }

    //! Gets the asset that was built througout compilation
    //! \return the structured asset
    Asset* GetBuiltAsset() { return mFinalAsset; }

    //! Increments the error count of this compilation process
    void IncErrorCount() { ++mErrorCount; }

    //! Returns the error count of this compilation process
    int GetErrorCount() const { return mErrorCount; }

    //! Enqueues the child asset of a particular object
    void EnqueueChildAsset(const char* name, const char* newAssetPath);

    //! Enqueues the child asset of a particular asset path
    void EnqueueAssetArrayElement(const char* assetPath);

private:
    Alloc::IAllocator* mAllocator;
    Utils::Vector<Array*>  mArrStack;
    Utils::Vector<Object*> mObjStack;

    struct ObjectChildAssetRequest
    {
        Object*     object; 
        const char* identifier;
        const char* assetPath;
    };

    struct ArrayChildAssetRequest
    {
        Array* array;
        const char* assetPath;
    };

    Utils::Vector<ObjectChildAssetRequest> mObjectChildAssetQueue;
    Utils::Vector<ArrayChildAssetRequest>  mArrayChildAssetQueue;

    Asset*  mFinalAsset;
    int     mCurrentLine;
    int     mErrorCount;
};

}

}

#endif
