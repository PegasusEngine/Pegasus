/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Pso.h
//! \author Kleber Garcia
//! \date   September 15th 2018
//! \brief  Convenience class to create PSOs

#pragma once

#include "Pegasus/Core/RefCounted.h"
#include "Pegasus/Core/Ref.h"
#include "Dx12GpuProgram.h"

interface ID3D12PipelineState;

namespace Pegasus
{
namespace Render
{

class Dx12Device;
class Dx12Program;

class Dx12GpuState : public GpuState
{
public:
    Dx12GpuState(const GpuStateConfig& config, Dx12Device* device);
    virtual ~Dx12GpuState();
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetGraphicsPsoTemplate() const { return mGraphicsStateTemplate; }
private:
    void Translate();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mGraphicsStateTemplate;
    D3D12_INPUT_ELEMENT_DESC mElementInputLayoutList[(unsigned)GpuStateConfig::MaxElements];
};

class Dx12Pso : public GpuPipeline
{
public:
    Dx12Pso(Dx12Device* device);
    virtual ~Dx12Pso();

    virtual bool Compile(const GpuPipelineConfig& config);
    virtual bool IsValid() const { return mValid; }

    bool SpaceToTableId(UINT space, Dx12ResType resType, UINT& outTableId) const;

    ID3D12RootSignature* GetD3DRootSignature() const;
    ID3D12PipelineState* GetD3DPso() const { return mPso; }

private:
    enum Type
    {
        PsoGraphics,
        PsoCompute
    };

    ID3D12PipelineState* mPso;
    GpuPipelineConfig mConfig;
    Dx12GpuProgram mProgram;
    Type mType;
    Dx12Device* mDevice;
    bool mValid;
};

typedef Pegasus::Core::Ref<Dx12Pso> Dx12PsoRef;

}
}
