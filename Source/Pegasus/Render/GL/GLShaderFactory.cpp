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
#include "../Source/Pegasus/Render/GL/GLGPUDataDefs.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Shader/shared/ShaderEvent.h"
#include "Pegasus/Shader/IShaderFactory.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Graph/NodeData.h"

namespace PegasusShaderPrivate {

static struct GLPegasusStageMapping 
{
    Pegasus::Shader::ShaderType mType;
    GLuint mGLRenderPlatShaderStage;
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

    virtual void GenerateShaderGPUData (Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData);
    virtual void DestroyShaderGPUData  (Pegasus::Graph::NodeData * nodeData);
    
    virtual void GenerateProgramGPUData (Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData);
    virtual void DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData);

    static short sProgramGUIDCounter;

private:
    Pegasus::Alloc::IAllocator * mAllocator;
    
};

short GLShaderFactory::sProgramGUIDCounter = 1;

//! define a global static shader factory API
static GLShaderFactory gGlobalStaticFactory;

void GLShaderFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
    mAllocator = allocator;
}

//! processes an error log from OpenGL compilation
static int ProcessErrorLog(Pegasus::Shader::ShaderStage * shaderNode, const char * log)
{
    int errorCount = 0;
#if PEGASUS_USE_GRAPH_EVENTS 
    //parsing log to extract line & column
    const char  * s = log;

    //WARNING, I have optimized the crap out of the following code, so parsing might be messy.
    // I am trying to parse the following string:
    // ERROR {row:column}: {Description}
    // OpenGL logs throw errors in this format.

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

        if (foundNumber)
        {
            ++errorCount;
            char descriptionError[512];
            int idx = 0;
            //  skip to new line or end of string
            while ( *s != '\0' && *s != '\n') 
            {
                if (idx < 511)
                    descriptionError[idx++] = *s; 
                ++s;
            } 
            descriptionError[idx] = '\0';
            GRAPH_EVENT_DISPATCH(
                shaderNode,
                Pegasus::Shader::CompilationNotification,
                // Shader Event specific arguments
                Pegasus::Shader::CompilationNotification::COMPILATION_ERROR,
                line,
                descriptionError
            );
        }

    }
#endif
    return errorCount;
}

//! allocates lazily or returns an existent shader data type
static Pegasus::Render::OGLShaderGPUData * GetOrAllocateShaderGPUData(Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OGLShaderGPUData * gpuData = nullptr;
    if (nodeData->GetNodeGPUData() == nullptr)
    {
        gpuData = PG_NEW(allocator, -1, "Shader GPU Data", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OGLShaderGPUData();
        
        // initialize
        gpuData->mHandle = 0; //invalid OpenGL handle
        nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(gpuData));
    }
    else 
    {
        gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLShaderGPUData, nodeData->GetNodeGPUData());
    }

    return gpuData;
}

//! allocates lazily or returns an existent shader data type
static Pegasus::Render::OGLProgramGPUData * GetOrAllocateProgramGPUData(Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OGLProgramGPUData * gpuData = nullptr;
    if (nodeData->GetNodeGPUData() == nullptr)
    {
        gpuData = PG_NEW(allocator, -1, "Shader GPU Data", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OGLProgramGPUData();
        
        // initialize
        gpuData->mHandle = 0; //invalid OpenGL handle
        for (unsigned i = 0; i < Pegasus::Shader::SHADER_STAGES_COUNT; ++i)
        {
            gpuData->mShaderCachedHandles[i] = 0;
        }
        //initializes reflection data to a null state
        bool unusedTriggerUnalignWarning = false;
        Pegasus::Render::PopulateReflectionInfo(nullptr, 0, gpuData->mReflection, unusedTriggerUnalignWarning);

        //assign a GUID
        gpuData->mGUID = GLShaderFactory::sProgramGUIDCounter++;
        gpuData->mVersion = 0; //first version
        nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(gpuData));
    }
    else 
    {
        gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLProgramGPUData, nodeData->GetNodeGPUData());
    }

    return gpuData;
}

//! destruction of a shader GPU data
static void CommonDestroyShaderGPUData (Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    if (nodeData->GetNodeGPUData() != nullptr)
    {
        Pegasus::Render::OGLShaderGPUData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLShaderGPUData, nodeData->GetNodeGPUData());
        if (gpuData->mHandle != 0)
        {
            glDeleteShader(gpuData->mHandle);
        }
        PG_DELETE(allocator, gpuData);
        nodeData->SetNodeGPUData(nullptr);
    }
}

//! destruction of a program GPU data
static void CommonDestroyProgramGPUData (Pegasus::Alloc::IAllocator * allocator, Pegasus::Graph::NodeData * nodeData)
{
    if (nodeData->GetNodeGPUData() != nullptr)
    {
        Pegasus::Render::OGLProgramGPUData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLProgramGPUData, nodeData->GetNodeGPUData());

        Pegasus::Render::DestroyReflectionInfo(allocator, gpuData->mReflection);
        if (gpuData->mHandle != 0)
        {
            glDeleteProgram(gpuData->mHandle);
        }
        PG_DELETE(allocator, gpuData);
        nodeData->SetNodeGPUData(nullptr);
    }
}

//! Inject GPU data to shader stage
void GLShaderFactory::GenerateShaderGPUData (Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OGLShaderGPUData * gpuData = GetOrAllocateShaderGPUData(mAllocator, nodeData);
    Pegasus::Shader::ShaderType type = shaderNode->GetStageType();
    const char * shaderSource;
    int sourceSize;
    shaderNode->GetSource(&shaderSource, sourceSize);

    if (type == Pegasus::Shader::SHADER_STAGE_INVALID || sourceSize == 0)
    {
        //empty shader, ignore compilation
        GRAPH_EVENT_DISPATCH (
            shaderNode,
            Pegasus::Shader::CompilationEvent,
            // Event specific arguments
            false, //compilation success status
            "invalid shader"
        );
        return;
    }

    PG_ASSERT(type >= 0 && type < Pegasus::Shader::SHADER_STAGES_COUNT);
    
    if (gpuData->mHandle == 0)
    {
        gpuData->mHandle = glCreateShader(PegasusShaderPrivate::gGLPegasusStageMappings[static_cast<int>(type)].mGLRenderPlatShaderStage);    
    }
    PG_ASSERT(gpuData->mHandle != 0);

    const GLint stringLength = static_cast<GLuint>(sourceSize);
    glShaderSource(
        gpuData->mHandle,
        1, // count
        &static_cast<const GLchar*>(shaderSource), // src (list of strings, on this case just 1 string
        &stringLength // length of each string in the list of strings,
            //OpenGL samples use NULL, not nullptr
            // NULL in this argument means just assume each string is a NULL terminated string
    );

    glCompileShader(gpuData->mHandle);
    GLint shaderCompiled = GL_TRUE;
    glGetShaderiv(gpuData->mHandle, GL_COMPILE_STATUS, &shaderCompiled);
    //failure shenanigans
    const GLsizei bufferSize = 256;
    char logBuffer[bufferSize];
    GLsizei logLength = 0;
    glGetShaderInfoLog(gpuData->mHandle, bufferSize, &logLength, logBuffer);
       
    int errorCount = ProcessErrorLog(shaderNode, logBuffer);
    shaderCompiled = shaderCompiled && errorCount == 0;
    GRAPH_EVENT_DISPATCH (
        shaderNode,
        Pegasus::Shader::CompilationEvent,
        // Event specific arguments
        shaderCompiled ? true : false, //compilation success status
        shaderCompiled ? "" : logBuffer
    );

#if PEGASUS_ENABLE_LOG
    if (!shaderCompiled)
    {
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "(%s)Shader Compilation Failure: %s", shaderNode->GetFileName(), logBuffer);
#else
        PG_LOG('ERR_', "Shader Compilation Failure: %s", logBuffer);
#endif
    }
#endif

    nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(gpuData));

    // Since the shader data has been updated, set the node GPU data as non-dirty
    nodeData->ValidateGPUData();
}

//! Destroy GPU data to shader stage
void GLShaderFactory::DestroyShaderGPUData  (Pegasus::Graph::NodeData * nodeData)
{
    CommonDestroyShaderGPUData(mAllocator, nodeData);
}

//! Inject GPU data to program 
void GLShaderFactory::GenerateProgramGPUData (Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::OGLProgramGPUData * gpuData = GetOrAllocateProgramGPUData(mAllocator, nodeData);
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
            Pegasus::Render::OGLShaderGPUData * shaderStageGPUData = GetOrAllocateShaderGPUData(mAllocator,&(*shaderNodeDataRef));
            glAttachShader(gpuData->mHandle, shaderStageGPUData->mHandle);
            shaderCachedHandles[shaderStage->GetStageType()] = shaderStageGPUData->mHandle;
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
        GRAPH_EVENT_DISPATCH (
            programNode,
            Pegasus::Shader::LinkingEvent,
            // Event specific arguments:
            Pegasus::Shader::LinkingEvent::INCOMPLETE_STAGES_FAIL,
            ""
        );

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
        GRAPH_EVENT_DISPATCH (
            programNode,
            Pegasus::Shader::LinkingEvent,
            // Event specific arguments:
            Pegasus::Shader::LinkingEvent::LINKING_FAIL,
            ""
        );
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "(%s)Shader Link Failure: %s", programNode->GetName(), logBuffer);
#else
        PG_LOG('ERR_', "Shader Link Failure: %s", logBuffer);
#endif
    }
    else
    {
        
        //output variable from reflector
        bool triggerAlignmentWarning = false;
        // populate reflection data for quick draw calls in meshes
        Pegasus::Render::PopulateReflectionInfo(
            mAllocator, 
            gpuData->mHandle, 
            gpuData->mReflection,
            triggerAlignmentWarning
        );

        gpuData->mVersion++; //shader has been compiled, increase the version

        if (triggerAlignmentWarning)
        {
            GRAPH_EVENT_DISPATCH (
                programNode,
                Pegasus::Shader::LinkingEvent,
                // Event specific arguments:
                Pegasus::Shader::LinkingEvent::LINKING_FAIL,
                "There is a buffer alignment issue. Check the Console for more details."
            );
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "(%s)Shader Link Failure (byte alignment issue)");
#else
        PG_LOG('ERR_', "Shader Link Failure (byte alignment issue");
#endif
        }
        else
        {
            GRAPH_EVENT_DISPATCH (
                programNode,
                Pegasus::Shader::LinkingEvent,
                // Event specific arguments:
                Pegasus::Shader::LinkingEvent::LINKING_SUCCESS,
                ""
            );
        }
    }

    // Since the shader data has been updated, set the node GPU data as non-dirty
    nodeData->ValidateGPUData();
}

//! Destroy GPU data to program
void GLShaderFactory::DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData)
{
    CommonDestroyProgramGPUData(mAllocator, nodeData);
}

//! return statically defined shader factory singleton
//! this function avoids using the heap, since the shader factory must be persistant and holds no state information
//! It instead, acts as a collection of APIs
Pegasus::Shader::IShaderFactory * Pegasus::Render::GetRenderShaderFactory()
{
    return &gGlobalStaticFactory;
}

#endif //PEGASUS_GAPI_GL
