/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file SourceCode.h
//! \author Kleber Garcia
//! \date 15th March 2015
//! \brief Pegasus core source code class. Represents a source code piece with a graph list of
//!        parents. Each parent can be thought of a file including this source.

#ifndef PEGASUS_SOURCE_CODE_H
#define PEGASUS_SOURCE_CODE_H

#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{
namespace Core
{

class SourceCode : public AssetLib::RuntimeAssetObject
{
    GRAPH_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

public:
    SourceCode(Alloc::IAllocator* allocator);
    virtual ~SourceCode();

    //! Set the source
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    virtual void SetSource(const char * src, int srcSize);

    //! Gets the source
    //! \param  output string constant pointer
    //! \param  output size of string 
    virtual void GetSource (const char ** outSrc, int& outSize) const;

    //! propagates compilation to all the parents who are including this source
    virtual void Compile();

    //! Register a source parent to this shader
    void RegisterParent(SourceCode* parent);

    //! Unregister a source parent to this shader
    void UnregisterParent(SourceCode* parent);

    //! Unregisters this source from all its parents
    void ClearParents();

    //! Called when the internal compiled data gets invalidated
    virtual void InvalidateData() = 0;

protected:
    Io::FileBuffer             mFileBuffer; //! buffer structure containing shader source
    Alloc::IAllocator* mAllocator;
    Utils::Vector<SourceCode*> mParents; //references to parents

private:
    bool mLockParentArray;
};

}
}
#endif
