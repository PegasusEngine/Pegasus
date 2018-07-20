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
