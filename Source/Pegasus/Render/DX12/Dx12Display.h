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

namespace Pegasus
{
namespace Render
{

class Dx12RenderContext;
class Dx12GpuProgram;
class Dx12Device;

class Dx12Display : public IDisplay
{
public:

    Dx12Display(const DisplayConfig& config, Alloc::IAllocator* alloc);
    virtual ~Dx12Display();

    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual void Resize(unsigned int width, unsigned int height) override;
    UINT GetBuffering() const { return Buffering; }
    void Flush();

private:

    enum
    {
        Buffering = 2 //double buffered
    };

    //! the swap chain
    IDXGISwapChain4*     mSwapChain;

    //cached dx12 device
    Dx12Device* mDevice;

    Dx12RDMgr::Handle mRtvBuffers[Buffering];
    CComPtr<ID3D12Resource> mColorResources[Buffering];
 
    UINT mBackBufferIdx;

    CComPtr<ID3D12GraphicsCommandList> mCmdList;

    //! target frame buffer width/height
    unsigned int mWidth;
    unsigned int mHeight;

    Dx12RenderContext* mRenderContext;
};
}
}
