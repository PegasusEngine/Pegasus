/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderStage.h
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Pegasus Shader Stage	
#ifndef PEGASUS_SHADER_STAGE_H
#define PEGASUS_SHADER_STAGE_H
#include "Pegasus/Core/Ref.h"
#include "Pegasus/Graph/GeneratorNode.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/Proxy/ShaderProxy.h"
#include "Pegasus/Core/Io.h"


namespace Pegasus
{
namespace Shader
{

// forward declarations
class IShaderFactory;
class ShaderTracker;

//! Shader Stage class, holds information about a shader stage
class ShaderStage : public Graph::GeneratorNode
{
    friend class ShaderManager;
    GRAPH_EVENT_DECLARE_DISPATCHER(IShaderEventListener)

public:
    //! Default constructor
    //! \param  nodeAllocator used for nodes
    //! \param  nodeDataAllocator used for data allocation of nodes
    ShaderStage(
        Alloc::IAllocator* nodeAllocator,
        Alloc::IAllocator* nodeDataAllocator
    );

    //! Destructor
    virtual ~ShaderStage();

    //! Set the shader source
    //! \param  type the type of the source
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    void SetSource(ShaderType type, const char * src, int srcSize);

    //! Set the shader source, keeping the current shader type
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    void SetSource(const char * src, int srcSize);

    //! Gets the shader source
    //! \param  output string constant pointer
    //! \param  output size of string 
    void GetSource (const char ** outSrc, int& outSize) const;

    //! Open a file and load its source internally
    //! \note - must set the IO manager before doing this call!
    //! \param  type the type of shader stage
    //! \param  path the path of the file to open
    //! \param  loader loader controller
    //! \return  true if succeeds loading the file, false if loading fails
    bool SetSourceFromFile(ShaderType type, const char * path);

    //! Return the stage type
    //! \return the shader type
    ShaderType GetStageType() const { return mType; }

    //! Sets the factory, which contains the render library implementation of shader
    //! compilation and linkage
    //! \param factory factory with GPU implementation to be set.
    void SetFactory(IShaderFactory * factory) { mFactory = factory; }

    //! Sets the io manager of this particular shader (for file IO operations)
    //! NOTE - must be set before doing any IO file loading operatoin.
    //! \param loader the actual loader to be used
    void SetIoManager(Pegasus::Io::IOManager * loader) { mLoader = loader;}

    //! Deallocate the data of the current node and ask the input nodes to do the same.
    //! Typically used when keeping the graph in memory but not the associated data,
    //! to save memory and to be able to restore the data later
    virtual void ReleaseDataAndPropagate();

    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);


#if PEGASUS_ENABLE_PROXIES
    //! Sets the full path, divides the stirng into the file name and the root full path
    //! to be used only by the editor
    //! \param fullpath file path of the shader to be set
    void SetFullFilePath(const char * fullPath);

    //! \return the path containing this shader
    const char * GetFilePath() const { return mPath; }

    //! \return the file name of this shader
    const char * GetFileName() const { return mName; }

    //! save the source to a file using the file loader
    void SaveSourceToFile();

    //! returns the proxy of this particular shader    
    IShaderProxy * GetProxy() { return &mProxy; }
#endif

protected:
    //! callback which allocates shader data
    virtual Pegasus::Graph::NodeData * AllocateData() const;

    //! override that generates data. This will generate the shader GPU data using the factory
    virtual void GenerateData();

private:
    PG_DISABLE_COPY(ShaderStage)
    Io::FileBuffer     mFileBuffer; //! buffer structure containing shader source
    Alloc::IAllocator* mAllocator; //! Allocator to use when creating this object
    IShaderFactory   * mFactory; //! reference to GPU shader factory
    ShaderType         mType; //! type of shader stage
    Pegasus::Io::IOManager * mLoader; //! reference to a loader

//! editor metadata
#if PEGASUS_ENABLE_PROXIES
    //! sets internal reference to shader tracker
    //! this is needed so when the shader is deleted, it can remove itself from the tracker
    //! \param tracker the shader tracker to reference.
    void SetShaderTracker(ShaderTracker * tracker) {mShaderTracker = tracker;}

    //! Filename metadata
    static const int METADATA_NAME_LENGTH = 256;
    char mName[METADATA_NAME_LENGTH];
    char mPath[METADATA_NAME_LENGTH];
    char mFullPath[METADATA_NAME_LENGTH * 2];
    ShaderTracker * mShaderTracker; //! reference to tracker
    ShaderProxy mProxy;

#endif

};

typedef Pegasus::Core::Ref<ShaderStage> ShaderStageRef;
typedef Pegasus::Core::Ref<ShaderStage> & ShaderStageIn;
typedef Pegasus::Core::Ref<ShaderStage> & ShaderStageInOut;
typedef Pegasus::Core::Ref<ShaderStage> ShaderStageReturn;

}
}

#endif
