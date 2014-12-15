/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BsIntrinsics.h
//! \author Kleber Garcia
//! \date   October 12th 2014
//! \brief  runtime library intrinsic functions

#ifndef BLOCKSCRIPT_INTRINSICS_H
#define BLOCKSCRIPT_INTRINSICS_H

namespace Pegasus
{
namespace BlockScript
{

class BlockLib;

//! registers all the default intrinsics and runtime types (int, float etc).
//! \param lib the core runtime library
void RegisterIntrinsics(BlockLib* lib);


}
}

#endif
