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
#include "Pegasus/Timeline/ScriptTracker.h"
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
,   mTimelineScript(nullptr)
,   mVmState(nullptr)
,   mScriptVersion(-1)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mBlockScriptObserver(this)
#endif  // PEGASUS_ENABLE_PROXIES
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to a timeline Block object");
    
    BEGIN_INIT_PROPERTIES(Block)
        INIT_PROPERTY(Color)
        INIT_PROPERTY(Beat)
        INIT_PROPERTY(Duration)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

Block::~Block()
{
#if PEGASUS_ENABLE_PROXIES
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
    }
#endif
}

//----------------------------------------------------------------------------------------

void Block::Initialize()
{
    // No default implementation
}

//----------------------------------------------------------------------------------------

void Block::Shutdown()
{
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
        mAppContext->GetTimelineManager()->GetScriptTracker()->UnregisterScript(mTimelineScript);
        mTimelineScript = nullptr;

        if (mVmState->GetUserContext() != nullptr)
        {
            Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
            mAppContext->GetRenderCollectionFactory()->DeleteRenderCollection(userCtx);
        }
        PG_DELETE(mAllocator, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::InitializeScript()
{
    if (mScriptVersion != mTimelineScript->GetSerialVersion())
    {
        mScriptVersion = mTimelineScript->GetSerialVersion();

        if (mVmState->GetUserContext() != nullptr)
        {
            Application::RenderCollection* nodeContaier = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
            nodeContaier->Clean();
        }
        mVmState->Reset();

        //re-initialize everything!
        mTimelineScript->CallGlobalScopeInit(mVmState, this);     
    }
}

void Block::UninitializeScript()
{
    //TODO: remove this global scope destroy stuff
    if (mTimelineScript != nullptr && mTimelineScript->IsDirty())
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
    }
}

void Block::UpdateViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        Application::RenderCollection* nodeContainer = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
        nodeContainer->SetWindow(window);
        mTimelineScript->CallUpdate(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::RenderViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->CallRender(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::AttachScript(TimelineScriptInOut script)
{
    //! Remove compile out of this equation? is this the right place to compile?
    script->Compile();

    if (mTimelineScript == nullptr)
    {
        mTimelineScript = script;

        PG_ASSERT(mVmState == nullptr);
        mVmState = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) BlockScript::BsVmState();
        mVmState->Initialize(mAllocator);

        Application::RenderCollection* userContext = mAppContext->GetRenderCollectionFactory()->CreateRenderCollection();
        mVmState->SetUserContext(userContext);
    }
    else
    {
#if PEGASUS_ENABLE_PROXIES
        mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
#endif
        mTimelineScript = script;
        Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
        if (userCtx != nullptr)
        {
            userCtx->Clean();
        }
        mVmState->Reset();
    }       
#if PEGASUS_ENABLE_PROXIES
    script->RegisterObserver(&mBlockScriptObserver);
#endif
    mScriptVersion = -1;  
    InitializeScript();
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    mTimelineScript = nullptr;
}

//----------------------------------------------------------------------------------------

void Block::SetLane(Lane * lane)
{
    PG_ASSERTSTR(lane != nullptr, "Invalid lane associated with a block");
    mLane = lane;
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

        int scriptId = propsObj->FindString("script");
        if (scriptId != -1)
        {
            TimelineScriptRef script = mAppContext->GetTimelineManager()->LoadScript(propsObj->GetString(scriptId));
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
    if (mTimelineScript != nullptr && mTimelineScript->GetOwnerAsset() != nullptr)
    {
        prop->AddString("script", mTimelineScript->GetOwnerAsset()->GetPath());
    }
    
    PropertyGrid::PropertyGridObject::WriteToObject(owner, prop);
}

#if PEGASUS_ENABLE_PROXIES
void Block::BlockScriptObserver::OnCompilationBegin()
{
    //try to initialize the script. Compile wont call this observer stuff again since it is not dirty.
    mBlock->UninitializeScript();
}
void Block::BlockScriptObserver::OnCompilationEnd()
{
    //try to initialize the script. Compile wont call this observer stuff again since it is not dirty.
    mBlock->InitializeScript();
}
#endif

}   // namespace Timeline
}   // namespace Pegasus
