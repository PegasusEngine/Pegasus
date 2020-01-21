#include "Dx12Fence.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"

namespace Pegasus
{
namespace Render
{

Dx12Fence::Dx12Fence(Dx12Device* device, ID3D12CommandQueue* ownerQueue) : m_fenceValue(0ull)
{
    DX_VALID_DECLARE(device->GetD3D()->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &((void*)m_fence)));
    m_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    ownerQueue->AddRef();
    m_ownerQueue = ownerQueue;
}

Dx12Fence::~Dx12Fence()
{
    m_ownerQueue->Release();
    if (m_fence)
        m_fence->Release();
    CloseHandle(m_event);
}

UINT64 Dx12Fence::Signal()
{
    Signal(m_fenceValue + 1);
    return m_fenceValue;
}

void Dx12Fence::Signal(UINT64 value)
{
    m_fenceValue = value;
    m_ownerQueue->Signal(m_fence, value);
}

bool Dx12Fence::IsComplete(UINT64 valueToCmp)
{
    return (m_fence->GetCompletedValue() >= valueToCmp);
}

void Dx12Fence::WaitOnCpu(UINT64 valueToWait)
{
    if (m_fence->GetCompletedValue() < valueToWait)
    {
        DX_VALID_DECLARE(m_fence->SetEventOnCompletion(valueToWait, m_event));
        WaitForSingleObject(m_event, INFINITE);
    }
}

void Dx12Fence::WaitOnGpu(UINT64 valueToWait, ID3D12CommandQueue* externalQueue)
{
    auto* targetQueue = externalQueue != nullptr ? externalQueue : m_ownerQueue;
    DX_VALID_DECLARE(targetQueue->Wait(m_fence, valueToWait));
}

}
}
