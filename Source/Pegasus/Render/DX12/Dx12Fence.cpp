#include "Dx12Fence.h"
#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{

Dx12Fence::Dx12Fence(Dx12Device* device, ID3D12CommandQueue* ownerQueue) : m_fenceValue(0ull)
{
    DX_VALID_DECLARE(device->GetD3D()->CreateFence(0, D3D11_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &m_fence));
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

void Dx12Fence::Signal(ID3D12CommandQueue* queue)
{
    ++m_fenceValue;
    m_ownerQueue->Signal(m_fence, m_fenceValue);
}

void Dx12Fence::Wait()
{
    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        DX_VALID_DECLARE(fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

}
}
