/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Fence.h
//! \author Kleber Garcia
//! \date   July 18th 2018
//! \brief  Implementation of fences

#include <Winbase.h>

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

    void Signal(ID3D12CommandQueue* queue);
    void Wait();

private:
    ID3D12Fence* m_fence;
    ID3D12CommandQueue* m_ownerQueue;
    HANDLE m_event;
    uint64_t m_fenceValue;
};

}
}
