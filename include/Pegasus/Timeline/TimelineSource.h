/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineSource.h
//! \author	Kleber Garcia
//! \date	March 15 2015
//! \brief	Script base file containing source

#ifndef TIMELINE_SOURCE_H
#define TIMELINE_SOURCE_H

#include "Pegasus/Timeline/Proxy/TimelineScriptProxy.h"
#include "Pegasus/Core/SourceCode.h"
#include "Pegasus/Core/Ref.h"
#include "Pegasus/Allocator/IAllocator.h"

#define MAX_SCRIPT_NAME 64

namespace Pegasus {
    namespace Io {
        class FileBuffer;
    }

    namespace AssetLib {
        class Asset;
    }
}

namespace Pegasus
{
namespace Timeline
{

class TimelineSource : public Core::SourceCode
{
public:
    //! constructor
    explicit TimelineSource(Alloc::IAllocator* allocator);

    //! destructor
    virtual ~TimelineSource();

    //! Call to set this script as dirty
    virtual void InvalidateData() {}

    //! Force this script to compile
    virtual void Compile();

#if PEGASUS_ENABLE_PROXIES
    //! Gets the proxy 
    //! \return Proxy to this script
    virtual AssetLib::IRuntimeAssetObjectProxy* GetProxy() { return &mProxy; }
    virtual const AssetLib::IRuntimeAssetObjectProxy* GetProxy() const { return &mProxy; }

#endif
    
    //! Allocate the data associated with the node
    //! \warning To be redefined by each class defining a new class for its data
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual Graph::NodeData * AllocateData() const { return nullptr; }

    //! Generate the content of the data associated with the node
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() {}
private:

#if PEGASUS_ENABLE_PROXIES
    TimelineScriptProxy mProxy;
#endif
};

//! Reference to a Node, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<TimelineSource>   TimelineSourceRef;

//! Const reference to a reference to a Node, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<TimelineSource> & TimelineSourceIn;

//! Reference to a reference to a Node, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<TimelineSource> & TimelineSourceInOut;

//! Reference to a Node, typically used as the return value of a function
typedef       Pegasus::Core::Ref<TimelineSource>   TimelineSourceReturn;

}
}

#endif
