/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetRuntimeFactory.cpp
//! \author	Kleber Garcia
//! \date	30th July 2015
//! \brief	Factory that generates asset runtime objects

#include "Pegasus/AssetLib/AssetRuntimeFactory.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Log.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

AssetRuntimeFactory::AssetRuntimeFactory()
: mAssetLib(nullptr)
{
}

bool AssetRuntimeFactory::IsCompatible(const Asset* asset, const char* ext, const Pegasus::PegasusAssetTypeDesc** outputTypeDesc) const
{
    const Pegasus::PegasusAssetTypeDesc*const* descs = GetAssetTypes();
    if (asset->GetFormat() == Pegasus::AssetLib::Asset::FMT_STRUCTURED)
    {
        int intIdx = asset->Root()->FindInt("__type_guid__");
        if (intIdx == -1)
        {
            PG_LOG('WARN' , "The asset %s does not have __type_guid__ associated.", asset->GetPath());
        }
        if (intIdx == -1) 
        {
            return false;
        }
        int typeGuid = asset->Root()->GetInt(intIdx);
        //find factory by type guid
        if (descs != nullptr) while (*descs != nullptr)
        {
            if (typeGuid == (*descs)->mTypeGuid)
            {
                *outputTypeDesc = *descs;
                return true;
            }
            ++descs;
        }
    }
    else
    {
        //find factory by  extension
        while (*descs != nullptr)
        {
            if (descs != nullptr) while (*descs != nullptr)
            {
                if (!Utils::Stricmp(ext,(*descs)->mExtension))
                {
                    *outputTypeDesc = *descs;
                    return true;
                }
                ++descs;
            }
        }
    }
    
    return false;
}
