/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Device.h
//! \author Kleber Garcia
//! \date   May 25th 2018
//! \brief  Class that implements the dx12 device

#ifndef PEGASUS_DX12DEVICE_H
#define PEGASUS_DX12DEVICE_H

#include <D3D12.h>
#include <string>
#include "Pegasus/Render/IDevice.h"


struct IDXGIFactory4;
struct IDXGIAdapter4;

namespace Pegasus
{
namespace Render
{

struct GraphicsCardInfos
{
    IDXGIFactory4* dxgiFactory = nullptr;
    struct Card {
		std::string description;
        IDXGIAdapter4* adapter4 = nullptr;
    }* infos = nullptr; 
	int cardCounts = 0;
    int usedIndex = 0;
};

//! Windows specific device for open gl graphics api
class Dx12Device : public IDevice
{
public:
    //! Constructor
    //! \param config the configuration needed
    //! \param render allocator for internal allocations
    Dx12Device(const DeviceConfig& config, Alloc::IAllocator * allocator);
    virtual ~Dx12Device();

    GraphicsCardInfos* GetGraphicsCardInfos() const { return sCardInfos; }
    ID3D12Device* GetD3D() { return m_device; }

private:

    static int sDeviceRefCounts;
    static GraphicsCardInfos* sCardInfos;
    ID3D12Device2* m_device;

};

}
}


#endif
