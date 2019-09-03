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

struct Dx12GpuProgramParams;
struct Dx12GpuProgramData;

class Dx12GpuProgram
{
public:
    PEGASUS_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

    Dx12GpuProgram(Dx12Device* device);
    virtual ~Dx12GpuProgram();

    bool Compile(const GpuPipelineConfig& desc);
    void ClearEventData();

    ID3D12RootSignature* GetRootSignature() const;
    CComPtr<ID3DBlob> GetShaderByteCode(Dx12PipelineType type);
    unsigned int GetShaderByteCodeCounts() const;
    CComPtr<ID3DBlob> GetShaderByteCodeByIndex(unsigned int index, Dx12PipelineType& outType);
    bool SpaceToTableId(UINT space, Dx12ResType resType, UINT& outTableId) const;

private:
    void InvalidateData() {}
    void fillInInternalData();
    bool createRootSignature();
    Dx12Device* mDevice;
    Dx12GpuProgramParams* mParams;
    Dx12GpuProgramData* mData;
};

}
}
