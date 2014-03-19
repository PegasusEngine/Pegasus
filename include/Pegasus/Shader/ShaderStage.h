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
#include "Pegasus/Shader/RenderPlatShaderStage.h"
#include "Pegasus/Core/Io.h"

namespace Pegasus
{
namespace Shader
{

// forward declarations
class IEventListener;
class IUserData;

//! Shader Stage class, holds information about a shader stage
class ShaderStage : public Graph::GeneratorNode
{
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


    //! Open a file and load its source internally
    //! \param  type the type of shader stage
    //! \param  path the path of the file to open
    //! \param  loader loader controller
    //! \return  true if succeeds loading the file, false if loading fails
    bool SetSourceFromFile(ShaderType type, const char * path, Io::IOManager * loader);

    //! Return the stage type
    //! \return the shader type
    ShaderType GetStageType() const { return mInternalStage.GetStageType(); }

#if PEGASUS_SHADER_USE_EDIT_EVENTS
    //! Set event listener for shader stage
    //! \param eventListener the event listener interface
    void SetEventListener(IEventListener * eventListener);

    //! Set user data of particular shader stage
    //! \param userData user data returned on the execution of an event. Use this 
    void SetUserData(IUserData * userData);
#endif

    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

#if PEGASUS_ENABLE_PROXIES
    void SetFullFilePath(const char * fullPath);
    const char * GetFilePath() const { return mPath; }
    const char * GetFileName() const { return mName; }
#endif

protected:
    virtual Pegasus::Graph::NodeData * AllocateData() const;
    virtual void GenerateData();

private:
    PG_DISABLE_COPY(ShaderStage)
    RenderPlatShaderStage mInternalStage;

//! editor metadata
#if PEGASUS_ENABLE_PROXIES
    static const int METADATA_NAME_LENGTH = 256;
    char mName[METADATA_NAME_LENGTH];
    char mPath[METADATA_NAME_LENGTH];
#endif

};

typedef Pegasus::Core::Ref<ShaderStage> ShaderStageRef;
typedef Pegasus::Core::Ref<ShaderStage> & ShaderStageIn;
typedef Pegasus::Core::Ref<ShaderStage> & ShaderStageInOut;
typedef Pegasus::Core::Ref<ShaderStage> ShaderStageReturn;

}
}

#endif
