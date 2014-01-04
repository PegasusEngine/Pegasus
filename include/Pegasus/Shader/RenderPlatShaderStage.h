/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderPlatShaderStage.h
//! \author Kleber Garcia
//! \date   15th October 2013
//! \brief  Platform specific shading pipeline stage header interface.

#ifndef PEGASUS_SHADER_RenderPlatShaderStage_H
#define PEGASUS_SHADER_RenderPlatShaderStage_H

#include "Pegasus/Shader/shared/ShaderDefs.h"
#include "Pegasus/Shader/EventDispatcher.h"
#include "Pegasus/Shader/ShaderData.h"
#include "Pegasus/Core/Io.h"

namespace Pegasus 
{
namespace Shader 
{


//----------------------------------------------------------------------------------------

//! Base shader stage class. Encapsulates all specific opengl functionality. Exposes GLuint handles.
class RenderPlatShaderStage 
#if PEGASUS_SHADER_USE_EDIT_EVENTS
: public EventDispatcher
#endif
{
    friend class RenderPlatProgramLinker;
    friend class ShaderStage;
public:

    //! Default constructor
    //! \param  alloc used for specific allocations of shader src strings
    RenderPlatShaderStage(Alloc::IAllocator* alloc);
    
    //! Destructor
    ~RenderPlatShaderStage();

    //! Set the shader source
    //! \param  type the type of the source
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    void SetSource(ShaderType type, const char * src, int buffSize);

    //! Open a file and load its source internally
    //! \param  type the type of the shader source
    //! \param  path the path of the file to open
    //! \param  loader loader controller
    //! \return true if succeeds loading the file, false if loading fails
    bool SetSourceFromFile(ShaderType type, const char* path, Io::IOManager* loader);
        

    //! Compile and load internally the shader handle
    //! \return return value for compilation
    bool Compile();

    //! Return the shader opengl handle
    //! \return opengl handle
    ShaderHandle GetCompiledShaderHandle() { return mShaderHandle; }

    //! Return the stage type
    //! \return the shader type
    ShaderType GetStageType() const { return mType; }

private:
    PG_DISABLE_COPY(RenderPlatShaderStage);

    void DestroyShader();
    void ProcessErrorLog(const char * errorLog);
    
    Alloc::IAllocator* mAllocator;
    Pegasus::Io::FileBuffer mFileBuffer;
    ShaderType mType;

    ShaderHandle mShaderHandle;
};


} // namespace Shader
} // namespace Pegasus

#endif // PEGASUS_SHADER_SHADERSTAGE_H
