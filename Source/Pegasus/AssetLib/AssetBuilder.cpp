/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetBuilder.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  Asset Builder (parser helper)

#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/AssetBuilder.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

#define AS_NEW PG_NEW(&mAstAllocator, -1, "AssetLibrary::ASTree", Pegasus::Alloc::PG_MEM_TEMP)
#define AST_PAGE_SIZE (10 * sizeof(AssetLib::Object))
#define MAX_STRING_PAGE_SIZE 512

AssetBuilder::AssetBuilder(Alloc::IAllocator* allocator)
: mArrStack(allocator),
  mObjStack(allocator),
  mFinalAsset(nullptr),
  mCurrentLine(1)
{
    mAstAllocator.Initialize(AST_PAGE_SIZE, allocator);
    mStringAllocator.Initialize(MAX_STRING_PAGE_SIZE, allocator);
}

AssetBuilder::~AssetBuilder()
{
}

void AssetBuilder::Reset()
{
    mCurrentLine = 1;
    mArrStack.Clear();
    mObjStack.Clear();
    mFinalAsset = nullptr;
}

Array* AssetBuilder::BeginArray()
{
    Array* arr = AS_NEW Array(&mAstAllocator);
    mArrStack.PushEmpty() = arr;
    return arr;
}

Array* AssetBuilder::GetArray()
{
    return mArrStack[mArrStack.GetSize() - 1];
}

void AssetBuilder::EndArray()
{
    mArrStack.Pop();
}

Object* AssetBuilder::BeginObject()
{
    Object* obj = AS_NEW Object(&mAstAllocator);
    mObjStack.PushEmpty() = obj; 
    return obj;
}

Object* AssetBuilder::GetObject()
{
    return mObjStack[mObjStack.GetSize() - 1];
}

void AssetBuilder::EndObject()
{
    mObjStack.Pop();
}

Asset* AssetBuilder::BuildAsset(Object* obj)
{
    if (obj == nullptr)
    {
        return nullptr;
    }
    Asset* asset = AS_NEW Asset(obj);
    mFinalAsset = asset; 
    return asset;
}

const char* AssetBuilder::CopyString(const char* string)
{

    int strSize = Utils::Strlen(string) + 1;
    if (strSize > MAX_STRING_PAGE_SIZE)
    {
        return nullptr;
    }

    char* strAllocation = static_cast<char*>(mStringAllocator.Alloc(
        strSize,
        Alloc::PG_MEM_TEMP
    ));

    strAllocation[0] = '\0';
    Utils::Strcat(strAllocation, string);
    return strAllocation;
}
