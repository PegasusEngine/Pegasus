/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderPlatProgramLinker.h
//! \author Kleber Garcia
//! \date   17th October 2013
//! \brief  Represents a platform specific program linker. This header provides the basic 
//!         structures necessary for a platform specific implementation in the cpp file.

#ifndef PEGASUS_SHADER_SHADERPROGRAM_H
#define PEGASUS_SHADER_SHADERPROGRAM_H

#include "Pegasus/Shader/RenderPlatShaderStage.h"


namespace Pegasus {
namespace Shader {

//! encapsulates all the linker work
class RenderPlatProgramLinker
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    : public EventDispatcher
#endif
{
public:

    //! Default constructor
    RenderPlatProgramLinker();

    //! Destructor
    ~RenderPlatProgramLinker();
    
    //! Performs linking operation, and attachment operatoin of shaders
    //! \param shaderPipeline array containing the GLuint handles of each stage.
    //! \return returns true on success, false on failure
    bool Link(ShaderHandle shaderPipeline[Pegasus::Shader::SHADER_STAGES_COUNT]);

    //! Returns a program handle of the linked shader
    //! \return The program handle of the linkage operation. call this after calling Link
	ShaderHandle GetProgramHandle() const { return mRenderPlatProgramLinkerHandle; }

private:
    PG_DISABLE_COPY(RenderPlatProgramLinker)

    void RemoveStage(ShaderType stageType);

    ShaderHandle mStages[SHADER_STAGES_COUNT];
    ShaderHandle mRenderPlatProgramLinkerHandle;
};


} // namespace Shader
} // namespace Pegasus

#endif // PEGASUS_SHADER_SHADERPROGRAM_H
