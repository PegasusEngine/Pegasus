/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineManager.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management

#ifndef PEGASUS_TIMELINE_TIMELINE_MGR_H
#define PEGASUS_TIMELINE_TIMELINE_MGR_H

#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/Proxy/TimelineManagerProxy.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Timeline/ScriptTracker.h"

namespace Pegasus {

    namespace Core {
        class IApplicationContext;

        namespace CompilerEvents {
            class ICompilerEventListener;
        }
    }

    namespace Timeline {
        class Block;
        class Lane;
        class Timeline;
    }

    namespace Wnd {
        class Window;
    }
    
    namespace AssetLib {
        class Asset;
    }
}

namespace Pegasus {
namespace Timeline {


//! Macro to register a Pegasus-side timeline block
//! \note Requires the external timelineManager variable to be defined, a pointer to the application timeline
//! \param className Name of the class to register, without quotes
#if PEGASUS_ENABLE_PROXIES

#define REGISTER_BASE_TIMELINE_BLOCK(className)                                                        \
    timelineManager->RegisterBlock(#className, className::GetStaticEditorString(), className::CreateBlock);   \

#else

#define REGISTER_BASE_TIMELINE_BLOCK(className)                    \
    timelineManager->RegisterBlock(#className, className::CreateBlock);   \

#endif  // PEGASUS_ENABLE_PROXIES
    

//! Macro to register a timeline block inside the Application class
//! \param className Name of the class to register, without quotes
#if PEGASUS_ENABLE_PROXIES

#define REGISTER_TIMELINE_BLOCK(className)                                                                  \
    GetTimelineManager()->RegisterBlock(#className, className::GetStaticEditorString(), className::CreateBlock);   \

#else

#define REGISTER_TIMELINE_BLOCK(className)                              \
    GetTimelineManager()->RegisterBlock(#className, className::CreateBlock);   \

#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

//! Timeline management, manages a set of blocks stored in lanes to sequence demo rendering
class TimelineManager
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    TimelineManager(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~TimelineManager();

    //! Get the allocator used for all timeline allocations
    inline Alloc::IAllocator * GetAllocator() const { return mAllocator; }


    //! Creation function type, to be defined once per timeline block class
    //! \param allocator Allocator used for block internal data
    //! \param appContext Application context, providing access to the global managers
    typedef Block * (* CreateBlockFunc)(Alloc::IAllocator * allocator,
                                        Core::IApplicationContext* appContext);

    //! Initializes all the blocks resources
    void InitializeAllTimelines();

    //! Shutdowns all the block resources
    void ShutdownAllTimelines();

#if PEGASUS_ENABLE_PROXIES

    //! Register a block class, to be called before any timeline block of this type is created
    //! \param className String of the block class (maximum length MAX_BLOCK_CLASS_NAME_LENGTH)
    //! \param editorString String for the editor (more readable than the class name), can be empty but != nullptr
    //! \param createBlockFunc Pointer to the block member function that instantiates the block
    //! \warning If the number of registered block classes reaches MAX_NUM_REGISTERED_BLOCKS,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of MAX_NUM_REGISTERED_BLOCKS
    //! \note Use the \a REGISTER_TIMELINE_BLOCK macro as convenience
    void RegisterBlock(const char * className, const char * editorString, CreateBlockFunc createBlockFunc);

    //! Get the list of registered block names (class and editor string)
    //! \param classNames Allocated 2D array of MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length MAX_BLOCK_CLASS_NAME_LENGTH + 1,
    //!                   containing the resulting class names
    //! \param editorStrings Allocated 2D array of MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
    //!                   containing the resulting editor strings (can be empty)
    //! \return Number of registered blocks (<= MAX_NUM_REGISTERED_BLOCKS)
    unsigned int GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH    + 1],
                                         char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_EDITOR_STRING_LENGTH + 1]) const;

#else

    //! Register a block class, to be called before any timeline block of this type is created
    //! \param className String of the block class (maximum length MAX_BLOCK_CLASS_NAME_LENGTH)
    //! \param createBlockFunc Pointer to the block member function that instantiates the block
    //! \warning If the number of registered block classes reaches MAX_NUM_REGISTERED_BLOCKS,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of MAX_NUM_REGISTERED_BLOCKS
    //! \note Use the \a REGISTER_TIMELINE_BLOCK macro as convenience
    void RegisterBlock(const char * className, CreateBlockFunc createBlockFunc);

#endif  // PEGASUS_ENABLE_PROXIES

    //! Create a block by class name
    //! \param className Name of the block class to instantiate
    //! \return Pointer to the created block, nullptr if an error occurred
    Block * CreateBlock(const char * className);

#if PEGASUS_USE_GRAPH_EVENTS
    //! Register a compiler event listener for blocks that have scripts and get compiled.
    //! \param eventListener - the listener to events during compilation
    void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener);

    //! Gets the event listener registered
    //! \return the event listener registered.
    Pegasus::Core::CompilerEvents::ICompilerEventListener* GetEventListener() const { return mEventListener; }
#endif

    //! Update the current state of the timeline based on the play mode and the current time
    //! \param musicPosition Currently heard position of the music (in milliseconds), 0 if unknown
    void Update(unsigned int musicPosition = 0);

    //! Test wether this asset is a script or not
    //! \return true if its a blockscript false otherwise
    bool IsTimelineScript(const AssetLib::Asset* asset) const;

    //! Loads a script from a file
    //! \param the file of the script name
    //! \return the timeline script reference
    TimelineScriptReturn LoadScript(const char* scriptName);

    //! Loads a header from a file
    //! \param the file of the script name
    //! \return the timeline script reference
    TimelineSourceReturn LoadHeader(const char* path);

    //! Loads a timeline from a file
    //! \return the timeline allocated. 
    Timeline* LoadTimeline(const char* filename);

    //! Creates a script from an asset
    //! \param the asset to use to create this script
    //! \return the timeline script reference
    TimelineScriptReturn CreateScript(AssetLib::Asset* asset);

    //! Creates a script from an asset
    //! \param the asset to use to create this script
    //! \return the timeline script reference
    TimelineSourceReturn CreateHeader(AssetLib::Asset* asset);

    //! Returns the current timeline in the playback state.
    //! \return the timeline in the current playback.
    Pegasus::Timeline::Timeline* GetCurrentTimeline() const { return mCurrentTimeline; }

    //! Gets the script tracker registered
    //! \return the script tracker
    ScriptTracker* GetScriptTracker() { return &mScriptTracker; }

    //! Creates a new timeline
    Timeline* CreateTimeline();

    //! Destroys a timeline
    //! \param timeline to destroy
    void DestroyTimeline(Timeline* timeline);

    //! Retrns true if this asset is of type Timeline, false otherwise
    //! \true if is a block, false otherwise
    bool IsTimeline(AssetLib::Asset* asset);

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the timeline
    //! \return Proxy associated with the timeline
    //@{
    inline TimelineManagerProxy * GetProxy() { return &mProxy; }
    inline const TimelineManagerProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    // The timeline cannot be copied
    PG_DISABLE_COPY(TimelineManager)

    //! Find a registered block by name
    //! \param className Name of the class of the block to find
    //! \return Index of the block in the \a mRegisteredBlocks array if found,
    //!         mNumRegisteredBlocks if not found
    unsigned int GetRegisteredBlockIndex(const char * className) const;


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Core::IApplicationContext* mAppContext;

    //! Structure describing one registered block class
    struct BlockEntry
    {
        char className[MAX_BLOCK_CLASS_NAME_LENGTH + 1];        //!< Name of the class
#if PEGASUS_ENABLE_PROXIES
        char editorString[MAX_BLOCK_EDITOR_STRING_LENGTH + 1];  //!< Editor string (more readable than the class name)
#endif
        CreateBlockFunc createBlockFunc;                        //!< Factory function of the block

        BlockEntry() { className[0] = '\0'; createBlockFunc = nullptr; }    //!< Default constructor }; 
    };

    //! List of registered blocks, only the first mNumRegisteredBlocks ones are valid
    BlockEntry mRegisteredBlocks[MAX_NUM_REGISTERED_BLOCKS];

    //! Number of currently registered blocks (<= MAX_NUM_REGISTERED_BLOCKS)
    unsigned int mNumRegisteredBlocks;

    //! TODO: temporary timeline singleton
    Timeline* mCurrentTimeline;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the timeline
    TimelineManagerProxy mProxy;

#endif  // PEGASUS_ENABLE_PROXIES

#if PEGASUS_USE_GRAPH_EVENTS
    //! Reference to the event listener
    Core::CompilerEvents::ICompilerEventListener* mEventListener;
#endif
    
    ScriptTracker mScriptTracker;

};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_TIMELINE_H
