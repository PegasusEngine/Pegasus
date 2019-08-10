#include "Pegasus/UnitTests/RenderTests.h"
#include "Pegasus/UnitTests/UnitTests.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Memory/mallocfreeallocator.h"
#include "Pegasus/Core/Log.h"
#include <iostream>

//Dx12 includes:
#include "Pegasus/Render/IDevice.h"
#include "../Source/Pegasus/Render/DX12/Dx12Device.h"
#include "../Source/Pegasus/Render/DX12/Dx12Defs.h"


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

class RenderHarness : public TestHarness
{
public:
    RenderHarness();
    virtual void Initialize();
    virtual void Destroy();
    virtual const TestFunc* GetTests(int& funcSz) const;

    Dx12Device* CreateDevice();
    void DestroyDevice();

private:
    Dx12Device* mDevice;
    Pegasus::Memory::MallocFreeAllocator mAllocator;
};


RenderHarness::RenderHarness()
: mAllocator(0), mDevice(nullptr)
{
}

void RenderHarness::Initialize()
{
	auto* logManager = Core::Singleton<Core::LogManager>::CreateInstance(&mAllocator);
    logManager->RegisterHandler(RenderLogFn);
}

void RenderHarness::Destroy()
{
    Core::Singleton<Core::LogManager>::DestroyInstance();
}

Dx12Device* RenderHarness::CreateDevice()
{
    if (mDevice == nullptr)
    {
        Pegasus::Render::DeviceConfig config;
        config.mModuleHandle = 0;
        config.mIOManager = nullptr;
        mDevice = D12_NEW(&mAllocator, "TestDevice") Dx12Device(config, &mAllocator);
    }

    return mDevice;
}

void RenderHarness::DestroyDevice()
{
    D12_DELETE(&mAllocator, mDevice);
    mDevice = nullptr;
}

bool runCreateDevice(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    return rh->CreateDevice() != nullptr;
}

bool runCreateShader(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
	return false;
}

bool runDestroyDevice(TestHarness* harness)
{
    RenderHarness* rh = static_cast<RenderHarness*>(harness);
    rh->DestroyDevice();
    return true;
}


TestHarness* createRenderHarness()
{
    return new RenderHarness();
}

const TestFunc gRenderTests[] = {
    DECLARE_TEST(CreateDevice),
    DECLARE_TEST(DestroyDevice)
};

const TestFunc* RenderHarness::GetTests(int& funcSz) const
{
    funcSz = sizeof(gRenderTests)/sizeof(gRenderTests[0]);
    return gRenderTests;
}

}
}
