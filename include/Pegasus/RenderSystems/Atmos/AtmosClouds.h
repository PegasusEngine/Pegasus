/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AtmosClouds.h
//! \author Arthur Homs
//! \date   August 24th 2017
//! \brief  Cloud class.

#ifndef PEGASUS_ATMOS_CLOUDS
#define PEGASUS_ATMOS_CLOUDS

#include "Pegasus/RenderSystems/Config.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS


#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/PropertyGrid/Property.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/RenderSystems/Camera/Camera.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{
namespace RenderSystems
{

class Terrain3dGenerator;

class Terrain3d : public Application::GenericResource
{
    BEGIN_DECLARE_PROPERTIES(Terrain3d,GenericResource)
        DECLARE_PROPERTY(int, Visible, 1)
        DECLARE_PROPERTY(Math::Vec3, Translation, Math::Vec3(0.0f,0.0f,0.0f))
        DECLARE_PROPERTY(Math::Vec3, Scale, Math::Vec3(1.0f,1.0f,1.0f))
        DECLARE_PROPERTY(int, BlockHeight, 2)
        DECLARE_PROPERTY(int, BlockLod0Threshold, 2)  //! Threshold is in segment units
        DECLARE_PROPERTY(int, BlockLod1Threshold, 4)
        DECLARE_PROPERTY(int, BlockLod2Threshold, 16)
    END_DECLARE_PROPERTIES()

public:
    Terrain3d(Alloc::IAllocator* allocator, Mesh::MeshManager* meshManager);
    virtual ~Terrain3d() {}
    virtual void Update() {}

    void Generate(unsigned int windowWidth, unsigned int windowHeight);

    //! Draw api. Vertex and pixel shaders must be set prior to calling this function.
    void Draw();

    //! Api function that receives a camera for culling purposes.
    void SetCullCamera(Camera::CameraRef camera) { mCamera = camera; }

    //! 
    static void GetBlockScriptApi(Utils::Vector<BlockScript::FunctionDeclarationDesc>& methods, Utils::Vector<BlockScript::FunctionDeclarationDesc>& functions);

    //! offset is done in: 1.0 units.
    //! size represents a single SegmentUnitSize increment.
    //! size resolution represents how many segment unit sizes;
    struct Segment {
        int sizeResolution;
        int offset[3];
        int size[3];
        Segment() { offset[0]=0;offset[1]=0;offset[2]=0;size[0]=0;size[1]=0;size[2]=0; }
    };

    struct Box {
        Math::Vec3 minAabb;
        Math::Vec3 maxAabb;
    };

    enum
    {
        MAXIMUM_LODS = 3, //WARNING: do not change this number, this is the total number of lods supported.
        MAXIMUM_TERRAIN_COVER_BOXES = (1 + 4*(MAXIMUM_LODS - 1)), // once box, eye centered for lod 0, 4 for every additional lod
        SEGMENT_UNIT_SIZE = 16 //WARNING: do not change this number. This is the size of a block in LDS.

    };

    //! Returns the results of camera culling. This happens after update.
    const Segment* GetCullResults(int& outSize) const;

    //! Returns the lod bounding boxes. Mostly used for debug drawing
    const Box& GetLodBoundingBox(unsigned int index) const { PG_ASSERT(index < MAXIMUM_LODS && index >= 0); return mLodBoundingBoxes[index]; }

    //! Returns the lod cover boxes.
    const Segment& GetLodCoverBox(unsigned int index) const { return mTerrainCoverBoxes[index]; }


private:
    void AllocateMeshBatch();
    void GenerateCullingData(unsigned int windowWidth, unsigned int windowHeight);
    void PushStack(const Segment& s);
    const Segment& PopStack();
    void RegisterSegment(const Segment& s);
    void UpdateTerrainLodBoundingBoxes();

    Camera::CameraRef mCamera;
    Math::Vec3 mCamAabbMin;
    Math::Vec3 mCamAabbMax;

    Utils::Vector<Segment> mSegmentList;
    unsigned int mSegmentListSize;

    Utils::Vector<Segment> mSegmentStack;
    unsigned int mSegmentStackSize;

    Box mLodBoundingBoxes[MAXIMUM_LODS];
    Segment mTerrainCoverBoxes[MAXIMUM_TERRAIN_COVER_BOXES];

    struct MeshInfo
    {
        Terrain3dGenerator* generator;
        Mesh::MeshRef mesh;
        MeshInfo() : generator(nullptr) {}
    };

    Utils::Vector<MeshInfo> mMeshPool;
    unsigned int mMeshPoolCount;
    Mesh::MeshManager* mMeshManager;

};

}
}

#endif // RENDER_SYSTEM_CONFIG_ENABLE_ATMOS
#endif // PEGASUS_ATMOS_CLOUDS
