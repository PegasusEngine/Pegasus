/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.cpp
//! \author	Karolyn Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"

namespace Pegasus {
namespace Timeline {

BEGIN_IMPLEMENT_PROPERTIES(Block)
    IMPLEMENT_PROPERTY(Block, Color)
    IMPLEMENT_PROPERTY(Block, Beat)
    IMPLEMENT_PROPERTY(Block, Duration)
END_IMPLEMENT_PROPERTIES(Block)

Block::Block(Alloc::IAllocator * allocator, Core::IApplicationContext * appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mLane(nullptr)
,   mScriptRunner(allocator, appContext, this
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    ,&mCategory
#endif
    )
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif  // PEGASUS_ENABLE_PROXIES
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to a timeline Block object");
    
    BEGIN_INIT_PROPERTIES(Block)
        INIT_PROPERTY(Color)
        INIT_PROPERTY(Beat)
        INIT_PROPERTY(Duration)
    END_INIT_PROPERTIES()
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    static unsigned sNextGuid = 0;
    mGuid = sNextGuid++;
    mCategory.SetUserData(mGuid);
#endif
}

//----------------------------------------------------------------------------------------

Block::~Block()
{
}

//----------------------------------------------------------------------------------------

void Block::AttachScript(TimelineScriptInOut script)
{
    mScriptRunner.AttachScript(script);
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    mScriptRunner.ShutdownScript();
}

//----------------------------------------------------------------------------------------

TimelineScriptReturn Block::GetScript()
{
    return mScriptRunner.GetScript();
}

//----------------------------------------------------------------------------------------

void Block::Update(float beat)
{
    mScriptRunner.CallUpdate(beat);
}

//----------------------------------------------------------------------------------------

void Block::Render(float beat, Wnd::Window * window)
{
    mScriptRunner.CallRender(beat, window);
}

//----------------------------------------------------------------------------------------

void Block::Initialize()
{
    //! don't use asset categories because these are set externally already.
    mScriptRunner.InitializeScript(false);
}

//----------------------------------------------------------------------------------------

void Block::Shutdown()
{
    mScriptRunner.ShutdownScript();
}

//----------------------------------------------------------------------------------------

void Block::SetLane(Lane * lane)
{
    PG_ASSERTSTR(lane != nullptr, "Invalid lane associated with a block");
    PG_ASSERTSTR(lane->GetTimeline() != nullptr, "The lane must belong to a timeline in order for this block to be part of it");
    mLane = lane;

    //link the script runner with the proper global cache from the timeline
    mScriptRunner.SetGlobalCache(lane->GetTimeline()->GetGlobalCache());
}

bool Block::OnReadObject(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* owner, AssetLib::Object* root) 
{
    int typeId = root->FindString("type");
    if (typeId == -1 || Utils::Strcmp(root->GetString(typeId), GetClassName()))
    {
        return false;
    }

    int propsId = root->FindObject("props");
    if (propsId != -1)
    {
        AssetLib::Object* propsObj = root->GetObject(propsId);
        PropertyGrid::PropertyGridObject::ReadFromObject(owner, propsObj);

        int scriptId = propsObj->FindAsset("script");
        if (scriptId != -1 && propsObj->GetAsset(scriptId)->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
        {
            TimelineScriptRef script = static_cast<TimelineScript*>(&(*propsObj->GetAsset(scriptId)));
            if (script != nullptr)
            {
                AttachScript(script);
            }
        }
    }
    else
    {
        return false;
    }


    return true;

}

void Block::OnWriteObject(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* owner, AssetLib::Object* root) 
{
    root->AddString("type", GetClassName());
    AssetLib::Object* prop = owner->NewObject();
    root->AddObject("props", prop);
    if (GetScript() != nullptr && GetScript()->GetOwnerAsset() != nullptr)
    {
        prop->AddAsset("script", GetScript());
    }
    
    PropertyGrid::PropertyGridObject::WriteToObject(owner, prop);
}

}   // namespace Timeline
}   // namespace Pegasus
