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

class BlockScriptBuilder;

//! registers all the default intrinsics and runtime types (int, float etc).
void RegisterIntrinsics(BlockScriptBuilder* builder);


}
}

#endif
