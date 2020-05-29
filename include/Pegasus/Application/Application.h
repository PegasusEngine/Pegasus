/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Application.h
//! \author David Worsham
//! \date   04th July 2013
//! \brief  Building block class for a Pegasus application.
//!         Manages access to the Pegasus runtime.

#ifndef PEGASUS_APP_APPLICATION_H
#define PEGASUS_APP_APPLICATION_H

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        class AppWindowManager;
        class AppWindowComponentFactory;
    }

    namespace Wnd {
        struct WindowConfig;
        class Window;
    }

    namespace Render {
        class IDevice;
        class Context;
    }

    namespace Shader {
        class ShaderManager;
    }

    namespace Texture {
        class TextureManager;
    }
    
    namespace Mesh {
        class MeshManager;
    }

    namespace BlockScript {
        class BlockScriptManager;
        class BlockLib;
    }

    namespace AssetLib {
        class AssetLib;
    }

    namespace RenderSystems {
        class RenderSystemManager;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace App {

//! Building block class for a Pegasus application
//! Override Init, Shutdown, and Render to perform rendering.
//! \note To use this class, simply:
//! \note   1. instantiate it
//! \note   2. attach a window to it
//! \note   3. call Run to enter the application loop.
//! \todo Multi-application support
class Application : public Core::IApplicationContext
{
public:
    //! Constructor
    //! \param config Configuration struct for this Application.
    Application(const ApplicationConfig& config);

    //! Destructor
    virtual ~Application();


    //! Get the name of this app
    //! \return Application name.
    virtual const char* GetAppName() const = 0;

    //! Load the assets required to render the timeline blocks
    void Load();

    //! Unload the assets used by the timeline blocks
    void Unload();

    //! Updates the simulation / calls update on every block of this timeline.
    void Update();

    //! Attaches a new window to this app
    //! \param appWindowConfig Config structure for the window.
    //! \return The new window.
    //! \note Returns nullptr on failure.
    Wnd::Window* AttachWindow(const AppWindowConfig& appWindowConfig);

    //! Detaches a window from this app
    //! \param wnd The window to detach.
    void DetachWindow(Wnd::Window* wnd);

    //! Gets the application config.
    const ApplicationConfig& GetConfig() const { return mConfig; }

    // IWindowContext interface
    virtual Io::IOManager*                                  GetIOManager()               const { return mIoManager;               }
    virtual Graph::NodeManager*                             GetNodeManager()             const { return mNodeManager;             }
    virtual Texture::TextureManager*                        GetTextureManager()          const { return mTextureManager;          }
    virtual Mesh::MeshManager*                              GetMeshManager()             const { return mMeshManager;             }
    virtual Shader::ShaderManager*                          GetShaderManager()           const { return mShaderManager;           }
    virtual Timeline::TimelineManager*                      GetTimelineManager()         const { return mTimelineManager;         }
    virtual BlockScript::BlockScriptManager*                GetBlockScriptManager()      const { return mBlockScriptManager;      }
    virtual AssetLib::AssetLib*                             GetAssetLib()                const { return mAssetLib;                }
    virtual Pegasus::Application::RenderCollectionFactory*  GetRenderCollectionFactory() const { return mRenderCollectionFactory; }
    virtual PropertyGrid::PropertyGridManager*              GetPropertyGridManager()     const { return mPropertyGridManager;     }
    virtual RenderSystems::RenderSystemManager*             GetRenderSystemManager()     const { return mRenderSystemManager;     }
    virtual BlockScript::BlockLib*                          GetRenderBsApi()             const { return mRenderApiScript;         }
    virtual Render::IDevice*                                GetRenderDevice()            const { return mDevice;                  }
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    //! Returns the reflection information from blockscript.
    virtual App::AppBsReflectionInfo*                       GetBsReflectionInfo() const { return mBsReflectionInfo; }
#endif

    AppWindowManager*                                       GetWindowManager() const { return mWindowManager; }
protected:
    
    //! Custom initialization, done in the user application before the timeline triggers the loading of blocks and their assets
    virtual void InitializeApp() = 0;

    //! Custom shutdown, done in the user application
    virtual void ShutdownApp() = 0;

    //! Optional custom function, utilize this function to register any custom system that the app might want to register
    virtual void RegisterCustomRenderSystems(RenderSystems::RenderSystemManager* renderSystemManager) {}

private:
    // No copies allowed
    PG_DISABLE_COPY(Application);
    
    ApplicationConfig                               mConfig;                 //!< Cached config object
    Render::IDevice*                                mDevice;                 //!< Render device
    Render::Context*                                mRenderContext;          //!< Rendering context
    AppWindowManager*                               mWindowManager;          //!< Window manager
    Io::IOManager*                                  mIoManager;              //!< IO manager
    Graph::NodeManager*                             mNodeManager;            //!< Graph node manager
    Shader::ShaderManager*                          mShaderManager;          //!< Shader node manager
    Texture::TextureManager*                        mTextureManager;         //!< Texture node manager
    Mesh::MeshManager*                              mMeshManager;            //!< Mesh node manager
    Timeline::TimelineManager*                      mTimelineManager;        //!< Timeline manager
    BlockScript::BlockScriptManager*                mBlockScriptManager;     //!< BlockScriptManager manager.
    AssetLib::AssetLib*                             mAssetLib;               //!< AssetLib manager    
    PropertyGrid::PropertyGridManager*              mPropertyGridManager;    //!< Property grid manager
    RenderSystems::RenderSystemManager*             mRenderSystemManager;    //!< Render systems manager. Used to instantiate custom systems.
    Pegasus::Application::RenderCollectionFactory*  mRenderCollectionFactory;//!< Render collection factory
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    App::AppBsReflectionInfo*                       mBsReflectionInfo; //!< Reflection and library info of blockscript libraries registered
#endif

    //Reference to the render api blockscript lib
    BlockScript::BlockLib* mRenderApiScript;
};

}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_APP_APPLICATION_H
