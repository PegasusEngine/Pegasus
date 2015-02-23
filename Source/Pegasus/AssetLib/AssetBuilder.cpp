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
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

AssetBuilder::AssetBuilder(Alloc::IAllocator* allocator)
: mArrStack(allocator),
  mObjStack(allocator),
  mFinalAsset(nullptr),
  mCurrentLine(1),
  mErrorCount(0)
{
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
    mErrorCount = 0;
}

Array* AssetBuilder::BeginArray()
{
    Array* arr = mFinalAsset->NewArray();
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
    Object* obj = mFinalAsset->NewObject();
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

void AssetBuilder::BeginCompilation(Asset* asset)
{
    mFinalAsset = asset; 
}

const char* AssetBuilder::CopyString(const char* string)
{
    return mFinalAsset->CopyString(string);
}

