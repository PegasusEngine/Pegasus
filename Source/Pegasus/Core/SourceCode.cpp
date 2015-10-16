#include "Pegasus/Core/SourceCode.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Core;

SourceCode::SourceCode(Alloc::IAllocator* allocator, Alloc::IAllocator* nodeDataAllocator)
    : Graph::GeneratorNode(allocator, nodeDataAllocator), AssetLib::RuntimeAssetObject(this), mAllocator(allocator), mParents(allocator), mLockParentArray(false)
{
}

SourceCode::~SourceCode()
{
    ClearParents();
}


void SourceCode::SetSource(const char * src, int srcSize)
{
    InvalidateData();
    //reallocate buffer size if more space requested on recompilation
    if (srcSize > mFileBuffer.GetFileSize())
    {
        mFileBuffer.DestroyBuffer();
        mFileBuffer.OwnBuffer (
            mAllocator,
            PG_NEW_ARRAY(mAllocator, -1, "shader src", Pegasus::Alloc::PG_MEM_PERM, char, srcSize),
            srcSize
        );
    }
    mFileBuffer.SetFileSize(srcSize);
    PG_ASSERTSTR(mFileBuffer.GetBufferSize() >= srcSize, "Not enough size to hold the string buffer!");
    Pegasus::Utils::Memcpy(mFileBuffer.GetBuffer(),src,srcSize);
}

#if PEGASUS_ENABLE_PROXIES
const char* SourceCode::GetDisplayName() const
{
    if (GetOwnerAsset() != nullptr)
    {
        return GetOwnerAsset()->GetName();
    }
    else
    {
        return "";
    }
}

#endif


void SourceCode::GetSource (const char ** outSrc, int& outSize) const
{
    *outSrc = mFileBuffer.GetBuffer(); 
    outSize = mFileBuffer.GetFileSize();
}

void SourceCode::Compile()
{
    //copy this vector to a temp vector to avoid pop / push issues

    mLockParentArray = true;
    for (int i = 0; i < mParents.GetSize(); ++i)
    {
        mParents[i]->InvalidateData();
        mParents[i]->Compile();
    }
    mLockParentArray = false;
}


void SourceCode::RegisterParent(SourceCode* parent)
{
    if (!mLockParentArray)
    {
        mParents.PushEmpty() = parent;
    }
}


void SourceCode::UnregisterParent(SourceCode* parent)
{
    if (!mLockParentArray)
    {
        for (int i = 0; i < mParents.GetSize(); ++i)
        {
            if (mParents[i] == parent)
            {
                mParents.Delete(i);
                return;
            }
        }
        PG_FAILSTR("Trying to unregister a source file that has no parent?");
    }

}

void SourceCode::ClearParents()
{
    if (!mLockParentArray)
    {
        mParents.Clear();
    }
}

bool SourceCode::OnReadAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset)
{
    if (asset->GetFormat() != Pegasus::AssetLib::Asset::FMT_RAW)
    {
        PG_LOG('ERR_', "Invalid shader extension");
        return false;
    }

    Io::FileBuffer* fb = asset->Raw();
    SetSource(fb->GetBuffer(), fb->GetFileSize());
    return true;
}

void SourceCode::OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset)
{
    const char* src = nullptr;
    int srcLen = 0;
    GetSource(&src, srcLen);
    Io::FileBuffer* fb = asset->Raw();
    if (srcLen > fb->GetBufferSize())
    {
        Pegasus::Alloc::IAllocator* alloc = fb->GetAllocator();
        fb->DestroyBuffer();
        fb->OwnBuffer(alloc, PG_NEW_ARRAY(alloc, -1, "", Alloc::PG_MEM_TEMP, char, srcLen), srcLen);
    }
    fb->SetFileSize(srcLen);
    Utils::Memcpy(fb->GetBuffer(), src, srcLen);
}
