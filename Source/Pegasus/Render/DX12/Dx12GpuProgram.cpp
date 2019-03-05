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
    case Dx12_Hull:            
        return D3D12_SHADER_VISIBILITY_HULL;
    case Dx12_Domain:
        return D3D12_SHADER_VISIBILITY_DOMAIN;
    case Dx12_Geometry:
        return D3D12_SHADER_VISIBILITY_GEOMETRY;
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

struct Dx12GpuProgramParams
{
    //resource table layout computed from desc and reflection data
    std::vector<D3D12_ROOT_PARAMETER> tables;
    
    //supporting memory for auto-generated descriptor ranges.
    std::vector<D3D12_DESCRIPTOR_RANGE*> supportingRanges;

    //Lookup used to find the right table id
    std::vector<int> lookup[Dx12_ResTypeCount];

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
    CComPtr<ID3DBlob> rootSignatureBlob;
    CComPtr<ID3D12RootSignature> rootSignature;
};

CComPtr<ID3DBlob> Dx12GpuProgram::GetShaderByteCode(Dx12PipelineType type)
{
    if (mData)
    {
        for (Dx12ShaderBlob& blob : mData->blobs)
        {
            if (blob.pipelineType == type)
                return blob.byteCode;
        }
    }

    return nullptr;
}

unsigned int Dx12GpuProgram::GetShaderByteCodeCounts() const
{
    return mData ? (unsigned int) mData->blobs.size() : 0u;
}

CComPtr<ID3DBlob> Dx12GpuProgram::GetShaderByteCodeByIndex(unsigned int index, Dx12PipelineType& outType)
{
    PG_ASSERT(mData);
    PG_ASSERT(index < (unsigned int)mData->blobs.size());
    outType = mData->blobs[index].pipelineType;
    return mData->blobs[index].byteCode;
}

void Dx12GpuProgram::fillInInternalData()
{
    const Dx12ShaderBlobList& inputShaders = mData->blobs;
    typedef std::map<std::string, Dx12ShaderParam> StrToParamMap;
    typedef std::map<UINT, StrToParamMap> ParamSpaceSet;
    ParamSpaceSet paramSpaceSetsTypes[Dx12_ResTypeCount];

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

            
#if PEGASUS_DEBUG
            if (resType == Dx12_ResInvalid)
                PG_FAILSTR("Cannot recognize input type of %s.", outResDesc.Name);
#endif

            ParamSpaceSet& targetParamSpaceSet = paramSpaceSetsTypes[resType];

            if (outResDesc.uFlags & D3D_SIF_UNUSED)
                continue;

            auto itSpace = targetParamSpaceSet.find(outResDesc.Space);
            if (itSpace == targetParamSpaceSet.end())
            {
                targetParamSpaceSet[outResDesc.Space] = StrToParamMap();
                itSpace = targetParamSpaceSet.find(outResDesc.Space);
            }

            auto it = itSpace->second.find(resName);
            if (it == itSpace->second.end())
            {
                itSpace->second[resName] = Dx12ShaderParam { resType, outResDesc, pipelineToVis(shaderBlob.pipelineType) };
            }
            else
            {
                D3D12_SHADER_VISIBILITY newVis = pipelineToVis(shaderBlob.pipelineType);
                if (it->second.visibility != D3D12_SHADER_VISIBILITY_ALL && it->second.visibility != newVis)
                {
                    it->second.visibility = D3D12_SHADER_VISIBILITY_ALL;
                }
                PG_ASSERT(it->second.desc.BindPoint == outResDesc.BindPoint && it->second.desc.BindCount == outResDesc.BindCount);
                PG_ASSERT(it->second.desc.Space == outResDesc.Space);
            }
        }
    }

    auto shaderParamsToD3d12DescriptorRanges = [&](UINT space, const Dx12ShaderBindParamList& shaderParams, D3D12_SHADER_VISIBILITY& rangesVis)
    {
            UINT maxRegister = 0;
            rangesVis = shaderParams[0].visibility;
            for (const auto& shaderParam : shaderParams)
            {
                bool isBindless = shaderParam.desc.BindCount == 0;
                UINT currRegister = isBindless ? UINT_MAX : shaderParam.desc.BindPoint + shaderParam.desc.BindCount;
                maxRegister = currRegister > maxRegister ? currRegister : maxRegister;
                if (rangesVis != shaderParam.visibility)
                    rangesVis = D3D12_SHADER_VISIBILITY_ALL;
            }

            return D3D12_DESCRIPTOR_RANGE {
                toD3dRangeType(shaderParams[0].resType),
                maxRegister, 0u, space, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
            };
    };

    auto createTable = [&](D3D12_DESCRIPTOR_RANGE& range, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER rootTableParameter; 
        rootTableParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootTableParameter.ShaderVisibility = visibility;
        D3D12_ROOT_DESCRIPTOR_TABLE& descTable = rootTableParameter.DescriptorTable;

        D3D12_DESCRIPTOR_RANGE* supportMem = new D3D12_DESCRIPTOR_RANGE;
        Utils::Memcpy(supportMem, &range, sizeof(D3D12_DESCRIPTOR_RANGE));
        descTable.pDescriptorRanges = supportMem;
        descTable.NumDescriptorRanges = 1;
        mParams->supportingRanges.push_back(supportMem);
        mParams->tables.push_back(rootTableParameter);
    };

    PG_ASSERT(mParams->supportingRanges.empty());
    PG_ASSERT(mParams->tables.empty());

    UINT resourceType = 0;
    for (const ParamSpaceSet& paramTypeBag : paramSpaceSetsTypes)
    {        
        for (const auto& paramSpace : paramTypeBag)
        {
			UINT space = paramSpace.first;
			const auto& strToParamMap = paramSpace.second;

            Dx12ShaderBindParamList bindList;
            for (auto paramStrValPair : strToParamMap)
                bindList.emplace_back(paramStrValPair.second);

            if (bindList.empty())
                continue;
            
            D3D12_SHADER_VISIBILITY rangeVis;
			D3D12_DESCRIPTOR_RANGE range = shaderParamsToD3d12DescriptorRanges(space, bindList, rangeVis);
			int tableId = (int)mParams->tables.size();
            createTable(range, rangeVis);
            auto& spaceLookupList = mParams->lookup[resourceType];
            if (space >= (int)spaceLookupList.size())
                spaceLookupList.resize(space + 1, -1);
            spaceLookupList[space] = tableId;
        }
        resourceType++;
    }
}

bool Dx12GpuProgram::createRootSignature()
{
    CComPtr<ID3DBlob> errorBlob;
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.NumParameters = (unsigned)mParams->tables.size();
    rootSignatureDesc.pParameters = mParams->tables.data();
    rootSignatureDesc.NumStaticSamplers = 0u;
    rootSignatureDesc.pStaticSamplers = nullptr;
    HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &mData->rootSignatureBlob, &errorBlob);
    if (result != S_OK)
    {
        mData->rootSignatureBlob = nullptr;

        PEGASUS_EVENT_DISPATCH(
            this, Core::CompilerEvents::CompilationEvent,
            false, (const char*)errorBlob->GetBufferPointer()
        );
        return false;
    }
    else
    {
        result = mDevice->GetD3D()->CreateRootSignature(0u, mData->rootSignatureBlob->GetBufferPointer(), mData->rootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(&mData->rootSignature));
        PEGASUS_EVENT_DISPATCH(
            this, Core::CompilerEvents::CompilationEvent,
            result == S_OK, result == S_OK ? "Success" : "Error creating root signature"
        );
        return true;
    }
}

ID3D12RootSignature* Dx12GpuProgram::GetRootSignature() const
{
    PG_ASSERT(mData != nullptr);
    return &(*mData->rootSignature);
}

bool Dx12GpuProgram::SpaceToTableId(UINT space, Dx12ResType resType, UINT& outTableId) const
{
    if (mParams == nullptr)
        return false;
    
    PG_ASSERT(resType < Dx12_ResTypeCount);
    if (space >= mParams->lookup[resType].size())
        return false;

    outTableId = mParams->lookup[resType][space];
    return true;
}

Dx12GpuProgram::Dx12GpuProgram(Dx12Device* device)
    : RefCounted(device->GetAllocator()), mDevice(device), mParams(nullptr), mData(nullptr)
{
#if PEGASUS_USE_EVENTS
    SetEventUserData(nullptr);
    SetEventListener(nullptr);
#endif
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

bool Dx12GpuProgram::IsValid() const
{
    return mData != nullptr && mData->rootSignature != nullptr;
}

bool Dx12GpuProgram::Compile(const Dx12ProgramDesc& desc)
{
    mDesc = desc;

    auto* ioManager = mDevice->GetIOMgr();
    Io::FileBuffer shaderBuffer;
    Io::IoError err = ioManager->OpenFileToBuffer(desc.filename.c_str(), shaderBuffer, true, mDevice->GetAllocator());
    PG_ASSERTSTR(err == Io::ERR_NONE, "Error opening test shader \"%s\". Error Code: %d", desc.filename.c_str(), err);

    auto compileShader = [&](Dx12ShaderBlob& blob, const char* src, int srcSize, const char* mainFn, Dx12PipelineType pipelineType)
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
            D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES,
            0,
            &blob.byteCode,
            &errBlob
        );
        if (result != S_OK)
        {
            PG_FAILSTR("Failed compiling test shader: %s", errBlob->GetBufferPointer());
            PEGASUS_EVENT_DISPATCH(
                this, Core::CompilerEvents::CompilationNotification,
                Core::CompilerEvents::CompilationNotification::COMPILATION_ERROR,
                "", 0u, ""
            );
            success = false;
        }
        else
        {
            result = D3DReflect(blob.byteCode->GetBufferPointer(), blob.byteCode->GetBufferSize(), __uuidof(ID3D12ShaderReflection), reinterpret_cast<void**>(&blob.reflectionInfo));
            blob.pipelineType = pipelineType;
            if (result != S_OK)
            {
                PEGASUS_EVENT_DISPATCH(
                    this, Core::CompilerEvents::CompilationNotification,
                    Core::CompilerEvents::CompilationNotification::COMPILATION_ERROR,
                    "", 0u, "Failed generating reflection for shader"
                );
            }
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
        if (!desc.mainNames[pipelineIdx].empty())
        {
            Dx12ShaderBlob shaderblob; 
            bool result = compileShader(
                shaderblob, shaderBuffer.GetBuffer(),
                shaderBuffer.GetFileSize(), desc.mainNames[pipelineIdx].c_str(),
                (Dx12PipelineType)pipelineIdx);
            if (result)
            {
                mData->blobs.push_back(shaderblob);
            }
            else
            {
                return false;
            }
        }
    }

    fillInInternalData();
    return createRootSignature();
}

}
}
