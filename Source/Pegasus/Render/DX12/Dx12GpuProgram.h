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
#include <Pegasus/Core/RefCounted.h>
#include <Pegasus/Core/Ref.h>
#include <Pegasus/Core/Shared/EventDefs.h>
#include <Pegasus/Core/Shared/CompilerEvents.h>
#include <vector>
#include <d3d12.h>
#include <atlbase.h>

namespace Pegasus
{
namespace Render
{

enum Dx12PipelineType : unsigned
{
    Dx12_Vertex,
    Dx12_Pixel,
    Dx12_Domain,
    Dx12_Hull,
    Dx12_Geometry,
    Dx12_Compute,
    Dx12_PipelineMax,
    Dx12_Unknown
};

enum Dx12ResType : unsigned
{
    Dx12_ResTypeBegin,
    Dx12_ResSrv = Dx12_ResTypeBegin,
    Dx12_ResCbv,
    Dx12_ResUav,
    Dx12_ResSampler,
    Dx12_ResCount,
    Dx12_ResInvalid
};

struct Dx12TableLayout
{
    struct Range
    {
        Dx12ResType tableType;
		unsigned baseRegister;
		unsigned count;
    };

    std::vector<Range> registerRanges;
};

struct Dx12ProgramDesc
{
    const char* filename;
    const char* mainNames[Dx12_PipelineMax];
    std::vector<Dx12TableLayout> tableLayouts;
    Dx12ProgramDesc()
    {
        filename = nullptr;
        for (unsigned i = 0; i < Dx12_PipelineMax; ++i) mainNames[i] = nullptr;
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

    ID3D12RootSignature* GetRootSignature();
    bool Compile(const Dx12ProgramDesc& programDesc);
    bool IsValid() const;
    const Dx12ProgramDesc& GetDesc() const { return mDesc; }
    CComPtr<ID3DBlob> GetShaderByteCode(Dx12PipelineType type);
    unsigned int GetShaderByteCodeCounts() const;
    CComPtr<ID3DBlob> GetShaderByteCodeByIndex(unsigned int index, Dx12PipelineType& outType);
    

private:

	inline void InvalidateData() {}

    void fillInReflectionData();
    void fillInResourceTableLayouts();
    bool createRootSignature();
    bool mAutoTableLayout;
    Dx12ProgramDesc mDesc;
    Dx12Device* mDevice;
    Dx12GpuProgramParams* mParams;
    Dx12GpuProgramData* mData;
};

typedef Pegasus::Core::Ref<Dx12GpuProgram> Dx12GpuProgramRef;

}
}
