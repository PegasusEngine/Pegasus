/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus                                           */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusAssetTypes.cpp
//! \author	Kleber Garcia
//! \date	30th July 2015
//! \brief	high level description of all the assets this app can have.
//!         \note Edit and view PegasusAssetTypes.inl for full data description of assets

#include "Pegasus/PegasusAssetTypes.h"
namespace Pegasus
{
#define REGISTER_ASSET_TYPE(__runtimeName, __typeGuid, __typeName, __ext, __isStruct, __icon) \
    &__runtimeName,   

static const PegasusAssetTypeDesc* gAllAssetTypes[] =
{
#include "Pegasus/PegasusAssetTypes.inl"
    nullptr
};

#undef REGISTER_ASSET_TYPE

const PegasusAssetTypeDesc * const *  GetAllAssetTypesDescs() { return gAllAssetTypes; }

}
