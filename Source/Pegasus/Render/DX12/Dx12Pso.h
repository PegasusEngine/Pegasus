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

struct Dx12PsoDesc
{
    
};

class Dx12Pso : public Core::RefCounted
{
public:
    Dx12Pso(Dx12Device* device);
    ~Dx12Pso();

    bool IsValid() const { return mPso != nullptr; }
    bool Compile(const Dx12PsoDesc& desc, Dx12GpuProgramRef program);
    bool SpaceToTableId(UINT space, Dx12ResType resType, UINT& outTableId);

    ID3D12RootSignature* GetD3DRootSignature() const;
    ID3D12PipelineState* GetD3DPso() const { return mPso; }

private:
    enum Type
    {
        PsoGraphics,
        PsoCompute
    };

    ID3D12PipelineState* mPso;
    Dx12PsoDesc mDesc;
    Dx12GpuProgramRef mProgram;
    Type mType;
    Dx12Device* mDevice;
};

typedef Pegasus::Core::Ref<Dx12Pso> Dx12PsoRef;

}
}
