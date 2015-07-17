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

#include "Pegasus/Core/SourceCode.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Proxy/ShaderProxy.h"
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

class ShaderSource : public Graph::GeneratorNode,  public Core::SourceCode 
{
public:
    //! constructor
    ShaderSource (Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! destructor
    virtual ~ShaderSource();

    //! Return the stage type
    //! \return the shader type
    virtual ShaderType GetStageType() const { return SHADER_STAGE_INVALID; }

    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    virtual void InvalidateData();

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

    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const { return GetFileName(); }

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
};

typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceRef;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceIn;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceInOut;
typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceReturn;

}
}

#endif
