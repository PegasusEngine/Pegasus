/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScriptRunner.cpp
//! \author	Kleber Garcia
//! \date	24th May 2016
//! \brief	Script runner, with state and coordination of Vm / compilation

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/Timeline/TimelineScriptRunner.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/PropertyGrid/Shared/PropertyEventDefs.h"
#include "Pegasus/Application/RenderCollection.h"

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
#include "Pegasus/AssetLib/Category.h"
#endif

namespace Pegasus
{
namespace Timeline
{


    TimelineScriptRunner::TimelineScriptRunner(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext, PropertyGrid::PropertyGridObject* propGrid
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
        ,AssetLib::Category* category
#endif
    )
    : mAllocator(allocator)
    , mAppContext(appContext)
    , mPropertyGrid(propGrid)
#if PEGASUS_ENABLE_PROXIES
    , mBlockScriptObserver(this)
#endif
    , mVmState(nullptr)
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    , mCategory(category)
#endif
    {
    }

    TimelineScriptRunner::~TimelineScriptRunner()
    {
#if PEGASUS_ENABLE_PROXIES
        if (mTimelineScript != nullptr)
        {
            mTimelineScript->CallGlobalScopeDestroy(mVmState);
            mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
            mTimelineScript = nullptr;
            mRuntimeListener.Shutdown();

            if (mVmState->GetUserContext() != nullptr)
            {
                Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
                mAppContext->GetRenderCollectionFactory()->DeleteRenderCollection(userCtx);
            }
            PG_DELETE(mAllocator, mVmState);
            mVmState = nullptr;
        }
#endif
    }

    
    void TimelineScriptRunner::AttachScript(TimelineScriptInOut script)
    {
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
        mCategory->RegisterAsset(script->GetOwnerAsset());
#endif
        if (mTimelineScript == nullptr)
        {
            mTimelineScript = script;

            if (mVmState == nullptr)
            {
                mVmState = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) BlockScript::BsVmState();
                mVmState->Initialize(mAllocator);
                Application::RenderCollection* userContext = mAppContext->GetRenderCollectionFactory()->CreateRenderCollection();
                mVmState->SetUserContext(userContext);
            }
        }
        else
        {
            UninitializeScript();
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
        mRuntimeListener.Initialize(mPropertyGrid, mTimelineScript->GetBlockScript());
        script->Compile();
        InitializeScript();
    }

    void TimelineScriptRunner::ShutdownScript()
    {
        if (mTimelineScript != nullptr)
        {
#if PEGASUS_ENABLE_PROXIES
            mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
#endif
            if (mVmState != nullptr)
            {
                if (mVmState->GetUserContext() != nullptr)
                {
                    Application::RenderCollection* nodeContaier = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
                    nodeContaier->Clean();
                }
                mVmState->Reset();
            }
            mTimelineScript = nullptr;
        }
    
        mRuntimeListener.Shutdown();
    }

    void TimelineScriptRunner::InitializeScript()
    {
        if (HasScript() && mScriptVersion != mTimelineScript->GetSerialVersion() && mTimelineScript->IsScriptActive())
        {
            mScriptVersion = mTimelineScript->GetSerialVersion();

            if (mVmState->GetUserContext() != nullptr)
            {
                Application::RenderCollection* nodeContaier = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
                nodeContaier->Clean();
            }
            mVmState->Reset();

            //just listen for runtime events on the global scope initialization
            mVmState->SetRuntimeListener(&mRuntimeListener);
            //re-initialize everything!
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
            mCategory->RemoveAssets();
            mAppContext->GetAssetLib()->BeginCategory(mCategory);
#endif
            mTimelineScript->CallGlobalScopeInit(mVmState); 

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
            mAppContext->GetAssetLib()->EndCategory();
#endif

            // remove the listener. No need to listen for more events.
            mVmState->SetRuntimeListener(nullptr);
        }
    }

    void TimelineScriptRunner::UninitializeScript()
    {
        //TODO: remove this global scope destroy stuff
        if (mTimelineScript != nullptr && mTimelineScript->IsDirty())
        {
            mTimelineScript->CallGlobalScopeDestroy(mVmState);
        }
    }

    void TimelineScriptRunner::NotifyInternalObjectPropertyUpdated(unsigned int index)
    {
        if (mTimelineScript != nullptr && mTimelineScript->IsScriptActive())
        {
            BlockRuntimeScriptListener::UpdateType updateType = mRuntimeListener.FlushProperty(*mVmState, index);
            if (updateType == BlockRuntimeScriptListener::RERUN_GLOBALS)
            {
                mScriptVersion = -1; //invalidate the script version, only when running globals
            }
            PEGASUS_EVENT_DISPATCH(mPropertyGrid, Pegasus::PropertyGrid::PropertyGridRenderRequest);
        }
    }

    void TimelineScriptRunner::CallUpdate(float beat, Wnd::Window * window)
    {
        if (mTimelineScript != nullptr)
        {
            InitializeScript(); //in case a dirty compilation has been carried on.
            Application::RenderCollection* nodeContainer = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
            nodeContainer->SetWindow(window);
            mTimelineScript->CallUpdate(beat, mVmState);
        }
    }

    void TimelineScriptRunner::CallRender(float beat, Wnd::Window * window)
    {
        if (mTimelineScript != nullptr)
        {
            Application::RenderCollection* nodeContainer = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
            nodeContainer->SetWindow(window);
            mTimelineScript->CallRender(beat, mVmState);
        }
    }


#if PEGASUS_ENABLE_PROXIES
    void TimelineScriptRunner::BlockScriptObserver::OnCompilationBegin()
    {
        //try to initialize the script. Compile wont call this observer stuff again since it is not dirty.
        mRunner->UninitializeScript();
    }
    void TimelineScriptRunner::BlockScriptObserver::OnCompilationEnd()
    {
        //try to initialize the script. Compile wont call this observer stuff again since it is not dirty.
        mRunner->InitializeScript();
    }
#endif

}
}