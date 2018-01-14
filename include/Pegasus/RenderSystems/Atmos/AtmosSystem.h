/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   3dTerrainSystem.h
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  3dTerrain system definition file

#ifndef PEGASUS_RENDER_SYSTEM_ATMOSPHERE_H
#define PEGASUS_RENDER_SYSTEM_ATMOSPHERE_H
#include "Pegasus\RenderSystems\Config.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

#include "Pegasus/RenderSystems/System/RenderSystem.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/RenderSystems/Camera/Camera.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
    namespace Texture
    {
        class TextureManager;
    }
}

namespace Pegasus 
{
namespace RenderSystems
{

class Atmosphere;
class BasicSky;

//! Deferred renderer system implementation. Adds a blockscript library.
class AtmosSystem : public RenderSystem
{
public:
    //! Constructor
    explicit AtmosSystem(Alloc::IAllocator* allocator)
        : RenderSystem(allocator)
    {
    }

    //! destructor
    virtual ~AtmosSystem() {}

    virtual void Load(Core::IApplicationContext* appContext);

    virtual bool CanCreateBlockScriptApi() const { return true; }

    virtual const char* GetSystemName() const { return "AtmosSystem"; } 

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);

    void DrawBasicSky(BasicSky* basicSky, bool enableStencil);

    Camera::CameraRef GetCubeCam(Render::CubeFace face) { return mCubeCams[face]; }

private:

    enum Programs
    {
        BASIC_SKY,
        PROGRAMS_COUNT
    };

    void GenerateAcosLut(Pegasus::Texture::TextureManager* textureManager);
    void GenerateCubeCams();

    Mesh::MeshRef mQuadMesh;
    Shader::ProgramLinkageRef mPrograms[PROGRAMS_COUNT];
    Render::RasterizerStateRef mSkyRasterState;
    Render::RasterizerStateRef mSkyRasterStateNoStencil;
    Texture::TextureRef mAcosLut;
    Render::SamplerStateRef mBilinearFilter;
    Camera::CameraRef mCubeCams[Render::CUBE_FACE_COUNT];

    //Basic sky resources
    struct BasicSkyCbuffer {
        float distance; 
        float unused0;
        float unused1;
        float unused2;
    };

    Render::BufferRef mBasicSkyCbuffer;
    Render::Uniform mBasicSkyCbufferUniform;
    Render::Uniform mBasicSkyAcosLutUniform;
};

}
}
#endif
#endif
