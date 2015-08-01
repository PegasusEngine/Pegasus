/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.cpp
//! \author	Kevin Boulanger
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


Block::Block(Alloc::IAllocator * allocator, Core::IApplicationContext * appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mBeat(0)
,   mDuration(1)
,   mLane(nullptr)
,   mTimelineScript(nullptr)
,   mVmState(nullptr)
,   mScriptVersion(-1)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mColorRed(128)
,   mColorGreen(128)
,   mColorBlue(128)
#endif  // PEGASUS_ENABLE_PROXIES
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to a timeline Block object");
}

//----------------------------------------------------------------------------------------

Block::~Block()
{

}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

void Block::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    mColorRed = red;
    mColorGreen = green;
    mColorBlue = blue;
}

//----------------------------------------------------------------------------------------

void Block::GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const
{
    red = mColorRed;
    green = mColorGreen;
    blue = mColorBlue;
}

#endif  // PEGASUS_ENABLE_PROXIES

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

void Block::SetBeat(Beat beat)
{
    mBeat = beat;
}

bool Block::InitializeScript()
{
    bool didChangeRuntimeLayout = false;
    
    //TODO: remove this global scope destroy stuff
    if (mTimelineScript->IsDirty())
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
    }

    mTimelineScript->Compile();

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
        mTimelineScript->CallGlobalScopeInit(mVmState);     

        didChangeRuntimeLayout = didChangeRuntimeLayout || true;
    }
    return didChangeRuntimeLayout;
}

void Block::UpdateViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        if (InitializeScript())// attempt to initialize if script was modified
        {
            TimelineScript* rawptr = &(*mTimelineScript);
            //Throw an event to the UI
            GRAPH_EVENT_DISPATCH(
                rawptr,
                Core::CompilerEvents::CompilationNotification,
                //Event that notifies that the runtime has changed, potentially changing layout of nodes and such.
                // This means that some views will require to be updated
                Core::CompilerEvents::CompilationNotification::COMPILATION_RUNTIME_INITIALIZATION, 0, ""
            );
        }
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

void Block::AttachScript(TimelineScriptIn script)
{
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
        mTimelineScript = script;
        Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
        if (userCtx != nullptr)
        {
            userCtx->Clean();
        }
        mVmState->Reset();
    }
    
    mScriptVersion = -1;  
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    mTimelineScript = nullptr;
}

//----------------------------------------------------------------------------------------

void Block::SetDuration(Duration duration)
{
    if (duration == 0)
    {
        mDuration = 1;
        PG_FAILSTR("Invalid duration given to the block (%u), setting it to 1 tick", duration);
    }
    else
    {
        mDuration = duration;
    }
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

    int propsId = root->FindObject("properties");
    if (propsId != -1)
    {
        AssetLib::Object* propsObj = root->GetObject(propsId);
        int beatsId = propsObj->FindInt("Beat");
        int durationId = propsObj->FindInt("Duration");
        if (beatsId == -1 || durationId == -1)
        {
            return false;
        }

        mBeat = static_cast<Beat>(propsObj->GetInt(beatsId));
        mDuration = static_cast<Duration>(propsObj->GetInt(durationId));

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

#if PEGASUS_ENABLE_PROXIES
    int editorPropsId = root->FindObject("editor-props");
    if (editorPropsId != -1)
    {
        AssetLib::Object* editorProps = root->GetObject(editorPropsId);
        int colId = editorProps->FindInt("color");
        if (colId != -1)
        {
            unsigned int encodedCol = static_cast<unsigned int>(editorProps->GetInt(colId));
            mColorRed =   static_cast<char>(encodedCol   & 0x000000FF);
            mColorGreen = static_cast<char>((encodedCol & 0x0000FF00) >> 8);
            mColorBlue =  static_cast<char>((encodedCol  & 0x00FF0000) >> 16);
        }
    }
#endif

    return true;

}

void Block::OnWriteObject(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* owner, AssetLib::Object* root) 
{
    root->AddString("type", GetClassName());
    AssetLib::Object* properties = owner->NewObject();
    root->AddObject("properties", properties);
    properties->AddInt("Beat", static_cast<int>(mBeat));
    properties->AddInt("Duration", static_cast<int>(mDuration));
    if (mTimelineScript != nullptr && mTimelineScript->GetOwnerAsset() != nullptr)
    {
        properties->AddString("script", mTimelineScript->GetOwnerAsset()->GetPath());
    }
    
#if PEGASUS_ENABLE_PROXIES
    AssetLib::Object* editorProps = owner->NewObject();
    root->AddObject("editor-props", editorProps);
    //encode the color
    unsigned int col = mColorRed + (mColorGreen << 8) + (mColorBlue << 16);
    editorProps->AddInt("color", static_cast<int>(col));
#endif
}

}   // namespace Timeline
}   // namespace Pegasus
