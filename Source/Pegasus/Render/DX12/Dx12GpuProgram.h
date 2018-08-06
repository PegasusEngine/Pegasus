/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12GpuProgram.h
//! \author Kleber Garcia
//! \date   August 5th 2018
//! \brief  The so feared encapsulation of PSOs, shader state, shader source.
#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{

struct Dx12GpuProgramParams;

class Dx12GpuProgram
{
public:
    Dx12GpuProgram(Dx12Device* device);
    ~Dx12GpuProgram();
    void Compile();

private:
    Dx12Device* mDevice;
    Dx12GpuProgramParams* mParams;
};

}
}
