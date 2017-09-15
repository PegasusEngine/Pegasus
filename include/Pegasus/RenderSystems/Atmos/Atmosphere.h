/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Atmosphere.h
//! \author Kleber Garcia
//! \date   August 26th 2016
//! \brief  3d Terrain class.

#ifndef PEGASUS_ATMOSPHERE
#define PEGASUS_ATMOSPHERE
#include "Pegasus/RenderSystems/Config.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Mesh/MeshManager.h"
#if 0 //RENDER_SYSTEM_CONFIG_ENABLE_ATMOS



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

//class AtmosLookUpGenerator;

class Atmosphere : public Application::GenericResource
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
	Atmosphere(Alloc::IAllocator* allocator, Mesh::MeshManager* meshManager);
    virtual ~Atmosphere() {}
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
		MAXIMUM_LODS = 3,
    };

    //! Returns the results of camera culling. This happens after update.
    const Segment* GetCullResults(int& outSize) const;

    //! Returns the lod bounding boxes. Mostly used for debug drawing
    const Box& GetLodBoundingBox(unsigned int index) const { PG_ASSERT(index < MAXIMUM_LODS && index >= 0); return mLodBoundingBoxes[index]; }


private:
    void AllocateMeshBatch();
    void GenerateCullingData(unsigned int windowWidth, unsigned int windowHeight);
    void PushStack(const Segment& s);
    const Segment& PopStack();
    void RegisterSegment(const Segment& s);
    void UpdateAtmosLodBoundingBoxes();

    Camera::CameraRef mCamera;
    Math::Vec3 mCamAabbMin;
    Math::Vec3 mCamAabbMax;

    Utils::Vector<Segment> mSegmentList;
    unsigned int mSegmentListSize;

    Utils::Vector<Segment> mSegmentStack;
    unsigned int mSegmentStackSize;

    Box mLodBoundingBoxes[MAXIMUM_LODS];

};

}
}

#endif
#endif
