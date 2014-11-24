/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.h
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#ifndef PEGASUS_TIMELINE_BLOCK_H
#define PEGASUS_TIMELINE_BLOCK_H

#include "Pegasus/Timeline/Shared/BlockDefs.h"
#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/ScriptHelper.h"
#include "Pegasus/Window/IWindowContext.h"

namespace Pegasus {
    namespace Timeline {
        class Lane;
    }

    namespace Wnd {
        class Window;
    }

    namespace BlockScript {
        class BlockScript;
    }
}

namespace Pegasus {
namespace Timeline {


//! Timeline block, describing the instance of an effect on the timeline
class Block
{
    DECLARE_TIMELINE_BLOCK(Block, "Block");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    Block(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~Block();


    //! Get the position of the block in the lane
    //! \return Position of the block, measured in ticks
    inline Beat GetBeat() const { return mBeat; }

    //! Get the duration of the block
    //! \return Duration of the block, measured in ticks (> 0)
    inline Duration GetDuration() const { return mDuration; }

    //! Get the lane the block belongs to
    //! \return Lane the block belongs to, nullptr when the block is not associated with a lane yet
    inline Lane * GetLane() const { return mLane; }

    //! Attempts to open and compile a script. True if success, false otherwise.
    //! \param scriptFileName - the file name to open. 
    //! \return true if successful, false otherwise
    bool OpenScript(const char* scriptFileName);

    //! Attempts to shutdown a script if it has been opened
    void ShutdownScript();

    //! Returns the script of this block, null if none is attached.
    //! \return the script object, null if not attached
    ScriptHelper* GetScript() { return mScriptHelper; }


#if PEGASUS_ENABLE_PROXIES

    //! Set the color of the block for the editor
    //! \param red Red component (0-255)
    //! \param green Green component (0-255)
    //! \param blue Blue component (0-255)
    void SetColor(unsigned char red, unsigned char green, unsigned char blue);

    //! Get the color of the block for the editor
    void GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const;


    //! Get the proxy associated with the block
    //! \return Proxy associated with the block
    //@{
    inline BlockProxy * GetProxy() { return &mProxy; }
    inline const BlockProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

public:

    // Functions called in derived classes

    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Update the content of the block, called once at the beginning of each rendered frame
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Update(float beat, Wnd::Window * window) { UpdateViaScript(beat, window); };

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Render(float beat, Wnd::Window * window) { RenderViaScript(beat, window); };

    //------------------------------------------------------------------------------------

protected:

    //! Update the content of the block by calling the script callback within.
    //! \note if the script does not implement an "int Timeline_Update(beat : float)" function, 
    //!        then update becomes a NOP for this block.
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    void UpdateViaScript(float beat, Wnd::Window * window);

    //! Render the content of the block by calling the script callback within
    //! \note if the script does not implement a "int Timeline_Render(beat : float)" function, 
    //!        then render becomes a NOP for this block.
    //! \param beat the beat
    //! \param window Window in which the lane is being rendered
    void RenderViaScript(float beat, Wnd::Window * window);

    // Accessors to the global manager for the derived classes
    
    //! Get the global IO manager
    //! \return Global IO manager
    inline Io::IOManager * GetIOManager() const { return mAppContext->GetIOManager(); }

    //! Get the node manager
    //! \return Global node manager
    inline Graph::NodeManager * GetNodeManager() const { return mAppContext->GetNodeManager(); }

    //! Get the shader manager
    //! \return Global shader manager
    inline Shader::ShaderManager * GetShaderManager() const { return mAppContext->GetShaderManager(); }

    //! Get the texture manager
    //! \return Global texture manager
    inline Texture::TextureManager * GetTextureManager() const { return mAppContext->GetTextureManager(); }

    //! Get the mesh manager
    //! \return Global mesh manager
    inline Mesh::MeshManager * GetMeshManager() const { return mAppContext->GetMeshManager(); }

    //! Get the timeline
    //! \return Global timeline
    inline Timeline * GetTimeline() const { return mAppContext->GetTimeline(); }

    //------------------------------------------------------------------------------------

private:

    // Functions accessible only from the Lane object, to prevent the sorted linked list
    // of the owner lane to lose its sorting
    friend class Lane;

    //! Set the position of the block in the lane
    //! \param beat Position of the block, measured in ticks
    void SetBeat(Beat beat);

    //! Set the duration of the block
    //! \param duration Duration of the block, measured in ticks (> 0)
    void SetDuration(Duration duration);

    //! Set the lane the block belongs to
    //! \param lane Lane the block belongs to (!= nullptr)
    void SetLane(Lane * lane);

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(Block)


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Wnd::IWindowContext * mAppContext;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the block
    BlockProxy mProxy;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Position of the block in the lane, measured in ticks
    Beat mBeat;

    //! Duration of the block, measured in ticks (> 0)
    Duration mDuration;

    //! script helper object
    ScriptHelper* mScriptHelper;

    //! virtual machine state
    BlockScript::BsVmState* mVmState;

    //! version of the script, used for global variable initialization
    int mScriptVersion;

    //! Lane the block belongs to, nullptr when the block is not associated with a lane yet
    Lane * mLane;

#if PEGASUS_ENABLE_PROXIES

    //! Red component of the color for the editor (0-255)
    unsigned char mColorRed;

    //! Green component of the color for the editor (0-255)
    unsigned char mColorGreen;

    //! Blue component of the color for the editor (0-255)
    unsigned char mColorBlue;

#endif  // PEGASUS_ENABLE_PROXIES
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_BLOCK_H
