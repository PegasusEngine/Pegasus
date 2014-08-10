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
#include <d3dcompiler.h>
#include "Pegasus/Graph/NodeGPUData.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Mesh/MeshInputLayout.h"

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

    CComPtr<ID3DBlob> mBlob;
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
    CComPtr<ID3DBlob>     mInputLayoutBlob;
    int mProgramGuid;
    int mProgramVersion;
    bool mProgramValid;
};

struct DXMeshGPUData
{
    static const int MAX_INPUT_ELEMENTS_DESC = 16;
    static const int INPUT_LAYOUT_TABLE_INCREMENT = 8;
    
    //input layout info
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXMeshGPUData, 0x3);
    D3D11_INPUT_ELEMENT_DESC mInputElementsDesc[MAX_INPUT_ELEMENTS_DESC];
    UINT mInputElementsCount;
    
    // buffer description
    D3D11_BUFFER_DESC mVertexBufferDesc[MESH_MAX_STREAMS];
    CComPtr<ID3D11Buffer> mVertexBuffer[MESH_MAX_STREAMS];

    bool mIsIndexed;
    D3D11_BUFFER_DESC mIndexBufferDesc;
    CComPtr<ID3D11Buffer> mIndexBuffer;

    struct InputLayoutEntry
    {
        CComPtr<ID3D11InputLayout> mInputLayout;
        int mProgramGuid;
        int mProgramVersion;
    }* mInputLayoutTable;
    int mInputLayoutTableCount;
    int mInputLayoutTableCapacity;

    int mIndexCount;
    int mVertexCount;
};


}

}


#endif
