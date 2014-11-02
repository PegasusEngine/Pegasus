/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXShaderFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 shader factory implementation

#if PEGASUS_GAPI_DX


#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")


#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Graph/NodeData.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "../Source/Pegasus/Render/DX11/DXGpuDataDefs.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXDevice.h"

using namespace Pegasus::Core;

static int gNextProgramGuid = 1;

//! internal definition of shader factory API
class DXShaderFactory : public Pegasus::Shader::IShaderFactory
{
public:
    DXShaderFactory(){}
    virtual ~DXShaderFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator); 
    
    virtual void GenerateShaderGPUData(Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData);

    virtual void DestroyShaderGPUData (Pegasus::Graph::NodeData * nodeData);

    virtual void GenerateProgramGPUData(Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData);

    virtual void DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData);

private:
    Pegasus::Render::DXShaderGPUData* GetOrCreateShaderGpuData(Pegasus::Graph::NodeData* nodeData);
    Pegasus::Render::DXProgramGPUData* GetOrCreateProgramGpuData(Pegasus::Graph::NodeData* nodeData);
    void PopulateReflectionData(Pegasus::Render::DXProgramGPUData* programData, const CComPtr<ID3D11ShaderReflection>& reflectionInfo, Pegasus::Shader::ShaderType shaderType);
    Pegasus::Alloc::IAllocator * mAllocator;
};

//! initializes the factory
void DXShaderFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
    mAllocator = allocator;
}

//! allocates lazily or returns an existent shader gpu data
Pegasus::Render::DXShaderGPUData* DXShaderFactory::GetOrCreateShaderGpuData(Pegasus::Graph::NodeData * data)
{
    Pegasus::Render::DXShaderGPUData* shaderGPUData = nullptr;
    Pegasus::Graph::NodeGPUData* gpuData = data->GetNodeGPUData();
    if (gpuData == nullptr)
    {
        shaderGPUData = PG_NEW(
            mAllocator,
            -1,
            "DX Shader GPU Data",
            Pegasus::Alloc::PG_MEM_TEMP
        )
        Pegasus::Render::DXShaderGPUData();
        shaderGPUData->mType = Pegasus::Shader::SHADER_STAGE_INVALID;
        shaderGPUData->mDeviceChild = nullptr;
        shaderGPUData->mReflectionInfo = nullptr;
        shaderGPUData->mBlob = nullptr;
        data->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(shaderGPUData));
    }
    else
    {
        shaderGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXShaderGPUData, gpuData);
    }

    return shaderGPUData;
}

//! processes an error log from OpenGL compilation
static int ProcessErrorLog(Pegasus::Shader::ShaderStage * shaderNode, const char * log)
{
    int errorCount = 0;
#if PEGASUS_USE_GRAPH_EVENTS 
    //parsing log to extract line & column
    const char  * s = log;

    //WARNING, I have optimized the crap out of the following code, so parsing might be messy.

    while (*s)
    {
        int line = 0;
        bool foundNumber = false;
        
        //Skip any non numerical character
        while ( *s != '\0' && (*s < '0' || *s > '9')) ++s;

        //Parse Line number
        for (; *s != '\0' && *s >= '0' && *s <= '9'; ++s)
        {
            foundNumber = true;
            line = 10*line + (*s - '0');
        }

        //Skip any character that is not ":"
        while ( *s != ':' && *s) ++s;

        if (*s && foundNumber)
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
                CompilerEvents::CompilationNotification,
                // Shader Event specific arguments
                CompilerEvents::CompilationNotification::COMPILATION_ERROR,
                line,
                descriptionError
            );
        }

    }
#endif
    return errorCount;
}

//! convinience function to delete a shader gpu data
static void DetachUnionShaders(Pegasus::Render::DXShaderGPUData* shaderGPUData)
{
    if (shaderGPUData->mDeviceChild != nullptr)
    {
        switch(shaderGPUData->mType)
        {
        case Pegasus::Shader::FRAGMENT:
            shaderGPUData->mPixel->Release();
            break;
        case Pegasus::Shader::VERTEX:
            shaderGPUData->mVertex->Release();
            break;
        case Pegasus::Shader::TESSELATION_CONTROL:
            shaderGPUData->mHull->Release();
            break;
        case Pegasus::Shader::TESSELATION_EVALUATION:
            shaderGPUData->mDomain->Release();
            break;
        case Pegasus::Shader::GEOMETRY:
            shaderGPUData->mGeometry->Release();
            break;
        case Pegasus::Shader::COMPUTE:
            shaderGPUData->mCompute->Release();
            break;
        };
        shaderGPUData->mDeviceChild = nullptr;
    }
}

//! generator function that compiles a shader
void DXShaderFactory::GenerateShaderGPUData(Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    const char * shaderSource = nullptr;
    int shaderSourceSize = 0;
    shaderNode->GetSource(&shaderSource, shaderSourceSize);

    static const char * sTargets[Pegasus::Shader::SHADER_STAGES_COUNT] = {
        "vs_5_0", // vertex shader stage
        "ps_5_0", // pixel shader stage
        "hs_5_0", // tesselation control / hull stage
        "ds_5_0"  // tesselation evaluation / domain stage
        "gs_5_0", // geometry shader stage
        "cs_5_0", // compute shader stage. Must be by itself if activated
    };

    Pegasus::Render::DXShaderGPUData* shaderGPUData = GetOrCreateShaderGpuData(nodeData);    
    DetachUnionShaders(shaderGPUData);
    shaderGPUData->mReflectionInfo = nullptr;

    shaderGPUData->mType = shaderNode->GetStageType();

    if (shaderGPUData->mType < Pegasus::Shader::SHADER_STAGES_COUNT)
    {
        
        shaderGPUData->mBlob = nullptr; //clear any previous blob reference
        CComPtr<ID3DBlob> errBlob;

        HRESULT result = D3DCompile(
            (LPCVOID)shaderSource,
            (SIZE_T)shaderSourceSize,
            "", //no name
            NULL, //no defines yet
            NULL, //no handler for includes
            "main", //entry function
            sTargets[shaderNode->GetStageType()],
            D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS,
            0, //no effect flags
            &shaderGPUData->mBlob,
            &errBlob
        );  
                    


        bool shaderCompiled = result == S_OK;;
        const char * logBuffer = nullptr;

        if (!shaderCompiled)
        {
            logBuffer = (const char *)errBlob->GetBufferPointer();
            ProcessErrorLog(shaderNode, logBuffer);
#if PEGASUS_ENABLE_LOG
#if PEGASUS_ENABLE_PROXIES
            PG_LOG('ERR_', "(%s)Shader Compilation Failure: %s", shaderNode->GetFileName(), logBuffer);
#else
            PG_LOG('ERR_', "Shader Compilation Failure: %s", logBuffer);
#endif
#endif
        }
        else
        {
            PG_ASSERT(shaderGPUData->mBlob != nullptr);
            HRESULT res = S_OK;
            switch(shaderGPUData->mType)
            {
            case Pegasus::Shader::FRAGMENT:
                res = device->CreatePixelShader(
                    shaderGPUData->mBlob->GetBufferPointer(), 
                    shaderGPUData->mBlob->GetBufferSize(), NULL, &shaderGPUData->mPixel);
                break;
            case Pegasus::Shader::VERTEX:
                res = device->CreateVertexShader(
                    shaderGPUData->mBlob->GetBufferPointer(), 
                    shaderGPUData->mBlob->GetBufferSize(), NULL, &shaderGPUData->mVertex);
                break;
            case Pegasus::Shader::TESSELATION_CONTROL:
                res = device->CreateHullShader(
                    shaderGPUData->mBlob->GetBufferPointer(), 
                    shaderGPUData->mBlob->GetBufferSize(), NULL, &shaderGPUData->mHull);
                break;
            case Pegasus::Shader::TESSELATION_EVALUATION:
                res = device->CreateDomainShader(
                    shaderGPUData->mBlob->GetBufferPointer(), 
                    shaderGPUData->mBlob->GetBufferSize(), NULL, &shaderGPUData->mDomain);
                break;
            case Pegasus::Shader::GEOMETRY:
                res = device->CreateGeometryShader(
                    shaderGPUData->mBlob->GetBufferPointer(), 
                    shaderGPUData->mBlob->GetBufferSize(), NULL, &shaderGPUData->mGeometry);
                break;
            case Pegasus::Shader::COMPUTE:
                res = device->CreateComputeShader(
                    shaderGPUData->mBlob->GetBufferPointer(), 
                    shaderGPUData->mBlob->GetBufferSize(), NULL, &shaderGPUData->mCompute);
                break;
            };
            PG_ASSERT(shaderGPUData->mDeviceChild != nullptr);
            PG_ASSERT(res == S_OK);

            res = D3DReflect (
                shaderGPUData->mBlob->GetBufferPointer(),
                shaderGPUData->mBlob->GetBufferSize(),
                IID_ID3D11ShaderReflection,
                (void**)&shaderGPUData->mReflectionInfo
            );
            PG_ASSERT(res == S_OK);
        } 

        GRAPH_EVENT_DISPATCH (
            shaderNode,
            CompilerEvents::CompilationEvent,
            // Event specific arguments
            shaderCompiled ? true : false, //compilation success status
            shaderCompiled ? "" : logBuffer
        );

    }
    nodeData->ValidateGPUData();
}

//!Destroy shader gpu data
void DXShaderFactory::DestroyShaderGPUData (Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Graph::NodeGPUData* nodeGpuData = nodeData->GetNodeGPUData();
    if (nodeGpuData != nullptr)
    {
        Pegasus::Render::DXShaderGPUData* shaderGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXShaderGPUData, nodeGpuData);
        DetachUnionShaders(shaderGPUData);
        shaderGPUData->mReflectionInfo = nullptr;
        PG_DELETE(mAllocator, shaderGPUData);
        nodeData->SetNodeGPUData(nullptr);
    }
}

//! Create or inject new program gpu
Pegasus::Render::DXProgramGPUData* DXShaderFactory::GetOrCreateProgramGpuData(Pegasus::Graph::NodeData* nodeData)
{
    Pegasus::Graph::NodeGPUData* nodeGPUData = nodeData->GetNodeGPUData();
    Pegasus::Render::DXProgramGPUData* programGPUData = nullptr;
    if (nodeGPUData == nullptr)
    {
        programGPUData = PG_NEW(
            mAllocator,
            -1,
            "DXProgramGPUData",
            Pegasus::Alloc::PG_MEM_PERM
        ) Pegasus::Render::DXProgramGPUData;

        nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(programGPUData));
        programGPUData->mProgramGuid = gNextProgramGuid++;
        programGPUData->mProgramVersion = 0;
        programGPUData->mProgramValid = false;
        programGPUData->mReflectionData = nullptr;
        programGPUData->mReflectionDataCount = 0;
        programGPUData->mReflectionDataCapacity = 0;
        
    }
    else
    {
        programGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGPUData);
    }
    return programGPUData;
}

//! Create or inject new program gpu
void DXShaderFactory::GenerateProgramGPUData(Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Render::DXProgramGPUData* programGPUData = GetOrCreateProgramGpuData(nodeData);

    //delete all shaders
    programGPUData->mPixel = nullptr;
    programGPUData->mVertex = nullptr;
    programGPUData->mDomain = nullptr;
    programGPUData->mHull = nullptr;
    programGPUData->mGeometry = nullptr;
    programGPUData->mCompute = nullptr;
    programGPUData->mInputLayoutBlob = nullptr;
    programGPUData->mProgramValid = false;
    programGPUData->mReflectionDataCount = 0; //empty the reflection data
    
    bool isProgramComplete = true; //assume true
    for (unsigned i = 0; i < programNode->GetNumInputs(); ++i)
    {
        Pegasus::Shader::ShaderStageRef shaderStage = programNode->FindShaderStageInput(i);
        if (shaderStage->GetStageType() != Pegasus::Shader::SHADER_STAGE_INVALID)
        {
            bool updated = false;
            Pegasus::Graph::NodeDataRef shaderNodeDataRef = shaderStage->GetUpdatedData(updated);
            Pegasus::Render::DXShaderGPUData * shaderStageGPUData = GetOrCreateShaderGpuData(&(*shaderNodeDataRef));
            switch(shaderStage->GetStageType())
            {
            case Pegasus::Shader::FRAGMENT:
                isProgramComplete = isProgramComplete && shaderStageGPUData->mPixel != nullptr;
                programGPUData->mPixel = shaderStageGPUData->mPixel;
                break;
            case Pegasus::Shader::VERTEX:
                isProgramComplete = isProgramComplete && shaderStageGPUData->mVertex != nullptr;
                programGPUData->mVertex = shaderStageGPUData->mVertex;
                PG_ASSERT(programGPUData->mInputLayoutBlob == nullptr);
                programGPUData->mInputLayoutBlob = shaderStageGPUData->mBlob;
                break;
            case Pegasus::Shader::TESSELATION_CONTROL:
                isProgramComplete = shaderStageGPUData->mHull != nullptr;
                programGPUData->mHull = shaderStageGPUData->mHull;
                break;
            case Pegasus::Shader::TESSELATION_EVALUATION:
                isProgramComplete = isProgramComplete && shaderStageGPUData->mDomain != nullptr;
                programGPUData->mDomain = shaderStageGPUData->mDomain;
                break;
            case Pegasus::Shader::GEOMETRY:
                isProgramComplete = isProgramComplete && shaderStageGPUData->mGeometry != nullptr;
                programGPUData->mGeometry = shaderStageGPUData->mGeometry;
                break;
            case Pegasus::Shader::COMPUTE:
                isProgramComplete = isProgramComplete && shaderStageGPUData->mCompute != nullptr;
                programGPUData->mCompute = shaderStageGPUData->mCompute;
                break;
            }
            PG_ASSERT(shaderStage->GetStageType() < Pegasus::Shader::SHADER_STAGES_COUNT);
			if (isProgramComplete)
			{
				PopulateReflectionData(programGPUData, shaderStageGPUData->mReflectionInfo, shaderStage->GetStageType());
			}
        }
    }
    
    if (
        !(programGPUData->mVertex != nullptr)
        &&  !(
                programGPUData->mVertex == nullptr &&
                programGPUData->mPixel == nullptr &&
                programGPUData->mHull  == nullptr &&
                programGPUData->mDomain == nullptr &&
                programGPUData->mGeometry == nullptr &&
                programGPUData->mCompute != nullptr 
            )
    )
    {
        GRAPH_EVENT_DISPATCH (
            programNode,
            CompilerEvents::LinkingEvent,
            // Event specific arguments:
            CompilerEvents::LinkingEvent::INCOMPLETE_STAGES_FAIL,
            "Incomplete shader stages"
        );
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "(%s)Program Link Failure, incomplete shader pipeline", programNode->GetName());
#else
        PG_LOG('ERR_', "Program Link Failure, incomplete shader pipeline");
#endif
    }
    else if (isProgramComplete)
    {
		programGPUData->mProgramValid = true;
        ++programGPUData->mProgramVersion;
        GRAPH_EVENT_DISPATCH (
            programNode,
            CompilerEvents::LinkingEvent,
            // Event specific arguments:
            CompilerEvents::LinkingEvent::LINKING_SUCCESS,
            ""
        );
    }
    else
    {
        GRAPH_EVENT_DISPATCH (
            programNode,
            CompilerEvents::LinkingEvent,
            // Event specific arguments:
            CompilerEvents::LinkingEvent::LINKING_FAIL,
            "Linking failed"
        );
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "(%s)Program Link Failure, compilation errors.", programNode->GetName());
#else
        PG_LOG('ERR_', "Program Link Failure, compilation errors.");
#endif
    }
    nodeData->ValidateGPUData();
}

void DXShaderFactory::PopulateReflectionData(Pegasus::Render::DXProgramGPUData* programData, const CComPtr<ID3D11ShaderReflection>& reflectionInfo, Pegasus::Shader::ShaderType shaderType)
{
    D3D11_SHADER_DESC shaderDesc;
    D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;
    VALID_DECLARE(reflectionInfo->GetDesc(&shaderDesc));
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
	    int size = 0;
        VALID(reflectionInfo->GetResourceBindingDesc(i, &inputBindDesc));
		if (inputBindDesc.Type == D3D_SIT_CBUFFER)
		{
			ID3D11ShaderReflectionConstantBuffer* cBuffer = reflectionInfo->GetConstantBufferByName(inputBindDesc.Name);
			D3D11_SHADER_BUFFER_DESC cbufferDesc;
			VALID(cBuffer->GetDesc(&cbufferDesc));
			size = cbufferDesc.Size;
		}


        //find semantic
        Pegasus::Render::DXProgramGPUData::UniformReflectionData* targetReflectionData = nullptr;
        for (int r = 0; r < programData->mReflectionDataCount; ++r)
        {
            Pegasus::Render::DXProgramGPUData::UniformReflectionData& candidate = programData->mReflectionData[r];
            if (!Pegasus::Utils::Strcmp(inputBindDesc.Name, candidate.mUniformName))
            {
                targetReflectionData = &candidate;
                break;
            }
        }

        //means is not found, so create a new piece of memory!
        if (targetReflectionData == nullptr)
        {
            if (programData->mReflectionDataCount >= programData->mReflectionDataCapacity)
            {
                int newCapacity = programData->mReflectionDataCapacity + UNIFORM_DATA_INCREMENT;
                Pegasus::Render::DXProgramGPUData::UniformReflectionData * newList = PG_NEW_ARRAY(
                    mAllocator,
                    -1,
                    "New Reflection Data List",
                    Pegasus::Alloc::PG_MEM_TEMP,
                    Pegasus::Render::DXProgramGPUData::UniformReflectionData,
                    newCapacity
                );

				if (programData->mReflectionData != nullptr)
				{
					Pegasus::Utils::Memcpy(newList, programData->mReflectionData, programData->mReflectionDataCapacity);
					PG_DELETE_ARRAY(mAllocator, programData->mReflectionData);
				}
                programData->mReflectionData = newList;
                programData->mReflectionDataCapacity = newCapacity;
            }
            targetReflectionData = &programData->mReflectionData[programData->mReflectionDataCount++];
            targetReflectionData->mUniformName[0] = '\0';
            targetReflectionData->mStageCount = 0;
            Pegasus::Utils::Strcat(targetReflectionData->mUniformName, inputBindDesc.Name);
        }

        PG_ASSERT(targetReflectionData->mStageCount < Pegasus::Shader::SHADER_STAGES_COUNT);
        Pegasus::Render::DXProgramGPUData::UniformReflectionData::StageBinding& stage = targetReflectionData->mStageBindings[targetReflectionData->mStageCount++];
        stage.mPipelineType = shaderType;
        stage.mBindPoint = inputBindDesc.BindPoint;
        stage.mBindCount = inputBindDesc.BindCount;
        stage.mType = inputBindDesc.Type;
        stage.mSize = size;
    }
}

//Dedestroy gpu data of program
void DXShaderFactory::DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData)
{
    Pegasus::Graph::NodeGPUData* nodeGPUData = nodeData->GetNodeGPUData();
    if (nodeGPUData != nullptr)
    {
        Pegasus::Render::DXProgramGPUData* programData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGPUData);

        //delete all Com shader references
        programData->mVertex   = nullptr;
        programData->mPixel    = nullptr;
        programData->mDomain   = nullptr;
        programData->mHull     = nullptr;
        programData->mGeometry = nullptr;
        programData->mCompute  = nullptr;

        if (programData->mReflectionData != nullptr)
        {
            PG_DELETE_ARRAY(mAllocator, programData->mReflectionData);
        }
        PG_DELETE(mAllocator, programData);
        nodeData->SetNodeGPUData(nullptr);

    }
}



namespace Pegasus {
namespace Render
{

//! The global shader factory
DXShaderFactory gShaderFactory;

//! return statically defined shader factory singleton
//! this function avoids using the heap, since the shader factory must be persistant and holds no state information
//! It instead, acts as a collection of APIs
Shader::IShaderFactory * GetRenderShaderFactory()
{
    return &gShaderFactory;
}

}
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
