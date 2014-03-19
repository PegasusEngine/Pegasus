#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Shader/ShaderData.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"

//! private data structures
namespace PegasusShaderPrivate {


//lookup struct array with extension / GL type / pegasus Shader stage mappings
static struct PegasusExtensionMappings
{
    Pegasus::Shader::ShaderType mType;
    const char * mExtension;
} gPegasusShaderStageExtensionMappings[Pegasus::Shader::SHADER_STAGES_COUNT] =
{
    { Pegasus::Shader::VERTEX,                 ".vs"  },
    { Pegasus::Shader::FRAGMENT,               ".ps"  },
    { Pegasus::Shader::TESSELATION_CONTROL,    ".tcs" },
    { Pegasus::Shader::TESSELATION_EVALUATION, ".tes" },
    { Pegasus::Shader::GEOMETRY,               ".gs"  },
    { Pegasus::Shader::COMPUTE,                ".cs"  }
}
;

} // namespace PegasusShaderPrivate

Pegasus::Shader::ShaderStage::ShaderStage(Pegasus::Alloc::IAllocator * allocator, Pegasus::Alloc::IAllocator * nodeDataAllocator)
    : Pegasus::Graph::GeneratorNode(allocator, nodeDataAllocator), 
      mInternalStage(allocator)
{
}

Pegasus::Shader::ShaderStage::~ShaderStage()
{
}

void Pegasus::Shader::ShaderStage::SetSource(Pegasus::Shader::ShaderType type, const char * src, int srcSize)
{
    if (GetData() != nullptr)
    {
        GetData()->Invalidate();
    }
    mInternalStage.SetSource(type, src, srcSize);
}

bool Pegasus::Shader::ShaderStage::SetSourceFromFile(Pegasus::Shader::ShaderType type, const char * path, Io::IOManager * loader)
{
    if (GetData() != nullptr)
    {
        GetData()->Invalidate();
    }
    return mInternalStage.SetSourceFromFile(type, path, loader);
}

Pegasus::Graph::NodeData * Pegasus::Shader::ShaderStage::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "ShaderData", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ShaderData(GetNodeDataAllocator());
}

#if PEGASUS_SHADER_USE_EDIT_EVENTS
void Pegasus::Shader::ShaderStage::SetEventListener(Pegasus::Shader::IEventListener * eventListener)
{
    mInternalStage.SetEventListener(eventListener);
}

void Pegasus::Shader::ShaderStage::SetUserData(Pegasus::Shader::IUserData * userData)
{
    mInternalStage.SetUserData(userData);
}
#endif //PEGASUS_SHADER_USE_EDIT_EVENTS

void Pegasus::Shader::ShaderStage::GenerateData()
{
    PG_ASSERT(GetData() != nullptr);
    Pegasus::Shader::ShaderDataRef shaderData = GetData();  
    if (mInternalStage.Compile())
    {
        shaderData->SetShaderHandle(mInternalStage.GetCompiledShaderHandle());
    }
    else
    {
        shaderData->SetShaderHandle(Pegasus::Shader::INVALID_SHADER_HANDLE);
    }
} 


//! editor metadata
#if PEGASUS_ENABLE_PROXIES
void Pegasus::Shader::ShaderStage::SetFullFilePath(const char * name)
{
    int len = 0;
    if (name)
    {
        int fullLen = Pegasus::Utils::Strlen(name);
        const char * nameString1 = Pegasus::Utils::Strrchr(name, '/');
        const char * nameString2 = Pegasus::Utils::Strrchr(name, '\\');
        const char * nameString = nameString1 > nameString2 ? nameString1 : nameString2;
        if (nameString != nullptr)
        {
            fullLen = fullLen - (nameString - name + 1);
            Pegasus::Utils::Memcpy(mName, nameString + 1, fullLen);
            mName[fullLen] = '\0';
            fullLen = nameString - name + 1;
            Pegasus::Utils::Memcpy(mPath, name, fullLen);
            mPath[fullLen] = '\0';
        }
        else
        {
            len = fullLen < Pegasus::Shader::ShaderStage::METADATA_NAME_LENGTH - 1 ? fullLen : Pegasus::Shader::ShaderStage::METADATA_NAME_LENGTH - 1;
            Pegasus::Utils::Memcpy(mName, name, len);
            mName[len] = '\0';
            mPath[0] = '\0';
        }
    }
} 
#endif


Pegasus::Graph::NodeReturn Pegasus::Shader::ShaderStage::CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ShaderStage", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ShaderStage(nodeAllocator, nodeDataAllocator);
}
