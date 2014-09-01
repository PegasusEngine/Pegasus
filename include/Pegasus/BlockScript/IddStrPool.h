/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IddStrPool.h
//! \author Kleber Garcia
//! \date   31th August 2014
//! \brief  String pool allocator for identifiers.

#ifndef IDD_STR_POOL_H
#define IDD_STR_POOL_H

namespace Pegasus
{

//fwd declarations
namespace Alloc
{
    class IAllocator;
}

namespace BlockScript
{

//! class that preallocates string pools, minimizing calls to malloc.
class IddStrPool
{
public:

    static const int sMaxPages = 16;
    static const int sMaxStringsPerPages = 64;
    static const int sCharsPerString = 64;
    static const int sPageByteSize = sCharsPerString * sMaxStringsPerPages;
    static const int sMaxStrings = sMaxStringsPerPages * sMaxPages;
    

    //! Constructor
    IddStrPool();

    //! Destructor
    ~IddStrPool();

    //! Initializes the identifier string pool
    void Initialize(Alloc::IAllocator * allocator);

    //! Clears the identifier string pool
    void Clear();

    //! Allocates a string in the cached pages
    char* AllocateString();

    //! Get page count
    int GetPageCount() const { return mStringCount == 0 ? 0 : (mStringCount / sMaxStringsPerPages + 1); }

    //! GetString count
    int GetStringCount() const { return mStringCount; }

private:

    void AllocatePage();
    
    Alloc::IAllocator* mAllocator;
    char* mPages[sMaxPages];
    int   mStringCount;

    
};

}
}

#endif
