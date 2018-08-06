/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12GpuProgram.cpp
//! \author Kleber Garcia
//! \date   August 5th 2018
//! \brief  The so feared encapsulation of PSOs, shader state, shader source.
#include <Pegasus/Core/Io.h>
#include "Dx12GpuProgram.h"
#include "Dx12Defs.h"
#include <D3DCompiler.h>
#include <atlbase.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#pragma comment(lib, "d3dcompiler")

namespace Pegasus
{
namespace Render
{

enum Dx12PipelineType
{
    Dx12_Vertex,
    Dx12_Pixel,
    Dx12_PipelineMax = Dx12_Pixel,
    Dx12_Unknown
};

const char* pipelineToModel(Dx12PipelineType type)
{
    switch(type)
    {
    case Dx12_Pixel:
        return "ps_5_1";
    case Dx12_Vertex:
        return "vs_5_1";
    }

    return "";
}

D3D12_SHADER_VISIBILITY pipelineToVis(Dx12PipelineType type)
{
    switch(type)
    {
    case Dx12_Pixel:
        return D3D12_SHADER_VISIBILITY_PIXEL;
    case Dx12_Vertex:
        return D3D12_SHADER_VISIBILITY_VERTEX;
    }

    return D3D12_SHADER_VISIBILITY_ALL;
}

struct Dx12ShaderBlob
{
    Dx12PipelineType pipelineType;
    CComPtr<ID3DBlob> byteCode;
    CComPtr<ID3D12ShaderReflection> reflectionInfo;
};

struct Dx12ShaderSrvParam
{
    D3D12_SHADER_INPUT_BIND_DESC desc;
    D3D12_SHADER_VISIBILITY visMask;
};

static bool isSrv(D3D_SHADER_INPUT_TYPE t)
{
    return t == D3D_SIT_TBUFFER || t == D3D_SIT_TEXTURE || t == D3D_SIT_STRUCTURED || t == D3D_SIT_BYTEADDRESS;
}

static bool isCbv(D3D_SHADER_INPUT_TYPE t)
{
    return t == D3D_SIT_CBUFFER;
}

static bool isUav(D3D_SHADER_INPUT_TYPE t)
{
    return t == D3D_SIT_UAV_RWTYPED || t == D3D_SIT_UAV_RWSTRUCTURED || t == D3D_SIT_UAV_RWBYTEADDRESS
    ||  t == D3D_SIT_UAV_APPEND_STRUCTURED || t == D3D_SIT_UAV_CONSUME_STRUCTURED || t == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER;
}

static bool isSampler(D3D_SHADER_INPUT_TYPE t)
{
		return t == D3D_SIT_SAMPLER;
}

typedef std::vector<Dx12ShaderSrvParam> Dx12ShaderBindParamList;
typedef std::vector<Dx12ShaderBlob> Dx12ShaderBlobList;

struct Dx12GpuProgramParams
{
    Dx12ShaderBindParamList srvs; 
    Dx12ShaderBindParamList uavs; 
    Dx12ShaderBindParamList cbvs; 
    Dx12ShaderBindParamList samplers; 
};

void gatherSortedGpuParameters(const Dx12ShaderBlobList& inputShaders, Dx12GpuProgramParams& outParams)
{
    typedef std::map<std::string, Dx12ShaderSrvParam> ParamSet;
    ParamSet srvParamSet;
	ParamSet uavParamSet;
	ParamSet cbvParamSet;
	ParamSet samplerParamSet;

    for (const auto& shaderBlob : inputShaders)
    {
        D3D12_SHADER_DESC outDesc;
        DX_VALID_DECLARE(shaderBlob.reflectionInfo->GetDesc(&outDesc));
        for (UINT resIdx =  0; resIdx < outDesc.BoundResources; ++resIdx)
        {
            D3D12_SHADER_INPUT_BIND_DESC outResDesc;
            DX_VALID(shaderBlob.reflectionInfo->GetResourceBindingDesc(resIdx, &outResDesc));
            std::string resName = outResDesc.Name;
			ParamSet* targetParamSet = nullptr;
            if (isSrv(outResDesc.Type))
            {
                targetParamSet = &srvParamSet;
            }
            else if (isUav(outResDesc.Type))
            {
                targetParamSet = &uavParamSet;
            }
            else if (isCbv(outResDesc.Type))
            {
                targetParamSet = &cbvParamSet;
            }
            else if (isSampler(outResDesc.Type))
            {
                targetParamSet = &samplerParamSet;
            }
            else
            {
                PG_FAILSTR("Cannot recognize input type of %s.", outResDesc.Name);
            }

			if (outResDesc.uFlags & D3D_SIF_UNUSED)
				continue;

            auto it = targetParamSet->find(resName);
            if (it == targetParamSet->end())
            {
				(*targetParamSet)[resName] = Dx12ShaderSrvParam { outResDesc, pipelineToVis(shaderBlob.pipelineType) };
            }
            else
            {
                D3D12_SHADER_VISIBILITY newVis = pipelineToVis(shaderBlob.pipelineType);
                if (it->second.visMask != D3D12_SHADER_VISIBILITY_ALL && it->second.visMask != newVis)
                {
                    it->second.visMask = D3D12_SHADER_VISIBILITY_ALL;
                }
                PG_ASSERT(it->second.desc.BindPoint == outResDesc.BindPoint && it->second.desc.BindCount == outResDesc.BindCount);
            }
        }
    }

    auto sorterFn = [](const Dx12ShaderSrvParam& a, const Dx12ShaderSrvParam& b)
    {
        return a.desc.BindPoint < b.desc.BindPoint;
    };

    auto populateList = [&](const ParamSet& inputMap, Dx12ShaderBindParamList& outList)
    {
        for (auto& keyValPair : inputMap)
        {
            outList.push_back(keyValPair.second);
        }
        std::sort(outList.begin(), outList.end(), sorterFn);
    };

    populateList(srvParamSet, outParams.srvs);
    populateList(uavParamSet, outParams.uavs);
    populateList(cbvParamSet, outParams.cbvs);
    populateList(samplerParamSet, outParams.samplers);
}

Dx12GpuProgram::Dx12GpuProgram(Dx12Device* device)
    : mDevice(device), mParams(nullptr)
{
}

Dx12GpuProgram::~Dx12GpuProgram()
{
    if (mParams != nullptr)
    {
        delete mParams;
    }
}

void Dx12GpuProgram::Compile()
{
    auto* ioManager = mDevice->GetIOMgr();
    const char* testShaderPath = "Shaders/hlsl/Dx12Test.hlsl";
    Io::FileBuffer shaderBuffer;
    Io::IoError err = ioManager->OpenFileToBuffer(testShaderPath, shaderBuffer, true, mDevice->GetAllocator());
    PG_ASSERTSTR(err == Io::ERR_NONE, "Error opening test shader \"%s\". Error Code: %d", testShaderPath, err);

    auto compileShader = [](Dx12ShaderBlob& blob, const char* src, int srcSize, const char* mainFn, Dx12PipelineType pipelineType)
    {
        ID3DBlob* errBlob = nullptr;
        HRESULT result = D3DCompile(
            (LPCVOID)src,
            (SIZE_T)srcSize,
            "", //No name
            NULL, //no defines
            NULL, //No include handler
            mainFn,
            pipelineToModel(pipelineType),
            D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS,
            0,
            &blob.byteCode,
            &errBlob
        );
        if (result != S_OK)
        {
            PG_FAILSTR("Failed compiling test shader: %s", errBlob->GetBufferPointer());
        }
        else
        {
            result = D3DReflect(blob.byteCode->GetBufferPointer(), blob.byteCode->GetBufferSize(), __uuidof(ID3D12ShaderReflection), reinterpret_cast<void**>(&blob.reflectionInfo));
            blob.pipelineType = pipelineType;
            PG_ASSERTSTR(result == S_OK, "Failed generating reflection info for shader.");
        }

        if (errBlob != nullptr)
	    	errBlob->Release();
    };

    Dx12ShaderBlob psBlob; 
    Dx12ShaderBlob vsBlob; 
    compileShader(psBlob, shaderBuffer.GetBuffer(), shaderBuffer.GetFileSize(), "psMain", Dx12_Pixel);
    compileShader(vsBlob, shaderBuffer.GetBuffer(), shaderBuffer.GetFileSize(), "vsMain", Dx12_Vertex);
    
    Dx12ShaderBlobList shaderLinkages = { psBlob, vsBlob };
    mParams = new Dx12GpuProgramParams();
    gatherSortedGpuParameters(shaderLinkages, *mParams);
}

}
}
