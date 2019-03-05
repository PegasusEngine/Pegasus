/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Defs.h
//! \author Kleber Garcia
//! \date   July 15th, 2018
//! \brief  Common definitions for dx12 stuff

#pragma once

#if PEGASUS_ENABLE_ASSERT
#define DX_VALID_DECLARE(exp) HRESULT hr = exp;PG_ASSERT(hr==S_OK)
#define DX_VALID(exp) hr = exp;PG_ASSERT(hr == S_OK)
#else
#define DX_VALID_DECLARE(exp) exp
#define DX_VALID(exp) exp
#endif

#define D12_NEW(allocator, debugString) PG_NEW(allocator, -1, debugString, Pegasus::Alloc::PG_MEM_PERM)
#define D12_DELETE PG_DELETE

namespace Pegasus
{
namespace Render
{

enum Dx12ResType : unsigned
{
    Dx12_ResTypeBegin,
    Dx12_ResSrv = Dx12_ResTypeBegin,
    Dx12_ResCbv,
    Dx12_ResUav,
    Dx12_ResSampler,
    Dx12_ResTypeCount,
    Dx12_ResInvalid
};

enum Dx12PipelineType : unsigned
{
    Dx12_Vertex,
    Dx12_Pixel,
    Dx12_Domain,
    Dx12_Hull,
    Dx12_Geometry,
    Dx12_Compute,
    Dx12_PipelineMax,
    Dx12_Unknown
};

}
}
