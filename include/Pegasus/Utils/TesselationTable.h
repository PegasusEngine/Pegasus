/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TesselationTable.h
//! \author	Kleber Garcia
//! \date	29th June 2014
//! \brief	Class representing a table stored compact in memory, to increase cache access
//!         efficiency. Use this table to store values that have been generated from two parent
//!         indices. This table stores values independently on their index orders. Its
//!         best application is to be used as a double sided connectivity graph.

#ifndef PEGASUS_UTILS_TESSELATION_TABLE
#define PEGASUS_UTILS_TESSELATION_TABLE

namespace Pegasus
{

namespace Alloc
{
    class IAllocator;
}

namespace Utils
{

//! Compact Table - a table representing connectivity of a double linked graph
class TesselationTable
{
public:
    //! Constructor
    //! \param alloc - allocator for dynamic memory
    //! \param elementSize - the size of each node to be stored. Use the sizeof
    //!        operator to store structs or classes
    TesselationTable(Alloc::IAllocator * alloc, int elementSize, int maxBucketSize = 512);

    //! Destructor
    virtual ~TesselationTable();

    //! \brief Inserts an element (piece of information) on a connection between
    //!        two nodes (or vertex indices). The order in which i or j are inserted is
    //!        independent. Meaning that Insert(i,j,e) == Insert(j,i,e) is the same operation
    //! \param i the first index
    //! \param j the second index
    //! \param element the element to store
    template <class T>
    void Insert(int i, int j, const T& element);
    
    //! \brief Gets a particular element and inserts it in the element pointer passed.
    //!        the order of the indexes is irrelevant of the location, meaning that
    //!        Get(i,j,e) is an isoform of Get(j,i.e)
    //! \param i the first index
    //! \param j the second index
    //! \param piece of memory to retrieve
    //! \return true if the particular element has been found, false otherwise.
    //!         be warry, since elements are stored in order, if we insert two elements that are
    //!         very apart (i.e. indexes (0,0) and (89,0)) then we might have to fill data inbetween,
    //!         therefor this function will return true, with element filled with 0s.
    template <class T>
    bool Get(int i, int j, T& element);

    //! clears all data from the table
    void Clear();

private:
    
    //! internal implementation of insert
    //! \param i first index
    //! \param j second index
    //! \param element the element to insert
    void Insert(int i, int j, const char * element);

    
    //! internal implementation of Get
    //! \param i first index
    //! \param j second index
    //! \param element the element to fill with data
    //! \return true if the element found, false otherwise.
    bool Get(int i, int j, char * element);
   
    //! Allocation convenience function
    //! \param size in bytes to allocate
    void * Allocate(int size);

    //! Free convenience function
    //! \param ptr the pointer to free
    void   Free(void * ptr);

    //! retrieves the bucketIndex and bucketOffset which the memory requested
    //! belongs to.
    //! \param i - the first index
    //! \param j - the second index
    //! \param bucketIndex - output parameter of the bucket index
    //! \param bucketOffset - output parameter of the bucket offset
    void  ConstructIndices(int i, int j, int& bucketIndex, int& bucketOffset) const;

    //! allocator reference
    Alloc::IAllocator * mAllocator;

    //! actual data of the table
    void ** mRoot;

    //! the size of the leafs, in bytes
    int mElementSize;

    //! the size of each bucket, a bucket being a set of elements. 
    //! size is in number of elements (not bytes)
    int mMaxBucketSize;
    
    //! maximum number of buckets this structure can hold in a master 
    //! bucket list. 
    int mBucketListCapacity;
};

template <class T>
void TesselationTable::Insert(int i, int j, const T& element)
{
    PG_ASSERTSTR(sizeof(T) == mElementSize, "FATAL! this will copy garbage to the target bucket");
    Insert(i, j, reinterpret_cast<const char*>(&element));
}

template <class T>
bool TesselationTable::Get(int i, int j, T& element)
{
    PG_ASSERTSTR(sizeof(T) == mElementSize, "FATAL! be ready to have the stack trashed.");
    return Get(i, j, reinterpret_cast<char*>(&element));
}

}
}

#endif
