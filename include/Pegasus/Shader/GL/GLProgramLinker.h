/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLProgramLinker.h
//! \author Kleber Garcia
//! \date   17th October 2013
//! \brief  Represents an opengl program linker

#ifndef PEGASUS_SHADER_SHADERPROGRAM_H
#define PEGASUS_SHADER_SHADERPROGRAM_H

#include "Pegasus/Shader/GL/GLShaderStage.h"
#include "Pegasus/Render/GL/GLEWStaticInclude.h"

namespace Pegasus {
namespace Shader {

//! encapsulates all the linker work
class GLProgramLinker
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    : public EventDispatcher
#endif
{
public:

    //! Default constructor
    GLProgramLinker();

    //! Destructor
    ~GLProgramLinker();
    
    //! Performs linking operation, and attachment operatoin of shaders
    //! \param shaderPipeline array containing the GLuint handles of each stage.
    //! \return returns true on success, false on failure
    bool Link(GLuint shaderPipeline[Pegasus::Shader::SHADER_STAGES_COUNT]);

    //! Returns a program handle of the linked shader
    //! \return The program handle of the linkage operation. call this after calling Link
	GLuint GetProgramHandle() const { return mGLProgramLinkerHandle; }

private:
    PG_DISABLE_COPY(GLProgramLinker)

    void RemoveStage(ShaderType stageType);

    GLuint mStages[SHADER_STAGES_COUNT];
    GLuint mGLProgramLinkerHandle;
};


} // namespace Shader
} // namespace Pegasus

#endif // PEGASUS_SHADER_SHADERPROGRAM_H
