/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineManager.cpp
//! \author	Karolyn Boulanger
//! \date	07th November 2013
//! \brief	Timeline management

#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Core/Time.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Math/Scalar.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Core/IApplicationContext.h"


#include <string.h>

namespace Pegasus {
namespace Timeline {

//----------------------------------------------------------------------------------------

TimelineManager::TimelineManager(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mNumRegisteredBlocks(0)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
#if PEGASUS_USE_GRAPH_EVENTS
,   mEventListener(nullptr)
#endif
,   mScriptTracker(allocator)
,   mCurrentTimeline(nullptr)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to the timeline object");
}

//----------------------------------------------------------------------------------------

TimelineManager::~TimelineManager()
{
    mCurrentTimeline = nullptr;
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
void TimelineManager::RegisterBlock(const char * className, const char * editorString, CreateBlockFunc createBlockFunc)
#else
void TimelineManager::RegisterBlock(const char * className, CreateBlockFunc createBlockFunc)
#endif
{
    if (className == nullptr)
    {
        PG_FAILSTR("Trying to register a block class but the name is undefined");
        return;
    }
#if PEGASUS_ENABLE_PROXIES
    if (editorString == nullptr)
    {
        PG_FAILSTR("Trying to register a block class but the editor string is undefined");
        return;
    }
#endif


    if (Pegasus::Utils::Strlen(className) < 4)
    {
        PG_FAILSTR("Trying to register a block class but the name (%s) is too short", className);
        return;
    }

    if (createBlockFunc == nullptr)
    {
        PG_FAILSTR("Trying to register a block class but the factory function is undefined");
        return;
    }

    if (mNumRegisteredBlocks >= MAX_NUM_REGISTERED_BLOCKS)
    {
        PG_FAILSTR("Unable to register a block, the maximum number of blocks (%d) has been reached", MAX_NUM_REGISTERED_BLOCKS);
        return;
    }

    // After the parameters have been validated, register the class
    BlockEntry & entry = mRegisteredBlocks[mNumRegisteredBlocks];
#if PEGASUS_COMPILER_MSVC

    strncpy_s(entry.className, MAX_BLOCK_CLASS_NAME_LENGTH + 1,
              className, MAX_BLOCK_CLASS_NAME_LENGTH + 1);
#if PEGASUS_ENABLE_PROXIES
    strncpy_s(entry.editorString, MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
              editorString, MAX_BLOCK_EDITOR_STRING_LENGTH + 1);
#endif

#else

    entry.className[MAX_BLOCK_CLASS_NAME_LENGTH] = '\0';
    strncpy(entry.className, className, MAX_BLOCK_CLASS_NAME_LENGTH);
#if PEGASUS_ENABLE_PROXIES
    strncpy(entry.editorString, editorString, MAX_BLOCK_EDITOR_STRING_LENGTH);
#endif

#endif  // PEGASUS_COMPILER_MSVC

    entry.createBlockFunc = createBlockFunc;
    ++mNumRegisteredBlocks;
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

unsigned int TimelineManager::GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH + 1],
                                               char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH + 1]) const
{
    if ((classNames != nullptr) && (editorStrings != nullptr))
    {
        for (unsigned int b = 0; b < mNumRegisteredBlocks; ++b)
        {
            if ((classNames[b] != nullptr) && (editorStrings[b] != nullptr))
            {
                const BlockEntry & entry = mRegisteredBlocks[b];
#if PEGASUS_COMPILER_MSVC
                strncpy_s(classNames[b], MAX_BLOCK_CLASS_NAME_LENGTH + 1,
                          entry.className, MAX_BLOCK_CLASS_NAME_LENGTH + 1);
                strncpy_s(editorStrings[b], MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
                          entry.editorString, MAX_BLOCK_EDITOR_STRING_LENGTH + 1);
#else
                classNames[b][MAX_BLOCK_CLASS_NAME_LENGTH] = '\0';
                strncpy(classNames[b], entry.className, MAX_BLOCK_CLASS_NAME_LENGTH);
                strncpy(editorStrings[b], entry.editorString, MAX_BLOCK_EDITOR_STRING_LENGTH);
#endif  // PEGASUS_COMPILER_MSVC
            }
            else
            {
                PG_FAILSTR("Invalid output array when getting the list of registered timeline blocks");
                return 0;
            }
        }

        return mNumRegisteredBlocks;
    }
    else
    {
        PG_FAILSTR("Invalid output array when getting the list of registered timeline blocks");
        return 0;
    }
}

#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------
Block * TimelineManager::CreateBlock(const char * className)
{
    const unsigned int registeredBlockIndex = GetRegisteredBlockIndex(className);
    if (registeredBlockIndex < mNumRegisteredBlocks)
    {
        BlockEntry & entry = mRegisteredBlocks[registeredBlockIndex];
        PG_ASSERT(entry.createBlockFunc != nullptr);
        Block* b = entry.createBlockFunc(mAllocator, mAppContext);
        return b;
    }
    else
    {
        if (className == nullptr)
        {
            PG_FAILSTR("Unable to create a block because the provided class name is invalid");
        }
        else
        {
            PG_FAILSTR("Unable to create the block of class %s because it has not been registered", className);
        }

        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void TimelineManager::Update(unsigned int musicPosition)
{
    if (mCurrentTimeline != nullptr)
    {
        mCurrentTimeline->Update(musicPosition);
    }
}

//----------------------------------------------------------------------------------------


#if PEGASUS_USE_GRAPH_EVENTS
void TimelineManager::RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener)
{
    for (int i = 0; i < mScriptTracker.GetScriptCount(); ++i)
    {
        mScriptTracker.GetScriptById(i)->SetEventListener(mEventListener);
    }
    mEventListener = eventListener;
}
#endif

//----------------------------------------------------------------------------------------

unsigned int TimelineManager::GetRegisteredBlockIndex(const char * className) const
{
    if (className == nullptr)
    {
        PG_FAILSTR("Trying to find a block class by name but the name is undefined");
        return mNumRegisteredBlocks;
    }

    unsigned int index;
    for (index = 0; index < mNumRegisteredBlocks; ++index)
    {
        if (Pegasus::Utils::Strcmp(mRegisteredBlocks[index].className, className) == 0)
        {
            // Block found
            return index;
        }
    }

    // Block not found
    PG_ASSERT(index == mNumRegisteredBlocks);
    return mNumRegisteredBlocks;
}

void TimelineManager::InitializeAllTimelines()
{
    if (mCurrentTimeline != nullptr)
    {
        mCurrentTimeline->InitializeBlocks();
    }
}

void TimelineManager::ShutdownAllTimelines()
{
    if (mCurrentTimeline != nullptr)
    {
        mCurrentTimeline->ShutdownBlocks();
    }
}

TimelineScriptReturn TimelineManager::LoadScript(const char* path)
{
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(path);
    if (obj != nullptr && obj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
    {
        return obj;
    }
    return nullptr;
}

TimelineSourceReturn TimelineManager::LoadHeader(const char* path)
{
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(path);
    if (obj != nullptr && obj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_H_BLOCKSCRIPT.mTypeGuid)
    {
        return obj;
    }
    return nullptr;
}

TimelineReturn TimelineManager::LoadTimeline(const char* path)
{
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(path);
    if (obj != nullptr && obj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_TIMELINE.mTypeGuid)
    {
        return obj;
    }
    return nullptr;
}

TimelineScriptReturn TimelineManager::CreateScript()
{
    TimelineScriptRef scriptRef = PG_NEW(mAllocator, -1, "Timeline Script", Alloc::PG_MEM_TEMP)
             TimelineScript(mAllocator, mAppContext);

#if PEGASUS_USE_GRAPH_EVENTS
    //register event listener
    scriptRef->SetEventListener(GetEventListener());
    GRAPH_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(scriptRef->GetProxy()), "BlockScript", scriptRef->GetEventListener());
#endif

    scriptRef->Compile();
    
    //TODO remove tracker on release?
    mScriptTracker.RegisterScript(&(*scriptRef));

    return scriptRef;
}

TimelineSourceReturn TimelineManager::CreateHeader()
{
    TimelineSourceRef scriptRef = PG_NEW(mAllocator, -1, "Timeline Script Header", Alloc::PG_MEM_TEMP)
                 TimelineSource(mAllocator);
    
#if PEGASUS_USE_GRAPH_EVENTS
    //register event listener
    scriptRef->SetEventListener(GetEventListener());
    GRAPH_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(scriptRef->GetProxy()), "BlockScript", scriptRef->GetEventListener());
#endif
    return scriptRef;
}

TimelineRef TimelineManager::CreateTimeline()
{
    PG_ASSERTSTR(mCurrentTimeline == nullptr, "Currently pegasus only supports one timeline. Dont go crazy yet.");
    mCurrentTimeline = PG_NEW(mAllocator, -1, "Timeline", Alloc::PG_MEM_PERM) Timeline(mAllocator, mAppContext);
    return mCurrentTimeline;
}

AssetLib::RuntimeAssetObjectRef TimelineManager::CreateRuntimeObject(const PegasusAssetTypeDesc* desc)
{
    if (desc->mTypeGuid == ASSET_TYPE_TIMELINE.mTypeGuid)
    {
        return CreateTimeline();
    }
    else if (desc->mTypeGuid == ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
    {
        return CreateScript();
    }
    else if (desc->mTypeGuid == ASSET_TYPE_H_BLOCKSCRIPT.mTypeGuid)
    {
        return CreateHeader();
    }
    
    return nullptr;
}

const PegasusAssetTypeDesc*const* TimelineManager::GetAssetTypes() const
{
    static const PegasusAssetTypeDesc* gDescs[] = {
          &ASSET_TYPE_TIMELINE
        , &ASSET_TYPE_BLOCKSCRIPT
        , &ASSET_TYPE_H_BLOCKSCRIPT
        , nullptr
    };

    return gDescs;
}

}   // namespace Timeline
}   // namespace Pegasus
