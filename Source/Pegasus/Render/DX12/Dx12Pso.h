/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Pso.h
//! \author Kleber Garcia
//! \date   September 15th 2018
//! \brief  Convenience class to create PSOs

#include "Pegasus/Core/RefCounted.h"
#include "Pegasus/Core/Ref.h"
#include "Dx12GpuProgram.h"

namespace Pegasus
{
namespace Render
{

class Dx12Device;
class Dx12Program;

struct Dx12PsoDesc
{
    Dx12GpuProgramRef program;
};

class Dx12Pso : public Core::RefCounted
{
public:
    Dx12Pso(Dx12Device* device);
    ~Dx12Pso();

    void Compile(const Dx12PsoDesc& desc);

private:
    Dx12PsoDesc mDesc;
    Dx12Device* mDevice;
};

typedef Pegasus::Core::Ref<Dx12Pso> Dx12PsoRef;

}
}
