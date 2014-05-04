/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLShaderFactory.h
//! \author Kleber Garcia
//! \date   24rd March 2014
//! \brief  Shader factory implementation. Provided to the shader package

#if PEGASUS_GAPI_GL

#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "../Source/Pegasus/Render/GL/GLGpuDataDefs.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Shader/shared/ShaderEvent.h"
#include "Pegasus/Shader/IShaderFactory.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/EventDispatcher.h"
#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Graph/NodeData.h"

namespace PegasusShaderPrivate {

static struct GLPegasusStageMapping 
{
    Pegasus::Shader::ShaderType mType;
    GLuint mGlRenderPlatShaderStage;
} gGLPegasusStageMappings[Pegasus::Shader::SHADER_STAGES_COUNT] =
{
    { Pegasus::Shader::VERTEX                 , GL_VERTEX_SHADER          },
    { Pegasus::Shader::FRAGMENT               , GL_FRAGMENT_SHADER        },
    { Pegasus::Shader::TESSELATION_CONTROL    , GL_TESS_CONTROL_SHADER    },
    { Pegasus::Shader::TESSELATION_EVALUATION , GL_TESS_EVALUATION_SHADER },
    { Pegasus::Shader::GEOMETRY               , GL_GEOMETRY_SHADER        },
    { Pegasus::Shader::COMPUTE                 , GL_COMPUTE_SHADER         }
}
;

}

//! internal definition of shader factory API
class GLShaderFactory : public Pegasus::Shader::IShaderFactory
{
public:
    GLShaderFactory ()
    : mAllocator(nullptr)
    {
    }

    virtual ~GLShaderFactory ()
    {
    }

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);

    virtual void GenerateShaderGpuData (Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData);
    virtual void DestroyShaderGpuData  (Pegasus::Graph::NodeData * nodeData);
    
    virtual void GenerateProgramGpuData (Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData);
    virtual void DestroyProgramGpuData (Pegasus::Graph::NodeData * nodeData);

private:
    Pegasus::Alloc::IAllocator * mAllocator;
    
};

//! define a global static shader factory api
static GLShaderFactory gGlobalStaticFactory;

void GLShaderFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
    mAllocator = allocator;
}

//! processes an error log from opengl compilation
static void ProcessErrorLog(Pegasus::Shader::ShaderStage * shaderNode, const char * log)
{
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    //parsing log to extract line & column
    const char  * s = log;

    //WARNING, I have optimized the crap out of the following code, so parsing might be messy.
    // I am trying to parse the following string:
    // ERROR {row:column}: {Description}
    // OpenGl logs throw errors in this format.

    while (*s)
    {
        int line = 0;
        bool foundNumber = false;
        
        //Skip any non numerical character
        while ( *s != '\0' && (*s < '0' || *s > '9')) ++s;

        //Skip first numerical character
        while ( *s != '\0' && *s >= '0' && *s <= '9') ++s;

        //Skip any non numerical character
        while ( *s != '\0' && (*s < '0' || *s > '9')) ++s;

        //Parse Line number
        for (; *s != '\0' && *s >= '0' && *s <= '9'; ++s)
        {
            foundNumber = true;
            line = 10*line + (*s - '0');
        }

        char descriptionError[512];
        int idx = 0;
        //skip to new line or end of string
        while ( *s != '\0' && *s != '\n') 
        {
            if (idx < 511)
                descriptionError[idx++] = *s; 
            ++s;
        } 
        descriptionError[idx] = '\0';
        SHADEREVENT_COMPILATION_ERROR(shaderNode, line, descriptionError);

    }
#endif
}

//! allocates lazily or returns an existant shader data type
static Pegasus::Render::OglShaderGpuData * GetOrAllocateShaderGpuData(Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OglShaderGpuData * gpuData = nullptr;
    if (nodeData->GetNodeGpuData() == nullptr)
    {
        gpuData = PG_NEW(allocator, -1, "Shader Gpu Data", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OglShaderGpuData();
        
        // initialize
        gpuData->mHandle = 0; //invalid opengl handle
        nodeData->SetNodeGpuData(reinterpret_cast<Pegasus::Graph::NodeGpuData*>(gpuData));
    }
    else 
    {
        gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OglShaderGpuData, nodeData->GetNodeGpuData());
    }

    return gpuData;
}

//! allocates lazily or returns an existant shader data type
static Pegasus::Render::OglProgramGpuData * GetOrAllocateProgramGpuData(Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OglProgramGpuData * gpuData = nullptr;
    if (nodeData->GetNodeGpuData() == nullptr)
    {
        gpuData = PG_NEW(allocator, -1, "Shader Gpu Data", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OglProgramGpuData();
        
        // initialize
        gpuData->mHandle = 0; //invalid opengl handle
        for (unsigned i = 0; i < Pegasus::Shader::SHADER_STAGES_COUNT; ++i)
        {
            gpuData->mShaderCachedHandles[i] = 0;
        }
        nodeData->SetNodeGpuData(reinterpret_cast<Pegasus::Graph::NodeGpuData*>(gpuData));
    }
    else 
    {
        gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OglProgramGpuData, nodeData->GetNodeGpuData());
    }

    return gpuData;
}

//! destruction of a shader gpu data
static void CommonDestroyShaderGpuData (Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    if (nodeData->GetNodeGpuData() != nullptr)
    {
        Pegasus::Render::OglShaderGpuData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OglShaderGpuData, nodeData->GetNodeGpuData());
        if (gpuData->mHandle != 0)
        {
            glDeleteShader(gpuData->mHandle);
        }
        PG_DELETE(allocator, gpuData);
        nodeData->SetNodeGpuData(nullptr);
    }
}

//! destruction of a program gpu data
static void CommonDestroyProgramGpuData (Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    if (nodeData->GetNodeGpuData() != nullptr)
    {
        Pegasus::Render::OglProgramGpuData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OglProgramGpuData, nodeData->GetNodeGpuData());
        if (gpuData->mHandle != 0)
        {
            glDeleteProgram(gpuData->mHandle);
        }
        PG_DELETE(allocator, gpuData);
        nodeData->SetNodeGpuData(nullptr);
    }
}

//! Inject gpu data to shader stage
void GLShaderFactory::GenerateShaderGpuData (Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OglShaderGpuData * gpuData = GetOrAllocateShaderGpuData(mAllocator, nodeData);
    Pegasus::Shader::ShaderType type = shaderNode->GetStageType();
    const char * shaderSource;
    int sourceSize;
    shaderNode->GetSource(&shaderSource, sourceSize);

    if (type == Pegasus::Shader::SHADER_STAGE_INVALID || sourceSize == 0)
    {
        //empty shader, ignore compilation
        SHADEREVENT_COMPILATION_FAIL(shaderNode, "invalid shader");
        return;
    }

    PG_ASSERT(type >= 0 && type < Pegasus::Shader::SHADER_STAGES_COUNT);
    
    if (gpuData->mHandle == 0)
    {
        gpuData->mHandle = glCreateShader(PegasusShaderPrivate::gGLPegasusStageMappings[static_cast<int>(type)].mGlRenderPlatShaderStage);    
    }
    PG_ASSERT(gpuData->mHandle != 0);

    const GLint stringLength = static_cast<GLuint>(sourceSize);
    glShaderSource(
        gpuData->mHandle,
        1, // count
        &static_cast<const GLchar*>(shaderSource), // src (list of strings, on this case just 1 string
        &stringLength // length of each string in the list of strings,
            //opengl samples use NULL, not nullptr
            // NULL in this argument means just assume each string is a NULL terminated string
    );

    glCompileShader(gpuData->mHandle);
    GLint shaderCompiled = GL_TRUE;
    glGetShaderiv(gpuData->mHandle, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled == GL_FALSE)
    {
        //failure shenanigans
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetShaderInfoLog(gpuData->mHandle, bufferSize, &logLength, logBuffer);
        
        ProcessErrorLog(shaderNode, logBuffer);
    
        SHADEREVENT_COMPILATION_FAIL(shaderNode, logBuffer);

    }
    else
    {
        SHADEREVENT_COMPILATION_SUCCESS(shaderNode);
    }
    nodeData->SetNodeGpuData(reinterpret_cast<Pegasus::Graph::NodeGpuData*>(gpuData));
}

//! Destroy gpu data to shader stage
void GLShaderFactory::DestroyShaderGpuData  (Pegasus::Graph::NodeData * nodeData)
{
    CommonDestroyShaderGpuData(mAllocator, nodeData);
}

//! Inject gpu data to program 
void GLShaderFactory::GenerateProgramGpuData (Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OglProgramGpuData * gpuData = GetOrAllocateProgramGpuData(mAllocator, nodeData);
    if (gpuData->mHandle == 0)
    {
        gpuData->mHandle = glCreateProgram();
    }
    GLuint * shaderCachedHandles = gpuData->mShaderCachedHandles;

    for (unsigned i = 0; i < Pegasus::Shader::SHADER_STAGES_COUNT; ++i)
    {
        //detach first all the shaders that this program has attached before
        if (shaderCachedHandles[i] != 0)
        {
            glDetachShader(gpuData->mHandle, shaderCachedHandles[i]);
            shaderCachedHandles[i] = 0; //clear the slot
        }
    }

    bool updated;

    //walk over inputs and attach new shaders
    for (unsigned i = 0; i < programNode->GetNumInputs(); ++i)
    {
        Pegasus::Shader::ShaderStageRef shaderStage = programNode->FindShaderStageInput(i);
        if (shaderStage->GetStageType() != Pegasus::Shader::SHADER_STAGE_INVALID)
        {
            updated = false;
            Pegasus::Graph::NodeDataRef shaderNodeDataRef = shaderStage->GetUpdatedData(updated);
            Pegasus::Render::OglShaderGpuData * shaderStageGpuData = GetOrAllocateShaderGpuData(mAllocator,&(*shaderNodeDataRef));
            glAttachShader(gpuData->mHandle, shaderStageGpuData->mHandle);
            shaderCachedHandles[shaderStage->GetStageType()] = shaderStageGpuData->mHandle;
        }
    }
    
    if (
        !(shaderCachedHandles[Pegasus::Shader::VERTEX] != 0)
        &&  !(
                shaderCachedHandles[Pegasus::Shader::VERTEX] == 0 &&
                shaderCachedHandles[Pegasus::Shader::FRAGMENT] == 0 &&
                shaderCachedHandles[Pegasus::Shader::TESSELATION_CONTROL] == 0 &&
                shaderCachedHandles[Pegasus::Shader::TESSELATION_EVALUATION] == 0 &&
                shaderCachedHandles[Pegasus::Shader::GEOMETRY] == 0 &&
                shaderCachedHandles[Pegasus::Shader::COMPUTE] != 0
            )
    )
    {
        SHADEREVENT_LINKING_INCOMPLETE(programNode);
        return;
    }

    glLinkProgram(gpuData->mHandle); 
    GLint shaderLinked = GL_FALSE;
    glGetProgramiv(gpuData->mHandle, GL_LINK_STATUS, &shaderLinked);
    if (shaderLinked == GL_FALSE)
    {
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetProgramInfoLog(gpuData->mHandle, bufferSize, &logLength, logBuffer);
        SHADEREVENT_LINKING_FAIL(programNode, logBuffer);
    }
    else
    {
        SHADEREVENT_LINKING_SUCCESS(programNode);
    }

}

//! Destroy gpu data to program
void GLShaderFactory::DestroyProgramGpuData (Pegasus::Graph::NodeData * nodeData)
{
    CommonDestroyProgramGpuData(mAllocator, nodeData);
}

//! return statically defined shader factory singleton
//! this function avoids using the heap, since the shader factory must be persistant and holds no state information
//! It instead, acts as a collection of APIs
Pegasus::Shader::IShaderFactory * Pegasus::Render::GetRenderShaderFactory()
{
    return &gGlobalStaticFactory;
}

#endif //PEGASUS_GAPI_GL
