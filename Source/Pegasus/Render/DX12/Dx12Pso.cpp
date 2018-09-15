#include "Dx12Pso.h"
#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{

Dx12Pso::Dx12Pso(Dx12Device* device)
: Core::RefCounted(device->GetAllocator()), mDevice(device)
{
}

Dx12Pso::~Dx12Pso()
{
}

void Dx12Pso::Compile(const Dx12PsoDesc& desc)
{
    mDesc = desc;
}

}
}
