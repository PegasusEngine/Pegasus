/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   3dTerrainSystem.h
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  3dTerrain system definition file

#ifndef PEGASUS_RENDER_SYSTEM_3DTERRAIN_H
#define PEGASUS_RENDER_SYSTEM_3DTERRAIN_H
#include "Pegasus\RenderSystems\Config.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

#include "Pegasus/RenderSystems/System/RenderSystem.h"
#include "Pegasus/RenderSystems/3dTerrain/CaseTable.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Render/Render.h"

namespace Pegasus 
{
namespace RenderSystems
{

//! Deferred renderer system implementation. Adds a blockscript library.
class Terrain3dSystem : public RenderSystem
{
public:
    struct TerrainResources
    {
        Render::VolumeTextureRef densityTexture;
        Render::VolumeTextureRef geoInfo;
        Render::VolumeTextureRef sparse8;
        Render::VolumeTextureRef sparse4;
    };

    enum Programs
    {
        DENSITY_CS,        
        SPARSE_2_8_CS,
        SPARSE_1_4_CS,
        GEOMETRY_INFO_CS,
        MESH_PRODUCER_CS,
        PROGRAM_COUNT
    };


    //! Constructor
    explicit Terrain3dSystem (Alloc::IAllocator* allocator) : RenderSystem(allocator) {}

    //! destructor
    virtual ~Terrain3dSystem () {}

    virtual void Load(Core::IApplicationContext* appContext);

    virtual bool CanCreateBlockScriptApi() const { return true; }

    virtual const char* GetSystemName() const { return "3dTerrainSystem"; } 

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);

    virtual void OnRegisterCustomMeshNodes(Mesh::MeshManager* meshManager);

    Shader::ProgramLinkageReturn GetProgram(Programs programEnum) { return mPrograms[programEnum]; }

    //! use for debugging, get the table with the marching cube patterns/cases.
    const CaseTable* GetCaseTable() const { return &mCaseTable; }

    //! gets the vertex count for a block of terrain.
    int GetVertexCount() const;
    
    //! gets the target index count for a block of terrain.
    int GetIndexCount() const;

    //! creates the resources of the 3d terrain, including internal render targets / vertex buffers required.
    void CreateResources(TerrainResources& resources) const;

    //! renders and processes using the resources struct, outputs a vertex / index and normal buffer.
    void RenderTerrain(TerrainResources& resources, Render::BufferRef indexBuffer, Render::BufferRef vertexPosBuffer, Render::BufferRef vertexNormalBuffer, Render::BufferRef drawIndirectBufferArgs);
    

private:

    CaseTable mCaseTable;    
    Shader::ProgramLinkageRef mPrograms[PROGRAM_COUNT];
    Render::BufferRef mPackedCaseCountInfoBuffer;
    Render::BufferRef mPackedIndexCasesBuffer;    
    Render::SamplerStateRef  mDensitySampler;

    //compute count / caseId inputs
    Render::Uniform mGeomInfoDensityInput;
    Render::Uniform mGeomInfoCaseCountInfoInput;

    //compute sparse inputs
    Render::Uniform mVolumeCountUniform;
    Render::Uniform mSparse8Uniform;

    //mesh prod inputs
    Render::Uniform mMeshInfoCaseCountInfoInput;
    Render::Uniform mMeshInfoCaseIndices;
    Render::Uniform mMeshDensity;
    Render::Uniform mMeshCounts;
    Render::Uniform mMeshSparse8;
    Render::Uniform mMeshSparse4;


};
}
}
#endif
#endif
