#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Utils/TesselationTable.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/Memset.h"

namespace Pegasus 
{

namespace Utils
{

    static int gDefaultBucketListIncrement = 10; //up to 10 buckets in the list

    TesselationTable::TesselationTable(Alloc::IAllocator * allocator, int elementSize, int maxBucketSize)
    : 
        mAllocator(allocator),
        mRoot(nullptr),
        mElementSize(elementSize), 
        mMaxBucketSize(maxBucketSize),
        mBucketListCapacity(0)
    {
    }
    
    TesselationTable::~TesselationTable()
    {
        Clear();
    }

    void TesselationTable::Clear()
    {
        for (int i = 0; i < mBucketListCapacity; ++i)
        {
            void * bucket = mRoot[i];
            if (bucket != nullptr)
            {
                Free(bucket);
            }
        }
        if (mRoot != nullptr)
        {
            Free(static_cast<void*>(mRoot));
            mRoot = nullptr;
            mBucketListCapacity = 0;
        }
    }

    void TesselationTable::ConstructIndices(int i, int j, int& bucketIndex, int& bucketOffset) const
    {
        // since this is a compact table of connections, no need of redundant data. Meaning we dont want to 
        // store (i,j) and (j,i). we want to make these two set of indices contain the same data. Therefore
        // we store the i and j index in the same patter, i always > j
        int ni = i >= j ? i : j;
        int nj = i >= j ? j : i;

        // the formula (i * i + i) / 2 + j can be derived as follows:
        // we want to store data in the following manner:
        //
        // 0 1 2 3 4
        // 0 x
        // 1 x x
        // 2 x x x
        // 3 x x x x
        // 4 x x x x x
        //
        // We only store those slots inside x. To calculate the offset in the linear buffer where we store these
        // elements, we apply the sumation progresion n * (n + 1) / 2 which is equal to 1 + 2 +3 + 4 +...+n

        int flatIndex = (ni * ni + ni) / 2 + nj;
        bucketIndex = flatIndex / mMaxBucketSize;
        bucketOffset = flatIndex % mMaxBucketSize;
    }

    void TesselationTable::Insert(int i, int j, const char * element)
    {
        int bucketIndex, bucketOffset;
        ConstructIndices(i, j, bucketIndex, bucketOffset);
        
        // grow the bucket list if not enough buckets
        if (bucketIndex >= mBucketListCapacity)
        {
            int currBucketListByteSize = sizeof(void**) * mBucketListCapacity;
            int newBucketListCapacity = bucketIndex + gDefaultBucketListIncrement;
            int newMemSize = sizeof(void**) * newBucketListCapacity ;

            void ** newBucketList = static_cast<void**>( Allocate( newMemSize ));
            if (mRoot != nullptr)
            {
                Utils::Memcpy(static_cast<void*>(newBucketList), static_cast<void*>(mRoot), currBucketListByteSize);
                Free(static_cast<void*>(mRoot));
            }
            Utils::Memset8(static_cast<void*>(newBucketList + mBucketListCapacity), 0, newMemSize - currBucketListByteSize); 
            mRoot = newBucketList;
            mBucketListCapacity = newBucketListCapacity;
        }

        // allocate a bucket if it doesn't exist
        void * rawBucket = mRoot[bucketIndex];
        if (rawBucket == nullptr)
        {
            int bucketByteSize = mElementSize * mMaxBucketSize;
            rawBucket = Allocate( bucketByteSize );
            Utils::Memset8(rawBucket, 0, bucketByteSize); 
            mRoot[bucketIndex] = rawBucket;
        }
        PG_ASSERT(rawBucket);
        PG_ASSERTSTR(bucketOffset < mMaxBucketSize, "Out of bounds! this will return garbage!");
        // finally retrieve destination memory and copy to destination memory
        char * destination = &(static_cast<char*>(rawBucket)[bucketOffset * mElementSize]);
        Pegasus::Utils::Memcpy(destination, element, mElementSize);
    }

    bool TesselationTable::Get(int i, int j, char * element)
    {
        int bucketIndex, bucketOffset;
        ConstructIndices(i, j, bucketIndex, bucketOffset);

        if (bucketIndex >= mBucketListCapacity || mRoot == nullptr || mRoot[bucketIndex] == nullptr)
        {
            return false;
        }

        PG_ASSERTSTR(bucketOffset < mMaxBucketSize, "Out of bounds! this will return garbage!");
        char * source = &(reinterpret_cast<char*>(mRoot[bucketIndex])[bucketOffset * mElementSize]);
        Pegasus::Utils::Memcpy(element, source, mElementSize);
        return true;
    }

    void * TesselationTable::Allocate(int size)
    {
        return mAllocator->Alloc(
            (size_t)size,
            Alloc::PG_MEM_TEMP,
            -1,
            "compact table allocation",
            __FILE__,
            __LINE__
        ); 
    }

    void TesselationTable::Free(void * ptr)
    {
        mAllocator->Delete(ptr);
    }
}
}
