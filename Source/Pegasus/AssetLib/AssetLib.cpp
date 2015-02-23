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
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/ByteStream.h"
#include "Pegasus/Allocator/Alloc.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

#define PAS_EXTENSION ".pas"

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

char toLow(char c)
{
    return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

char toBrac(char c)
{
    return c == '/' ? '\\' : c;
}

static bool PathsAreEqual(const char* str1, const char* str2)
{
    bool eq = true;
    while (eq && *str1 != '\0' && *str2 != '\0')
    {
        char ch1 = *str1;
        char ch2 = *str2;
        ch1 = toBrac(toLow(ch1));
        ch2 = toBrac(toLow(ch2));
        eq = ch1 == ch2;
        ++str1;
        ++str2;
    }
    return eq && *str1 == *str2;
}

Pegasus::AssetLib::AssetLib::~AssetLib()
{
    for (int i = 0; i < mAssets.GetSize(); ++i)
    {      
        PG_DELETE(mAllocator, mAssets[i]);
    }
}

extern void Bison_AssetScriptParse(const Io::FileBuffer* fileBuffer, AssetBuilder* builder);

Io::IoError Pegasus::AssetLib::AssetLib::LoadAsset(const char* name, Pegasus::AssetLib::Asset** assetOut)
{
    //try to find it first
    for (int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (PathsAreEqual(mAssets[i]->GetPath(), name))
        {
            *assetOut = mAssets[i];
            return Io::ERR_NONE;
        }
    }

    //not found? lets build it from a file..
    *assetOut = nullptr;
    mBuilder.Reset();
    Io::FileBuffer fileBuffer;
    Io::IoError err = mIoMgr->OpenFileToBuffer(name, fileBuffer, true, mAllocator); //open the raw file first

    if (err == Io::ERR_NONE)
    {
        // structured means its a json file. non structured means it does not get parsed and the file gets raw'd
        const char* extension = Utils::Strrchr(name, '.');
        bool isStructured = extension != nullptr && !Utils::Stricmp(PAS_EXTENSION, extension);
        *assetOut = PG_NEW(mAllocator, -1, "Asset", Alloc::PG_MEM_TEMP) Asset(mAllocator, isStructured ? Asset::FMT_STRUCTURED : Asset::FMT_RAW);
        (*assetOut)->SetPath(name);
        if (isStructured)
        {
            mBuilder.BeginCompilation(*assetOut);
            Bison_AssetScriptParse(&fileBuffer, &mBuilder);    
            if (mBuilder.GetErrorCount() == 0)
            {
                mAssets.PushEmpty() = *assetOut;
            }
            else
            {
                PG_DELETE(mAllocator, *assetOut);
                *assetOut = nullptr;
                err = Io::ERR_READING_FILE;
            }
        }
        else
        {
            (*assetOut)->SetFileBuffer(fileBuffer);
            fileBuffer.ForgetBuffer(); //forget this buffer, so we dont destroy it twice.
            mAssets.PushEmpty() = *assetOut;
        }
    }
    return err;
}

Io::IoError Pegasus::AssetLib::AssetLib::CreateBlankAsset(const char* name, Asset** assetOut)
{
    //try to find it first
    for (int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (PathsAreEqual(mAssets[i]->GetPath(), name))
        {
            return Io::ERR_WRITING_FILE;  //Cant allow to override this asset
        }
    }

    //not found? lets build it from a file..
    *assetOut = nullptr;
    char* value = "";
    Io::FileBuffer fileBuffer;
    fileBuffer.OwnBuffer(nullptr, value, 2);
    Io::IoError err = mIoMgr->SaveFileToBuffer(name, fileBuffer);
    fileBuffer.ForgetBuffer();

    if (err == Io::ERR_NONE)
    {
        // structured means its a json file. non structured means it does not get parsed and the file gets raw'd
        const char* extension = Utils::Strrchr(name, '.');
        bool isStructured = extension != nullptr && !Utils::Stricmp(PAS_EXTENSION, extension);
        *assetOut = PG_NEW(mAllocator, -1, "Asset", Alloc::PG_MEM_TEMP) Asset(mAllocator, isStructured ? Asset::FMT_STRUCTURED : Asset::FMT_RAW);
        (*assetOut)->SetPath(name);
        mAssets.PushEmpty() = *assetOut;
        if (!isStructured)
        {
            //alocate a little fake buffer
            char* buff = PG_NEW_ARRAY(mAllocator, -1, "Raw Asset", Alloc::PG_MEM_TEMP, char, 2);
            buff[0] = '\0';
            fileBuffer.OwnBuffer(mAllocator, buff, 2);
            (*assetOut)->SetFileBuffer(fileBuffer);
            fileBuffer.ForgetBuffer();
        }
    }
    return err;
}

void Pegasus::AssetLib::AssetLib::BindAssetToRuntimeObject(Asset* asset, RuntimeAssetObject* runtimeObject)
{
    asset->SetRuntimeData(runtimeObject);
    runtimeObject->Connect(asset, 0, this);
}

Io::IoError Pegasus::AssetLib::AssetLib::SaveAsset(Asset* asset)
{
    if (asset->GetFormat() == Asset::FMT_STRUCTURED)
    {
        Utils::ByteStream bs(mAllocator);
        asset->DumpToStream(bs);
        Io::FileBuffer fb;
        fb.OwnBuffer(mAllocator, static_cast<char*>(bs.GetBuffer()), bs.GetSize());
        Io::IoError result = mIoMgr->SaveFileToBuffer(asset->GetPath(), fb);
        fb.ForgetBuffer();
        return result;
    }
    else
    {
        return mIoMgr->SaveFileToBuffer(asset->GetPath(), *asset->Raw());
    }
}


void Pegasus::AssetLib::AssetLib::DestroyAsset(Asset* asset)
{
    //find this asset
    for (int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (mAssets[i] == asset)
        {
            if (asset->GetRuntimeData() != nullptr)
            {
                asset->GetRuntimeData()->mAsset = nullptr;
            }
            PG_DELETE(mAllocator, mAssets[i]);
            mAssets.Delete(i);
            return;
        }
    }
    PG_FAILSTR("Asset not found!, do not call this function if this asset is not associated with this library");
}
