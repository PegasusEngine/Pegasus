/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXGpuDataDefs.h
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  Pegasus directx11 definitions of gpu data for render elements

#ifndef PEGASUS_DXGPUDATADEF_H
#define PEGASUS_DXGPUDATADEF_H

#include <d3d11.h>
#include <atlbase.h>
#include "Pegasus/Graph/NodeGPUData.h"

namespace Pegasus
{

namespace Render
{

struct DXShaderGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXShaderGPUData, 0x1);
    Pegasus::Shader::ShaderType mType;
    union 
    {
       ID3D11PixelShader*    mPixel;
       ID3D11VertexShader*   mVertex;
       ID3D11DomainShader*   mDomain;
       ID3D11HullShader*     mHull;
       ID3D11GeometryShader* mGeometry;
       ID3D11ComputeShader*  mCompute;
       ID3D11DeviceChild*    mDeviceChild;
    };

    CComPtr<ID3D11ShaderReflection> mReflectionInfo;
};

struct DXProgramGPUData
{
   PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXProgramGPUData, 0x2);
   CComPtr<ID3D11PixelShader>    mPixel;
   CComPtr<ID3D11VertexShader>   mVertex;
   CComPtr<ID3D11DomainShader>   mDomain;
   CComPtr<ID3D11HullShader>     mHull;
   CComPtr<ID3D11GeometryShader> mGeometry;
   CComPtr<ID3D11ComputeShader>  mCompute;
};


}

}


#endif
