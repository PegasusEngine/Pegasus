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
#include "Pegasus/Application/IWindowRegistry.h"
#include "Pegasus/Core/IApplicationContext.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        class AppWindowManager;
    }

    namespace Wnd {
        struct WindowConfig;
        class Window;
    }

    namespace Render {
        class IDevice;
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
    }

    namespace AssetLib {
        class AssetLib;
    }

    namespace Camera {
        class CameraManager;
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

    //! Register the custom timeline blocks in the user application
    virtual void RegisterTimelineBlocks() = 0;

    //! Custom initialization, done in the user application before the timeline triggers the loading of blocks and their assets
    virtual void InitializeApp() = 0;

    //! Custom shutdown, done in the user application
    virtual void ShutdownApp() = 0;


    //! Initializes this app
    void Initialize();

    //! Shut this app down
    void Shutdown();

    //! Load the assets required to render the timeline blocks
    void Load();

    //! Unload the assets used by the timeline blocks
    void Unload();


    //! Gets the window registry for this app, to register window types
    //! \return Window registry interface.
    IWindowRegistry* GetWindowRegistry();

    //! Attaches a new window to this app
    //! \param appWindowConfig Config structure for the window.
    //! \return The new window.
    //! \note Returns nullptr on failure.
    Wnd::Window* AttachWindow(const AppWindowConfig& appWindowConfig);

    //! Detaches a window from this app
    //! \param wnd The window to detach.
    void DetachWindow(Wnd::Window* wnd);


    // IWindowContext interface
    virtual Io::IOManager*                                  GetIOManager()               const { return mIoManager;               }
    virtual Graph::NodeManager*                             GetNodeManager()             const { return mNodeManager;             }
    virtual Texture::TextureManager*                        GetTextureManager()          const { return mTextureManager;          }
    virtual Mesh::MeshManager*                              GetMeshManager()             const { return mMeshManager;             }
    virtual Shader::ShaderManager*                          GetShaderManager()           const { return mShaderManager;           }
    virtual Timeline::TimelineManager*                      GetTimelineManager()         const { return mTimelineManager;         }
    virtual BlockScript::BlockScriptManager*                GetBlockScriptManager()      const { return mBlockScriptManager;      }
    virtual AssetLib::AssetLib*                             GetAssetLib()                const { return mAssetLib;                }
    virtual Camera::CameraManager*                          GetCameraManager()           const { return mCameraManager;           }
    virtual Pegasus::Application::RenderCollectionFactory*  GetRenderCollectionFactory() const { return mRenderCollectionFactory; }
    virtual PropertyGrid::PropertyGridManager*              GetPropertyGridManager()     const { return mPropertyGridManager;     }
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    //! Returns the reflection information from blockscript.
    virtual App::AppBsReflectionInfo*                       GetBsReflectionInfo() const { return mBsReflectionInfo; }
#endif

private:
    // No copies allowed
    PG_DISABLE_COPY(Application);

    //! Internal helper to start up the app
    void StartupAppInternal();

    //! Internal handler to shutdown the app
    void ShutdownAppInternal();

    //! Creates and initializes asset factories and type ids
    void RegisterAssetLib();

    bool mInitialized;                                                       //!< Initialized flag
    ApplicationConfig                               mConfig;                 //!< Cached config object
    Render::IDevice*                                mDevice;                 //!< Render device
    AppWindowManager*                               mWindowManager;          //!< Window manager
    Io::IOManager*                                  mIoManager;              //!< IO manager
    Graph::NodeManager*                             mNodeManager;            //!< Graph node manager
    Shader::ShaderManager*                          mShaderManager;          //!< Shader node manager
    Texture::TextureManager*                        mTextureManager;         //!< Texture node manager
    Mesh::MeshManager*                              mMeshManager;            //!< Mesh node manager
    Timeline::TimelineManager*                      mTimelineManager;        //!< Timeline manager
    BlockScript::BlockScriptManager*                mBlockScriptManager;     //!< BlockScriptManager manager.
    AssetLib::AssetLib*                             mAssetLib;               //!< AssetLib manager
    Camera::CameraManager*                          mCameraManager;          //!< Camera manager
    PropertyGrid::PropertyGridManager*              mPropertyGridManager;    //!< Property grid manager
    Pegasus::Application::RenderCollectionFactory*  mRenderCollectionFactory;//!< Render collection factory
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    App::AppBsReflectionInfo*                       mBsReflectionInfo; //!< Reflection and library info of blockscript libraries registered
#endif
};

}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_APP_APPLICATION_H
