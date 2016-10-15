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
#include "Pegasus/Render/Render.h"

#define MAX_UNIFORM_NAME 64
#define UNIFORM_DATA_INCREMENT 16
#define GLOBAL_UNIFORM_COUNT 8

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

    struct UniformReflectionData
    {
        char mUniformName[MAX_UNIFORM_NAME];
        int  mStageCount;
        struct StageBinding
        {
            Pegasus::Shader::ShaderType mPipelineType;
            int mBindPoint;
            int mBindCount;      
            int mSize;
            D3D_SHADER_INPUT_TYPE mType;

            StageBinding()
            : mPipelineType(Pegasus::Shader::SHADER_STAGE_INVALID),
              mBindPoint(-1),
              mBindCount(-1),
              mSize(-1)
            {
            }
        } mStageBindings[Pegasus::Shader::SHADER_STAGES_COUNT];

        UniformReflectionData()
        : mStageCount(0)
        {
            mUniformName[0] = '\0';
        }             
    }* mReflectionData;
    int mReflectionDataCount;
    int mReflectionDataCapacity;
    int mProgramGuid;
    int mProgramVersion;
    bool mProgramValid;

    // Global uniforms pushed by render systems. For example, cameras.
    Render::Uniform   mGlobalUniforms[GLOBAL_UNIFORM_COUNT];
    Render::BufferRef mGlobalBuffers[GLOBAL_UNIFORM_COUNT];
    int mGlobalUniformCount;
};

struct DXBufferGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXBufferGPUData, 0x3);

    CComPtr<ID3D11Buffer> mBuffer;
    CComPtr<ID3D11UnorderedAccessView> mUav;
    CComPtr<ID3D11ShaderResourceView> mSrv;
    D3D11_BUFFER_DESC mDesc;
    D3D11_UNORDERED_ACCESS_VIEW_DESC mUavDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC mSrvDesc;
};

struct DXMeshGPUData
{
    static const int MAX_INPUT_ELEMENTS_DESC = 16;
    static const int INPUT_LAYOUT_TABLE_INCREMENT = 8;
    
    //input layout info
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXMeshGPUData, 0x4);

    D3D11_INPUT_ELEMENT_DESC mInputElementsDesc[MAX_INPUT_ELEMENTS_DESC];
    UINT mInputElementsCount;

    //draw info
    D3D_PRIMITIVE_TOPOLOGY mTopology;
    bool mIsIndexed;
    bool mIsIndirect;
    
    // buffer description
    DXBufferGPUData mVertexStreams[MESH_MAX_STREAMS];
    DXBufferGPUData mIndexStream;
    DXBufferGPUData mIndirectDrawStream;

    // internal buffers so user can access them. Just wrap the vertex streams.
    Render::BufferRef mVertexBuffers[MESH_MAX_STREAMS];
    Render::BufferRef mIndexBuffer;
    Render::BufferRef mDrawIndirectBuffer;

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


struct DXTextureGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXTextureGPUData, 0x5);

    D3D11_TEXTURE2D_DESC mDesc;

    CComPtr<ID3D11Texture2D> mTexture;

    D3D11_SHADER_RESOURCE_VIEW_DESC mSrvDesc;
    CComPtr<ID3D11ShaderResourceView> mSrv;

    D3D11_UNORDERED_ACCESS_VIEW_DESC mUavDesc;
    CComPtr<ID3D11UnorderedAccessView> mUav;
};

struct DXTextureGPUData3d
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXTextureGPUData3d, 0x6);

    D3D11_TEXTURE3D_DESC mDesc;

    CComPtr<ID3D11Texture3D> mTexture;

    D3D11_SHADER_RESOURCE_VIEW_DESC mSrvDesc;
    CComPtr<ID3D11ShaderResourceView> mSrv;

    D3D11_UNORDERED_ACCESS_VIEW_DESC mUavDesc;
    CComPtr<ID3D11UnorderedAccessView> mUav;
};

struct DXRenderTargetGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXRenderTargetGPUData, 0x7);
    enum Dim
    {
        Dim_2d,
        Dim_3d
    };
    
    Dim mDim;

    DXTextureGPUData mTextureView;
    DXTextureGPUData3d mTextureView3d;
    
    D3D11_RENDER_TARGET_VIEW_DESC mDesc;
    CComPtr<ID3D11RenderTargetView> mRenderTarget;
};

struct DXSampler
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(DXSampler, 0x8);
    
    D3D11_SAMPLER_DESC mDesc;
    CComPtr<ID3D11SamplerState> mSampler;
};


void DXInitBufferData(DXBufferGPUData& outBuffer);

void DXCreateBuffer(
    ID3D11Device * device,
    int bufferSize,
    int elementCount,
    bool isDynamic,
    void* initData,
    D3D11_BIND_FLAG bindFlags,
    DXBufferGPUData& outBuffer,
    UINT extraMiscFlags = 0);

} //namespace Render
} //namespace Pegasus


#endif
