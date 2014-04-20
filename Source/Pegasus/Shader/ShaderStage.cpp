/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderStage.cpp
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Pegasus Shader Stage	

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Shader/IShaderFactory.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Shader/ShaderTracker.h"


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
      mAllocator(allocator),
      mType(Pegasus::Shader::SHADER_STAGE_INVALID),
      mFactory(nullptr)
#if PEGASUS_ENABLE_PROXIES
      , mShaderTracker(nullptr)
#endif
{
}

Pegasus::Shader::ShaderStage::~ShaderStage()
{
    if (GetData() != nullptr)
    {
        mFactory->DestroyShaderGpuData(&(*GetData()));
    }
#if PEGASUS_ENABLE_PROXIES
    if (mShaderTracker != nullptr)
    {
        mShaderTracker->DeleteShader(this);
    }
#endif

}

void Pegasus::Shader::ShaderStage::ReleaseDataAndPropagate()
{
    if (GetData() != nullptr)
    {
        mFactory->DestroyShaderGpuData(&(*GetData()));
    }
    Pegasus::Graph::Node::ReleaseDataAndPropagate();
}

void Pegasus::Shader::ShaderStage::SetSource(Pegasus::Shader::ShaderType type, const char * src, int srcSize)
{
    //! mark data as dirty
    if (GetData() != nullptr)
    {
        GetData()->Invalidate();
    }

    //reallocate buffer size if more space requested on recompilation
    if (srcSize > mFileBuffer.GetFileSize())
    {
        mFileBuffer.DestroyBuffer();
        mFileBuffer.OwnBuffer (
            mAllocator,
            PG_NEW_ARRAY(mAllocator, -1, "shader src", Pegasus::Alloc::PG_MEM_PERM, char, srcSize),
            srcSize
        );
    }
    mFileBuffer.SetFileSize(srcSize);
    PG_ASSERTSTR(mFileBuffer.GetBufferSize() >= srcSize, "Not enough size to hold the string buffer!");
    Pegasus::Utils::Memcpy(mFileBuffer.GetBuffer(),src,srcSize);
    mType = type;
}

void Pegasus::Shader::ShaderStage::SetSource(const char * src, int srcSize)
{
    PG_ASSERT(mType != Pegasus::Shader::SHADER_STAGE_INVALID);
    SetSource(mType, src, srcSize);
}

void Pegasus::Shader::ShaderStage::GetSource ( const char ** outSrc, int& outSize) const
{
    *outSrc = mFileBuffer.GetBuffer(); 
    outSize = mFileBuffer.GetFileSize();
}

bool Pegasus::Shader::ShaderStage::SetSourceFromFile(Pegasus::Shader::ShaderType type, const char * path, Io::IOManager * loader)
{
    if (GetData() != nullptr)
    {
        GetData()->Invalidate();
    }

    PG_ASSERT(path != nullptr);
    mType = type;
    if (mType != Pegasus::Shader::SHADER_STAGE_INVALID)
    {
        mFileBuffer.DestroyBuffer(); //clear any buffers pre-allocated to this
        Pegasus::Io::IoError ioError = loader->OpenFileToBuffer(path, mFileBuffer, true, mAllocator);
        if (ioError == Pegasus::Io::ERR_NONE)
        {
            SHADEREVENT_LOADED(mFileBuffer.GetBuffer(), mFileBuffer.GetFileSize());
            return true;
        }
        else
        {
            SHADEREVENT_IO_ERROR(ioError, path, "Io error");
        }
    }
    else
    {
        SHADEREVENT_WRONG_FILE_FORMAT(path, "wrong file format!");
    }
    return false;
}

Pegasus::Graph::NodeData * Pegasus::Shader::ShaderStage::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "Shader Node Data", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Graph::NodeData(GetNodeDataAllocator());
}

void Pegasus::Shader::ShaderStage::GenerateData()
{
    PG_ASSERT(GetData() != nullptr);
    mFactory->GenerateShaderGpuData(&(*this), &(*GetData()));
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
