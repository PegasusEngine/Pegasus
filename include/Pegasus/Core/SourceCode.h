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

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Graph/GeneratorNode.h"

//fwd declarations
namespace Pegasus
{
    namespace AssetLib {
        class AssetLib;
        class Asset;
    }
}

namespace Pegasus
{
namespace Core
{

class SourceCode : public Graph::GeneratorNode, public AssetLib::RuntimeAssetObject
{
    PEGASUS_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

public:
    SourceCode(Alloc::IAllocator* allocator, Alloc::IAllocator* nodeDatallocator);
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

#if PEGASUS_ENABLE_PROXIES
    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const; 

#endif

    
protected:
    Io::FileBuffer             mFileBuffer; //! buffer structure containing shader source
    Alloc::IAllocator* mAllocator;
    Utils::Vector<SourceCode*> mParents; //references to parents

    virtual bool OnReadAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset);

    virtual void OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset);

private:


    bool mLockParentArray;
};

}
}
#endif
