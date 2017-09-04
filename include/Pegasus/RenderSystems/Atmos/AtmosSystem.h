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
//#include "Pegasus/RenderSystems/Atmos/AtmosLookUpTable.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus 
{
namespace RenderSystems
{

class Atmosphere;


//! Deferred renderer system implementation. Adds a blockscript library.
class AtmosSystem : public RenderSystem
{
public:

	struct AtmosResources
	{
		float size;

	};

    enum Programs
    {
		GENERATE_ATMOS_LOOK_UP,
        PROGRAM_COUNT
    };

    //!Constants, these constants must match the 
    enum Constants {
    };


    //! Constructor
    explicit AtmosSystem(Alloc::IAllocator* allocator)
        : RenderSystem(allocator)
#if PEGASUS_ENABLE_PROXIES 
        , mEnableCamCullDebug(false)
        , mEnableGeoDebug(false)
#endif
    {
    }

    //! destructor
    virtual ~AtmosSystem() {}

    virtual void Load(Core::IApplicationContext* appContext);

    virtual bool CanCreateBlockScriptApi() const { return true; }

    virtual const char* GetSystemName() const { return "AtmosSystem"; } 

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);

    virtual void OnRegisterCustomMeshNodes(Mesh::MeshManager* meshManager);

    Shader::ProgramLinkageReturn GetProgram(Programs programEnum) { return mPrograms[programEnum]; }

    //! use for debugging, get the table with the marching cube patterns/cases.
    //const AtmosLookUpTable* GetLookUpTable() const { return &mLookUpTable; }

    //! creates the resources of the 3d terrain, including internal render targets / vertex buffers required.
    void CreateResources(AtmosResources& resources) const;

    //! renders and processes using the resources struct, outputs a vertex / index and normal buffer.
    void ComputeAtmosphereMesh(AtmosResources& resources, Render::BufferRef indexBuffer, Render::BufferRef vertexPosBuffer, Render::BufferRef vertexNormalBuffer, Render::BufferRef drawIndirectBufferArgs);

#if PEGASUS_ENABLE_PROXIES
    //! Updates debug state.
    void UpdateDebugState(bool enableDebugGeometry, bool enableDebugCamera)
    {
        mEnableGeoDebug = enableDebugGeometry;
        mEnableCamCullDebug = enableDebugCamera;
    }
    bool CamCullDebugEnable() const { return mEnableCamCullDebug; }
    void LoadDebugObjects(Core::IApplicationContext* appContext);
    void DrawDebugAtmosphere(Atmosphere* terrain);
#endif
    

private:

    //AtmosLookUpTable mLookUpTable;    
    Shader::ProgramLinkageRef mPrograms[PROGRAM_COUNT];
    Render::BufferRef mPackedRenderData;
    Render::SamplerStateRef  mPrecomputedAtmosSampler;

    //density inputs
    Render::Uniform mAtomsphereConstantUniform;

#if PEGASUS_ENABLE_PROXIES
    void RenderDebugBox(const Math::Vec3& minAabb, const Math::Vec3& maxAabb, const Math::Vec4& color);

	bool mEnableGeoDebug;
	bool mEnableCamCullDebug;

    Render::RasterizerStateRef mPreviousState;
    Render::BlendingStateRef mAlphaBlending;
    Render::BlendingStateRef mNoBlending;
#endif

};
}
}
#endif
#endif
