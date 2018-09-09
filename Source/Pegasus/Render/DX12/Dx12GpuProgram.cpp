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
#include <Pegasus/Utils/Memcpy.h>
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

struct Dx12ShaderParam
{
    Dx12ResType resType;
    D3D12_SHADER_INPUT_BIND_DESC desc;
    D3D12_SHADER_VISIBILITY visibility;
};

D3D12_DESCRIPTOR_RANGE_TYPE toD3dRangeType(Dx12ResType resType)
{
    switch(resType)
    {
    case Dx12_ResSrv:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    case Dx12_ResCbv:
        return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    case Dx12_ResUav:
        return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    case Dx12_ResSampler:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    }
    
    return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
}

struct Dx12ParamRange
{
    D3D12_SHADER_VISIBILITY rangeVisibility = D3D12_SHADER_VISIBILITY_ALL;
    int idx = 0;
    int count = 0;
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

Dx12ResType getResType(D3D_SHADER_INPUT_TYPE t)
{
    if (isSrv(t))
    {
        return Dx12_ResSrv;
    }
    else if (isCbv(t))
    {
        return Dx12_ResCbv;
    }
    else if (isUav(t))
    {
        return Dx12_ResUav;
    }
    else if (isSampler(t))
    {
        return Dx12_ResSampler;
    }
    else return Dx12_ResInvalid;

}


typedef std::vector<Dx12ShaderParam> Dx12ShaderBindParamList;
typedef std::vector<Dx12ShaderBlob> Dx12ShaderBlobList;
typedef std::vector<Dx12ParamRange> Dx12ParamRangeList;
typedef std::map<int, int> Dx12BindPoint2ResIdx;

struct Dx12GpuProgramParams
{
    //reflection data
    Dx12ShaderBindParamList res[Dx12_ResCount]; 
    Dx12BindPoint2ResIdx b2Res[Dx12_ResCount];

    //resource table layout computed from desc and reflection data
    std::vector<D3D12_ROOT_PARAMETER> tables;
    
    //supporting memory for auto-generated descriptor ranges.
    std::vector<D3D12_DESCRIPTOR_RANGE*> supportingRanges;

    ~Dx12GpuProgramParams()
    {
        for (auto& ranges : supportingRanges)
        {
            delete ranges;
        }
    }
};

struct Dx12GpuProgramData
{
    Dx12ShaderBlobList blobs;
};

void Dx12GpuProgram::fillInReflectionData()
{
    const Dx12ShaderBlobList& inputShaders = mData->blobs;
    typedef std::map<std::string, Dx12ShaderParam> ParamSet;
    ParamSet paramSets[Dx12_ResCount];

    for (const auto& shaderBlob : inputShaders)
    {
        D3D12_SHADER_DESC outDesc;
        DX_VALID_DECLARE(shaderBlob.reflectionInfo->GetDesc(&outDesc));
        for (UINT resIdx =  0; resIdx < outDesc.BoundResources; ++resIdx)
        {
            D3D12_SHADER_INPUT_BIND_DESC outResDesc;
            DX_VALID(shaderBlob.reflectionInfo->GetResourceBindingDesc(resIdx, &outResDesc));
            std::string resName = outResDesc.Name;
            Dx12ResType resType = getResType(outResDesc.Type);

            
            if (resType == Dx12_ResInvalid)
                PG_FAILSTR("Cannot recognize input type of %s.", outResDesc.Name);

			ParamSet* targetParamSet = &paramSets[resType];

			if (outResDesc.uFlags & D3D_SIF_UNUSED)
				continue;

            auto it = targetParamSet->find(resName);
            if (it == targetParamSet->end())
            {
				(*targetParamSet)[resName] = Dx12ShaderParam { resType, outResDesc, pipelineToVis(shaderBlob.pipelineType) };
            }
            else
            {
                D3D12_SHADER_VISIBILITY newVis = pipelineToVis(shaderBlob.pipelineType);
                if (it->second.visibility != D3D12_SHADER_VISIBILITY_ALL && it->second.visibility != newVis)
                {
                    it->second.visibility = D3D12_SHADER_VISIBILITY_ALL;
                }
                PG_ASSERT(it->second.desc.BindPoint == outResDesc.BindPoint && it->second.desc.BindCount == outResDesc.BindCount);
            }
        }
    }

    auto sorterFn = [](const Dx12ShaderParam& a, const Dx12ShaderParam& b)
    {
        return a.desc.BindPoint < b.desc.BindPoint;
    };

    auto populateList = [&](const ParamSet& inputMap, Dx12ShaderBindParamList& outList, Dx12BindPoint2ResIdx& b2r)
    {
        for (auto& keyValPair : inputMap)
        {
            outList.push_back(keyValPair.second);
        }
        std::sort(outList.begin(), outList.end(), sorterFn);
        unsigned i = 0;
        for (auto& val : outList)
        {
            b2r[val.desc.BindPoint] = i++;
        }
    };

    for (unsigned i = 0; i < Dx12_ResCount; ++i)
        populateList(paramSets[i], mParams->res[i], mParams->b2Res[i]);
}

void Dx12GpuProgram::fillInResourceTableLayouts()
{
    auto userRangesToShaderParams = [&](const Dx12TableLayout& userLayout, std::vector<Dx12ShaderParam>& outShaderParams)
    {
        for (const auto& inputRange : userLayout.registerRanges)
        {
            PG_ASSERT(inputRange.tableType >= Dx12_ResTypeBegin && inputRange.tableType < Dx12_ResCount);
            if (inputRange.tableType >= Dx12_ResTypeBegin && inputRange.tableType < Dx12_ResCount)
            {
                for (unsigned regIdx = inputRange.baseRegister; regIdx < inputRange.baseRegister + inputRange.count; ++regIdx)
                {
                    auto& it = mParams->b2Res[inputRange.tableType].find(regIdx);
                    if (it != mParams->b2Res[inputRange.tableType].end())
                    {
                        outShaderParams.push_back(mParams->res[inputRange.tableType][it->second]);
                    }
                }
            }
        }
    };

    auto initRange = [](const Dx12ShaderParam* param)
    {
        return D3D12_DESCRIPTOR_RANGE {
            toD3dRangeType(param->resType),
            param->desc.BindCount, param->desc.BindPoint,
            0u, /*No support for reg spaces for now*/
            D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        };
	};
    
    auto shaderParamsToD3d12DescriptorRanges = [&](const Dx12ShaderBindParamList& shaderParams, std::vector<D3D12_DESCRIPTOR_RANGE>& ranges, D3D12_SHADER_VISIBILITY& rangesVis)
    {
        if (!shaderParams.empty())
        {
            const Dx12ShaderParam* lastParam = &shaderParams[0];
            rangesVis = lastParam->visibility;

            bool pendingRange = true;
            D3D12_DESCRIPTOR_RANGE currRange = initRange(lastParam);

            unsigned expectedRegister = currRange.BaseShaderRegister + currRange.NumDescriptors;

            for (unsigned i = 1; i < shaderParams.size(); ++i)
            {
                pendingRange = true;
                const Dx12ShaderParam* newParam = &shaderParams[i]; 
                if (newParam->desc.BindPoint != expectedRegister || newParam->resType != lastParam->resType)
                {
                    pendingRange = false;
                    ranges.push_back(currRange);
                    currRange = initRange(newParam);
                }
                else
                {
                    currRange.NumDescriptors += newParam->desc.BindCount;
                }

                if (rangesVis != newParam->visibility)
                    rangesVis = D3D12_SHADER_VISIBILITY_ALL;

                lastParam = newParam;
            }
            
            if (pendingRange)
                ranges.push_back(currRange);
        }
    };

    auto createTable = [&](const std::vector<D3D12_DESCRIPTOR_RANGE>& ranges, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER rootTableParameter; 
        D3D12_ROOT_DESCRIPTOR_TABLE& descTable = rootTableParameter.DescriptorTable;
        D3D12_DESCRIPTOR_RANGE* supportMem = new D3D12_DESCRIPTOR_RANGE[ranges.size()];
        Utils::Memcpy(supportMem, ranges.data(), (unsigned)ranges.size() * sizeof(D3D12_DESCRIPTOR_RANGE));
        descTable.pDescriptorRanges = supportMem;
        descTable.NumDescriptorRanges = (unsigned)ranges.size();
		rootTableParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootTableParameter.ShaderVisibility = visibility;
        mParams->supportingRanges.push_back(supportMem);
        mParams->tables.push_back(rootTableParameter);
    };

    const std::vector<Dx12TableLayout>& tableLayouts = mDesc.tableLayouts;
    if (tableLayouts.empty())
    {
        //if the user did not defined anything, we create a massive table. 

        D3D12_SHADER_VISIBILITY rangesVis = D3D12_SHADER_VISIBILITY_ALL;
        std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
        for (unsigned resType = 0; resType < Dx12_ResCount; ++resType)
            shaderParamsToD3d12DescriptorRanges(mParams->res[resType], ranges, rangesVis);

        createTable(ranges, rangesVis);
        mAutoTableLayout = true;
    }
    else
    {
        //Lets use dx12 and optimally group params like the user wants!
        for (const Dx12TableLayout& layout : tableLayouts)
        {
            std::vector<Dx12ShaderParam> shaderParams;
            userRangesToShaderParams(layout, shaderParams);
            
            D3D12_SHADER_VISIBILITY rangesVis = D3D12_SHADER_VISIBILITY_ALL;
            std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
            shaderParamsToD3d12DescriptorRanges(shaderParams, ranges, rangesVis);

            createTable(ranges, rangesVis);
        }
        mAutoTableLayout = false;
    }
}

Dx12GpuProgram::Dx12GpuProgram(Dx12Device* device)
    : mDevice(device), mParams(nullptr), mData(nullptr), mAutoTableLayout(false)
{
}

Dx12GpuProgram::~Dx12GpuProgram()
{
    if (mParams != nullptr)
    {
        delete mParams;
    }

    if (mData != nullptr)
    {
        delete mData;
    }
}

void Dx12GpuProgram::Compile(const Dx12ProgramDesc& desc)
{
    mDesc = desc;

    auto* ioManager = mDevice->GetIOMgr();
    const char* testShaderPath = "Shaders/hlsl/Dx12Test.hlsl";
    Io::FileBuffer shaderBuffer;
    Io::IoError err = ioManager->OpenFileToBuffer(testShaderPath, shaderBuffer, true, mDevice->GetAllocator());
    PG_ASSERTSTR(err == Io::ERR_NONE, "Error opening test shader \"%s\". Error Code: %d", testShaderPath, err);

    auto compileShader = [](Dx12ShaderBlob& blob, const char* src, int srcSize, const char* mainFn, Dx12PipelineType pipelineType)
    {
        ID3DBlob* errBlob = nullptr;

        bool success = true;

        HRESULT result = D3DCompile(
            (LPCVOID)src,
            (SIZE_T)srcSize,
            "", //No name
            NULL, //no defines
            NULL, //No include handler
            mainFn,
            pipelineToModel(pipelineType),
            D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES,
            0,
            &blob.byteCode,
            &errBlob
        );
        if (result != S_OK)
        {
            PG_FAILSTR("Failed compiling test shader: %s", errBlob->GetBufferPointer());
            success = false;
        }
        else
        {
            result = D3DReflect(blob.byteCode->GetBufferPointer(), blob.byteCode->GetBufferSize(), __uuidof(ID3D12ShaderReflection), reinterpret_cast<void**>(&blob.reflectionInfo));
            blob.pipelineType = pipelineType;
            PG_ASSERTSTR(result == S_OK, "Failed generating reflection info for shader.");
        }

        if (errBlob != nullptr)
	    	errBlob->Release();
    
        return success;
    };

    if (mParams != nullptr)
    {
        delete mParams;
    }
    if (mData != nullptr)
    {
        delete mData;
    }

    mParams = new Dx12GpuProgramParams();
    mData = new Dx12GpuProgramData();

    for (unsigned pipelineIdx = 0; pipelineIdx < Dx12_PipelineMax; ++pipelineIdx)
    {
        if (desc.mainNames[pipelineIdx] != nullptr)
        {
            Dx12ShaderBlob shaderblob; 
            bool result = compileShader(
                shaderblob, shaderBuffer.GetBuffer(),
                shaderBuffer.GetFileSize(), desc.mainNames[pipelineIdx],
                (Dx12PipelineType)pipelineIdx);
            if (result)
            {
                mData->blobs.push_back(shaderblob);
            }
        }
    }

    fillInReflectionData();
    fillInResourceTableLayouts();
}

}
}
