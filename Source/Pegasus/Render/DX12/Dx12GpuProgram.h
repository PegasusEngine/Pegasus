/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12GpuProgram.h
//! \author Kleber Garcia
//! \date   August 5th 2018
//! \brief  The so feared encapsulation of PSOs, shader state, shader source.

#pragma once

#include "Dx12Device.h"
#include "Dx12Defs.h"
#include <Pegasus/Core/RefCounted.h>
#include <Pegasus/Core/Ref.h>
#include <Pegasus/Core/Shared/EventDefs.h>
#include <Pegasus/Core/Shared/CompilerEvents.h>
#include <vector>
#include <d3d12.h>
#include <atlbase.h>
#include <string>

namespace Pegasus
{
namespace Render
{

struct Dx12ProgramDesc
{
    std::string filename;
    std::string mainNames[Dx12_PipelineMax];
    Dx12ProgramDesc()
    {
        filename = "";
        for (unsigned i = 0; i < Dx12_PipelineMax; ++i) mainNames[i] = "";
    }
};

struct Dx12GpuProgramParams;
struct Dx12GpuProgramData;

class Dx12GpuProgram : public Core::RefCounted
{
    PEGASUS_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

public:
    Dx12GpuProgram(Dx12Device* device);
    virtual ~Dx12GpuProgram();

    ID3D12RootSignature* GetRootSignature() const;
    bool Compile(const Dx12ProgramDesc& programDesc);
    bool IsValid() const;
    const Dx12ProgramDesc& GetDesc() const { return mDesc; }
    CComPtr<ID3DBlob> GetShaderByteCode(Dx12PipelineType type);
    unsigned int GetShaderByteCodeCounts() const;
    CComPtr<ID3DBlob> GetShaderByteCodeByIndex(unsigned int index, Dx12PipelineType& outType);
    bool SpaceToTableId(UINT space, Dx12ResType resType, UINT& outTableId) const;

private:

    inline void InvalidateData() {}

    void fillInInternalData();
    bool createRootSignature();
    Dx12ProgramDesc mDesc;
    Dx12Device* mDevice;
    Dx12GpuProgramParams* mParams;
    Dx12GpuProgramData* mData;
};

typedef Pegasus::Core::Ref<Dx12GpuProgram> Dx12GpuProgramRef;

}
}
