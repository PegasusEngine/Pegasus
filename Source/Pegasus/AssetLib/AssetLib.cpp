/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLib.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLib Class.
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/AssetBuilder.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/ByteStream.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

Pegasus::AssetLib::AssetLib::AssetLib(Alloc::IAllocator* allocator, Io::IOManager* mgr)
: mBuilder(allocator),
  mIoMgr(mgr),
  mAllocator(allocator),
  mAssets(allocator)
{
#if PEGASUS_ENABLE_PROXIES
    mProxy.SetObject(this);
#endif
}

Pegasus::AssetLib::AssetLib::~AssetLib()
{
}

extern void Bison_AssetScriptParse(const Io::FileBuffer* fileBuffer, AssetBuilder* builder);

Io::IoError Pegasus::AssetLib::AssetLib::LoadAsset(const char* name, Asset** assetOut)
{
    //try to find it first
    for (int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (!Utils::Strcmp(mAssets[i]->GetPath(), name))
        {
            *assetOut = mAssets[i];
            return Io::ERR_NONE;
        }
    }

    *assetOut = nullptr;
    mBuilder.Reset();
    Io::FileBuffer fileBuffer;
    Io::IoError err = mIoMgr->OpenFileToBuffer(name, fileBuffer, true, mAllocator);
    if (err == Io::ERR_NONE)
    {
        Bison_AssetScriptParse(&fileBuffer, &mBuilder);    
        *assetOut = mBuilder.GetBuiltAsset();
        if (*assetOut == nullptr)
        {
            err = Io::ERR_READING_FILE;
        }
        else
        {
            (*assetOut)->SetPath(name);
            mAssets.PushEmpty() = *assetOut;
        }
    }
    return err;
}

Io::IoError Pegasus::AssetLib::AssetLib::SaveAsset(Asset* asset)
{
    Utils::ByteStream bs(mAllocator);
    asset->DumpToStream(bs);
    Io::FileBuffer fb;
    fb.OwnBuffer(mAllocator, static_cast<char*>(bs.GetBuffer()), bs.GetSize());
    Io::IoError result = mIoMgr->SaveFileToBuffer(asset->GetPath(), fb);
    fb.ForgetBuffer();
    return result;
}

