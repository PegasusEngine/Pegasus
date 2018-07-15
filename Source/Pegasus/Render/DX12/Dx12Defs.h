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
