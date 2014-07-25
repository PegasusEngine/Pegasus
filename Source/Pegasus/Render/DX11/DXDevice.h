/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXDevice.h
//! \author Kleber Garcia
//! \date   17th of July 2014
//! \brief  Class that implements the platform specific / render api specific device

#ifndef PEGASUS_DXDEVICE_H
#define PEGASUS_DXDEVICE_H

#include <D3D11.h>
#include "Pegasus/Render/IDevice.h"


namespace Pegasus
{
namespace Render
{

//! Windows specific device for open gl graphics api
class DXDevice : public IDevice
{
public:
    //! Constructor
    //! \param config the configuration needed
    //! \param render allocator for internal allocations
    DXDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);
    virtual ~DXDevice();
    
    ID3D11Device * GetD3D() const { return mD3D11Device; }
    ID3D11DeviceContext * GetD3DImmContext() const { return mD3D11ImmContext; }
    
private:
    ID3D11Device * mD3D11Device;
    ID3D11DeviceContext * mD3D11ImmContext;
    
    
};

}
}


#endif
