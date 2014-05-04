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
#include "../Source/Pegasus/Render/GL/GLGpuDataDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"

void Pegasus::Render::Dispatch (Pegasus::Shader::ProgramLinkageRef program)
{
    bool updated = false;
    Pegasus::Graph::NodeGpuData * gpuData = program->GetUpdatedData(updated)->GetNodeGpuData();
    Pegasus::Render::OglProgramGpuData * programGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(
         Pegasus::Render::OglProgramGpuData, 
         gpuData
    );

    PG_ASSERT(programGpuData->mHandle != 0);
    glUseProgram(programGpuData->mHandle);
}

#endif
