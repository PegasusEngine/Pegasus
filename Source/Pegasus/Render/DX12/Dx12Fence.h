/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Fence.h
//! \author Kleber Garcia
//! \date   July 18th 2018
//! \brief  Implementation of fences

#include <windows.h>

struct ID3D12Fence;
struct ID3D12CommandQueue;

namespace Pegasus
{
namespace Render
{

class Dx12Device;

class Dx12Fence
{
public:
    Dx12Fence(Dx12Device* device, ID3D12CommandQueue* ownerQueue);
    ~Dx12Fence();

    UINT64 Signal();
    void WaitOnCpu(UINT64 valueToWait);
    void WaitOnGpu(UINT64 valueToWait, ID3D12CommandQueue* externalQueue = nullptr);
    ID3D12Fence* GetD3D() { return m_fence; }

private:
    ID3D12Fence* m_fence;
    ID3D12CommandQueue* m_ownerQueue;
    HANDLE m_event;
    UINT64 m_fenceValue;
};

}
}
