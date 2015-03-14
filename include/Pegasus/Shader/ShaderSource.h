/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file ShaderSource.h
//! \author Kleber Garcia
//! \date 3rd March 2015
//! \brief Pegasus Shader include source file

#ifndef PEGASUS_SHADER_SOURCE_H
#define PEGASUS_SHADER_SOURCE_H

#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Proxy/ShaderProxy.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Graph/GeneratorNode.h"

//fwd declarations
namespace Pegasus {
    namespace Alloc {
        class IAllocator;
    }

    namespace Shader {
        class ShaderTracker;
    }
}

namespace Pegasus
{
namespace Shader
{

class ShaderSource : public Graph::GeneratorNode,  public AssetLib::RuntimeAssetObject
{
    GRAPH_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

public:
    //! constructor
    ShaderSource (Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! destructor
    virtual ~ShaderSource();

    //! Set the shader source, keeping the current shader type
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    virtual void SetSource(const char * src, int srcSize);

    //! Gets the shader source
    //! \param  output string constant pointer
    //! \param  output size of string 
    virtual void GetSource (const char ** outSrc, int& outSize) const;

    //! propagates compilation to all the parents who are including this shader source
    virtual void Compile();

    //! Register a source parent to this shader
    void RegisterParent(ShaderSource* parent);

    //! Unregister a source parent to this shader
    void UnregisterParent(ShaderSource* parent);

    //! Return the stage type
    //! \return the shader type
    virtual ShaderType GetStageType() const { return SHADER_STAGE_INVALID; }

    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Unregisters this source from all its parents
    void ClearParents();


#if PEGASUS_ENABLE_PROXIES
    //! sets internal reference to shader tracker
    //! this is needed so when the shader is deleted, it can remove itself from the tracker
    //! \param tracker the shader tracker to reference.
    void SetShaderTracker(ShaderTracker * tracker) {mShaderTracker = tracker;}

    //! Sets the full path, divides the stirng into the file name and the root full path
    //! to be used only by the editor
    //! \param fullpath file path of the shader to be set
    void SetFullFilePath(const char * fullPath);

    //! \return the path containing this shader
    const char * GetFilePath() const { return mPath; }

    //! \return the file name of this shader
    const char * GetFileName() const { return mName; }

    //! returns the proxy of this particular shader    
    IShaderProxy * GetProxy() { return &mProxy; }
#endif


protected:
    Alloc::IAllocator* mAllocator;

    //! override that generates data. This will generate the shader GPU data using the factory
    virtual void GenerateData();

    //! callback which allocates shader data
    virtual Pegasus::Graph::NodeData * AllocateData() const;

private:

//! editor metadata
#if PEGASUS_ENABLE_PROXIES

    //! Filename metadata
    static const int METADATA_NAME_LENGTH = 256;
    char mName[METADATA_NAME_LENGTH];
    char mPath[METADATA_NAME_LENGTH];
    char mFullPath[METADATA_NAME_LENGTH * 2];
    ShaderTracker * mShaderTracker; //! reference to tracker
    ShaderProxy mProxy;

#endif

    Io::FileBuffer     mFileBuffer; //! buffer structure containing shader source
    Utils::Vector<ShaderSource*> mParents; //references to parents
    bool mLockParentArray;
};

typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceRef;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceIn;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceInOut;
typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceReturn;

}
}

#endif
