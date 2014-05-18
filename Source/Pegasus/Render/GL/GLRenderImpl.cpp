/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLRenderImpl.cpp
//! \author Kleber Garcia
//! \date   25th March of 2014
//! \brief  OpenGL 4.X implementation of PARR

#if PEGASUS_GAPI_GL

#include "Pegasus/Render/Render.h"
#include "../Source/Pegasus/Render/GL/GLGPUDataDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"

void Pegasus::Render::Dispatch (Pegasus::Shader::ProgramLinkageRef program)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * gpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    Pegasus::Render::OGLProgramGPUData * programGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(
         Pegasus::Render::OGLProgramGPUData, 
         gpuData
    );

    PG_ASSERT(programGPUData->mHandle != 0);
    glUseProgram(programGPUData->mHandle);
}

#endif
