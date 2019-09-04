#include "Pegasus/UnitTests/RenderTests.h"
#include "Pegasus/UnitTests/UnitTests.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Memory/mallocfreeallocator.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Render/IDevice.h"
#include <Pegasus/Core/Shared/ISourceCodeProxy.h>
#include <Pegasus/Core/Shared/CompilerEvents.h>
#include "../Source/Pegasus/Render/Common/ResourceStateTable.h"
#include <string>
#include <iostream>

using namespace Pegasus::Render;

const char* simpleProgram0 = R"(
    SamplerState gSampler : register(s0,space0);
    Texture2D tex0 : register(t0, space0);

	struct VsIn
	{
		float4 pos : POSITION0;
		float3 n : NORMAL0;
	};

    struct VsOut
    {
        float4 p : SV_Position;
        float3 normal : TEXCOORD;
    };

	cbuffer Constants : register(b0)
	{
		float4x4 g_viewProjTransform;
	};

	void vsMain(VsIn vsIn, out VsOut vsOut)
	{
		vsOut.p = mul(vsIn.pos, g_viewProjTransform);
        vsOut.normal = float3(0,0,0);//vsIn.n;
	}

	void psMain(in VsOut vsOut, out float4 c : SV_Target0)
	{
		c = float4(tex0.Sample(gSampler, vsOut.normal.xy).rgb, 1.0);
	}
)";

const char* simpleCs0 = R"(
    Buffer<float4> input : register(t0, space0);
    RWBuffer<float4> output : register(u0, space0);

    cbuffer Constants : register(b0)
    {
        uint4 gCounts;
    };

    [numthreads(32,1,1)]
    void csMain(uint di : SV_DispatchThreadID)
    {
        output[di] = input[di];
    }
)";

using namespace Pegasus::Render;

namespace Pegasus
{
namespace Tests
{

static void RenderLogFn(Core::LogChannel logChannel, const char* msgStr)
{
    char logChnl[5];
    for (int i = 0; i < 4; ++i)
    {
		logChnl[i] = reinterpret_cast<char*>(&logChannel)[3 - i];
    }

    logChnl[4] = 0;
    std::cout << "[" << logChnl << "]" << msgStr << std::endl;
}

static Pegasus::Core::AssertReturnCode AssertHandler(const char * testStr,
	const char * fileStr,
	int line,
	const char * msgStr)
{
	std::cout << "ASSERT[" << fileStr << ":" << line << "] " << msgStr << std::endl;
	return Pegasus::Core::ASSERTION_CONTINUE;
}

#if PEGASUS_USE_EVENTS
class CompilerListener : public Pegasus::Core::CompilerEvents::ICompilerEventListener
{
public:
    virtual void OnInitUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name) override
    {
    }

    virtual void OnDestroyUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name)override
    {
    }
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::SourceLoadedEvent& e)  override
    {
    }
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::CompilationEvent& e)  override
    {
    }
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::CompilationNotification& e)  override
    {
        if (e.GetType() == Pegasus::Core::CompilerEvents::CompilationNotification::COMPILATION_ERROR)
        {
            std::cout << "Gpu Program Error: " << e.GetDescription() << std::endl;
        }
    }
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::LinkingEvent& e)  override
    {
    }
};
#endif

class RenderHarness : public TestHarness
{
public:
    RenderHarness();
    virtual void Initialize();
    virtual void Destroy();
    virtual const TestFunc* GetTests(int& funcSz) const;

    IDevice* CreateDevice();
    void DestroyDevice();

	Pegasus::Alloc::IAllocator* GetAllocator() { return &mAllocator;  }

#if PEGASUS_USE_EVENTS
    CompilerListener& GetCompilerListener() { return mCompilerListener; }
#endif

private:
    IDevice* mDevice;
    Pegasus::Memory::MallocFreeAllocator mAllocator;

#if PEGASUS_USE_EVENTS
    CompilerListener mCompilerListener;
#endif
};


RenderHarness::RenderHarness()
: mAllocator(0), mDevice(nullptr)
{
}

void RenderHarness::Initialize()
{
	auto* logManager = Core::Singleton<Core::LogManager>::CreateInstance(&mAllocator);
    logManager->RegisterHandler(RenderLogFn);

	auto* assertMgr = Core::Singleton<Core::AssertionManager>::CreateInstance(&mAllocator);
	assertMgr->RegisterHandler(AssertHandler);
}

void RenderHarness::Destroy()
{
    Core::Singleton<Core::LogManager>::DestroyInstance();
}

IDevice* RenderHarness::CreateDevice()
{
    if (mDevice == nullptr)
    {
        Pegasus::Render::DeviceConfig config;
        config.platform = DevicePlat::Dx12;
        config.mModuleHandle = 0;
        config.mIOManager = nullptr;
        mDevice = IDevice::CreatePlatformDevice(config, &mAllocator);
    }

    return mDevice;
}

void RenderHarness::DestroyDevice()
{
    PG_DELETE(&mAllocator, mDevice);
    mDevice = nullptr;
}

bool runCreateDevice(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    return rh->CreateDevice() != nullptr;
}

bool runCreateSimpleGpuPipeline(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    IDevice* device = rh->CreateDevice();
    GpuPipelineRef gpuPipeline = device->CreateGpuPipeline();

#if PEGASUS_USE_EVENTS
    gpuPipeline->SetEventListener(&rh->GetCompilerListener());
#endif

    GpuPipelineConfig gpuPipelineConfig;
    gpuPipelineConfig.source = simpleProgram0;
    gpuPipelineConfig.mainNames[Pipeline_Vertex] = "vsMain";
    gpuPipelineConfig.mainNames[Pipeline_Pixel] = "psMain";

    bool result = gpuPipeline->Compile(gpuPipelineConfig);
    if (!result)
        return false;
    
	return true;
}

bool runTestSimpleCompute(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    IDevice* device = rh->CreateDevice();
    GpuPipelineConfig gpuPipelineConfig;
    gpuPipelineConfig.source = simpleCs0;
    gpuPipelineConfig.mainNames[Pipeline_Compute] = "csMain";

    GpuPipelineRef gpuPipeline = device->CreateGpuPipeline();

#if PEGASUS_USE_EVENTS
    gpuPipeline->SetEventListener(&rh->GetCompilerListener());
#endif
    
    if (!gpuPipeline->Compile(gpuPipelineConfig))
        return false;

    return true;
}

bool runDestroyDevice(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    rh->DestroyDevice();
    return true;
}

bool runTestResourceStateTable(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    IDevice* device = rh->CreateDevice();
    ResourceStateTable* resourceStateTable = device->GetResourceStateTable();

    uintptr_t state0 = 0xdeadbeef;
    uintptr_t state1 = 0xdeadbeea;
    uintptr_t state2 = 0xffffffff;
    uintptr_t state3 = 0xaaaaaaaa;

    int slot0 = resourceStateTable->CreateStateSlot();
    int slot1 = resourceStateTable->CreateStateSlot();
    ResourceStateTable::Domain d0 = resourceStateTable->CreateDomain();

    resourceStateTable->StoreState(d0, slot0, state0);
    resourceStateTable->StoreState(d0, slot1, state1);

    ResourceStateTable::Domain d1 = resourceStateTable->CreateDomain();
    resourceStateTable->StoreState(d1, slot0, state2);
    resourceStateTable->StoreState(d1, slot1, state3);

    uintptr_t result;
    bool success;
    success = resourceStateTable->GetState(d0, slot0, result);
    if (!success || result != state0)
        return false;

    success = resourceStateTable->GetState(d0, slot1, result);
    if (!success || result != state1)
        return false;

    success = resourceStateTable->GetState(d1, slot0, result);
    if (!success || result != state2)
        return false;

    success = resourceStateTable->GetState(d1, slot1, result);
    if (!success || result != state3)
        return false;

	resourceStateTable->RemoveDomain(d0);
    success = resourceStateTable->GetState(d0, slot1, result);
    if (success)
        return false;
    
	return true;
}


TestHarness* createRenderHarness()
{
    return new RenderHarness();
}

const TestFunc gRenderTests[] = {
    DECLARE_TEST(CreateDevice),
    DECLARE_TEST(CreateSimpleGpuPipeline),
    DECLARE_TEST(TestSimpleCompute),
    DECLARE_TEST(TestResourceStateTable),
    DECLARE_TEST(DestroyDevice)
};

const TestFunc* RenderHarness::GetTests(int& funcSz) const
{
    funcSz = sizeof(gRenderTests)/sizeof(gRenderTests[0]);
    return gRenderTests;
}

}
}
