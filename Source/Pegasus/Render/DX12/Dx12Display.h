/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Display.h
//! \author Kleber Garcia
//! \date   July 8th 2018
//! \brief  dx12 swap chain implementation.

#pragma once

#include <Pegasus/Render/IDisplay.h>
#include <dxgi1_5.h>
#include "Dx12RDMgr.h"
#include "Dx12Resources.h"
#include "Dx12GpuProgram.h"
#include <vector>

namespace Pegasus
{
namespace Render
{

class Dx12Fence;
class Dx12GpuProgram;
class Dx12Device;

class Dx12Display : public IDisplay
{
public:

    Dx12Display(const DisplayConfig& config, Dx12Device* device);
    virtual ~Dx12Display();

    virtual void Resize(unsigned int width, unsigned int height) override;
    virtual Texture* GetTexture() override;
    virtual RenderTarget* GetRenderTarget() override;
    
    UINT64 GetFenceVal() const;
    void Present(Dx12Fence* fence);

private:
    //! the swap chain
    IDXGISwapChain4*     mSwapChain;

    //cached dx12 device
    Dx12Device* mDevice;

    std::vector<UINT64> mFenceVals;
    std::vector<Dx12TextureRef> mTextures;
    std::vector<Dx12RenderTargetRef> mRts;
};

}
}
