/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Display.h
//! \author Kleber Garcia
//! \date   July 8th 2018
//! \brief  dx12 swap chain implementation.

#include <Pegasus/Render/IDisplay.h>
#include <dxgi1_2.h>

namespace Pegasus
{
namespace Render
{

class Dx12Device;

class Dx12Display : public IDisplay
{
public:
    Dx12Display(const DisplayConfig& config, Alloc::IAllocator* alloc);
    virtual ~Dx12Display();

    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual void Resize(unsigned int width, unsigned int height) override;

private:

    //! the swap chain
    IDXGISwapChain1*     mSwapChain;

    //cached dx12 device
    Dx12Device* mDevice;

    //! target frame buffer width/height
    unsigned int mWidth;
    unsigned int mHeight;
    

};
}
}
