/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Terrain3d.cpp
//! \author Kleber Garcia
//! \date   August 26th 2016
//! \brief  3d Terrain class.

#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/RenderSystems/Volumes/Terrain3d.h"
#include "Pegasus/RenderSystems/Volumes/VolumesSystem.h"
#include "Pegasus/RenderSystems/Volumes/Terrain3dGenerator.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_VOLUMES

#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Timeline/Timeline.h"


using namespace Pegasus;
using namespace Pegasus::Application;
using namespace Pegasus::Camera;
using namespace Pegasus::Math;

extern Pegasus::RenderSystems::VolumesSystem* gVolumesSystem;



namespace Pegasus
{
namespace RenderSystems
{

BEGIN_IMPLEMENT_PROPERTIES(Terrain3d)
    IMPLEMENT_PROPERTY(Terrain3d, Visible)
    IMPLEMENT_PROPERTY(Terrain3d, Translation)
    IMPLEMENT_PROPERTY(Terrain3d, Scale)
    IMPLEMENT_PROPERTY(Terrain3d, BlockHeight)
    IMPLEMENT_PROPERTY(Terrain3d, BlockLod0Threshold)
    IMPLEMENT_PROPERTY(Terrain3d, BlockLod1Threshold)
    IMPLEMENT_PROPERTY(Terrain3d, BlockLod2Threshold)
END_IMPLEMENT_PROPERTIES(Terrain3d)

Terrain3d::Terrain3d(Alloc::IAllocator* allocator, Mesh::MeshManager* meshManager)
    : Application::GenericResource(allocator),
      mSegmentListSize(0), mSegmentStackSize(0), mSegmentList(allocator), mSegmentStack(allocator), mMeshManager(meshManager), mMeshPool(allocator), mMeshPoolCount(0)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(Terrain3d)
        INIT_PROPERTY(Visible)
        INIT_PROPERTY(Translation)
        INIT_PROPERTY(Scale)
        INIT_PROPERTY(BlockHeight)
        INIT_PROPERTY(BlockLod0Threshold)
        INIT_PROPERTY(BlockLod1Threshold)
        INIT_PROPERTY(BlockLod2Threshold)
    END_INIT_PROPERTIES()

    AllocateMeshBatch();
}

void Terrain3d::AllocateMeshBatch()
{
    for (int i = 0; i < 75; ++i)
    {
        MeshInfo& mi = mMeshPool.PushEmpty();
        Mesh::MeshGeneratorRef meshGen = mMeshManager->CreateMeshGeneratorNode("Terrain3dGenerator");
        mi.generator = static_cast<Terrain3dGenerator*>(&(*meshGen));
        PG_ASSERT(mi.generator != nullptr);
        mi.generator->CreateComputeTerrainResources(); //create the intermediate compute gpu resources
        mi.mesh = mMeshManager->CreateMeshNode();
        mi.mesh->SetGeneratorInput(mi.generator);
    }
}

void Terrain3d::Generate(unsigned int windowWidth, unsigned int windowHeight)
{
    static float sTerrainSeed = 0.0f;
    GenerateCullingData(windowWidth,windowHeight);

    //reset the mesh pool count.
    mMeshPoolCount = 0;

    //generate now all the mesh data necesary.
    for (unsigned int sid = 0; sid < mSegmentList.GetSize(); ++sid)
    {
        Segment& s = mSegmentList[sid];
        Vec3 originalOffset = Math::Vec3((float)s.offset[0],(float)s.offset[1],(float)s.offset[2]);
        for (int i = 0; i < s.size[0]; ++i)
            for (int j = 0; j < s.size[1]; ++j)
                for (int k = 0; k < s.size[2]; ++k)
        {
                    if (mMeshPoolCount >= mMeshPool.GetSize())
                    {
                        AllocateMeshBatch();
                    }

                    MeshInfo& mi = mMeshPool[mMeshPoolCount];
                    mi.generator->SetTerrainSeed(sTerrainSeed);
                    Vec3 offset = originalOffset + (float)(s.sizeResolution*SEGMENT_UNIT_SIZE)*Vec3((float)i,(float)j,(float)k);
                    mi.generator->SetOffsetScale((float)s.sizeResolution,offset);
                    mi.generator->Update();
                    ++mMeshPoolCount;
        }
    }

    sTerrainSeed += 0.01f;
    if (sTerrainSeed > 1000.0f)
    {
        sTerrainSeed = 0.0f;
    }
}

void Terrain3d::PushStack(const Terrain3d::Segment& s)
{
    if (mSegmentStackSize == mSegmentStack.GetSize())
    {
        mSegmentStack.PushEmpty() = s;
    }
    else
    {
        mSegmentStack[mSegmentStackSize] = s;
    }
    mSegmentStackSize++;
}

const Terrain3d::Segment& Terrain3d::PopStack()
{
    PG_ASSERT(mSegmentStackSize != 0);
    return mSegmentStack[--mSegmentStackSize];
}

void Terrain3d::RegisterSegment(const Terrain3d::Segment& s)
{
    if (mSegmentListSize == mSegmentList.GetSize())
    {
        mSegmentList.PushEmpty() = s;
    }
    else
    {
        mSegmentList[mSegmentListSize] = s;
    }
    mSegmentListSize++;
}

Camera::Camera::CollisionState ComputeCollistion(const Terrain3d::Segment& s, const Camera::Camera::Frustum& f)
{
    Math::Vec3 aabbMin = Math::Vec3((float)s.offset[0],(float)s.offset[1],(float)s.offset[2]);
    Math::Vec3 aabbMax = aabbMin + Math::Vec3((float)s.size[0],(float)s.size[1],(float)s.size[2]) * (float)Terrain3d::SEGMENT_UNIT_SIZE * (float)s.sizeResolution;
    return f.GetCollisionState(aabbMin,aabbMax);
}

void Terrain3d::GenerateCullingData(unsigned int windowWidth, unsigned int windowHeight)
{
    if (mCamera != nullptr)
    {
        mCamera->WindowUpdate(windowWidth, windowHeight);
#if PEGASUS_ENABLE_PROXIES
        if (gVolumesSystem->CamCullDebugEnable())
            return;
#endif


        mSegmentListSize = 0;
        mSegmentStackSize = 0;
        const Camera::Camera::Frustum& f = mCamera->GetWorldFrustum();

        //Step 1, create terrain lod bounding boxes.
        UpdateTerrainLodBoundingBoxes();

        //Step 2 create smaller segments of such bounding boxes, let the loop below break these down into single segments, representing shader dispatches.
        for (int i = 0; i < MAXIMUM_TERRAIN_COVER_BOXES; ++i)
        {
            PushStack(mTerrainCoverBoxes[i]);
        }
        
        while (mSegmentStackSize > 0)
        {
//! use this to debug the initial position of the lod boxes.
#if 1
            const Terrain3d::Segment s = PopStack();
            Camera::Camera::CollisionState collisionState = ComputeCollistion(s, f);
            switch(collisionState)
            {
            case Camera::Camera::INSIDE:                
                RegisterSegment(s);  
                break;
            case Camera::Camera::INTERSECT:
                //break the segment into 8 segments
                if (s.size[0] == 1 && s.size[1] == 1 && s.size[2] == 1)
                {
                    RegisterSegment(s);
                }
                else if (s.size[0] > 0 && s.size[1] > 0 && s.size[2] > 0)
                {
                    int divisions[3][2];
                    int offset[3];
                    for (int d = 0; d < 3; ++d)
                    {
                        divisions[d][0] = (s.size[d] % 2) == 0 ? s.size[d] / 2 : (s.size[d] / 2) + 1;
                        divisions[d][1] = s.size[d] -  divisions[d][0];
                    }

                    offset[0] = 0;
                    for (int i = 0; i < 2; ++i)
                    {
                        if (divisions[0][i] == 0) continue;
                        offset[1] = 0;
                        for (int j = 0; j < 2; ++j)
                        {
                            if (divisions[1][j] == 0) continue;
                            offset[2] = 0;
                            for (int k = 0; k < 2; ++k)
                            {
                                if (divisions[2][k] == 0) continue;
                                Terrain3d::Segment newS = s;
                                newS.offset[0] += offset[0];
                                newS.offset[1] += offset[1];
                                newS.offset[2] += offset[2];

                                newS.size[0] = divisions[0][i];
                                newS.size[1] = divisions[1][j];
                                newS.size[2] = divisions[2][k];

                                PushStack(newS);
                                offset[2] += divisions[2][k]*SEGMENT_UNIT_SIZE*s.sizeResolution;
                            }
                            offset[1] += divisions[1][j]*SEGMENT_UNIT_SIZE*s.sizeResolution;
                        }
                        offset[0] += divisions[0][i]*SEGMENT_UNIT_SIZE*s.sizeResolution;
                    }
                }
                break;
            case Camera::Camera::OUTSIDE:
            default:
                break;
            }
            
#else
            RegisterSegment(PopStack());
#endif
        }
    }
    else
    {
        PG_LOG('ERR_', "Terrain does not have cull camera attached! please attach a cull camera.");
    }
}

static void CreateTerrain3d_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());

    int height = stream.NextArgument<int>();    
    int maxBlockCount = stream.NextArgument<int>();    
    Terrain3d* newTerrain = PG_NEW(Memory::GetCoreAllocator(), -1, "3dTerrain", Pegasus::Alloc::PG_MEM_TEMP) Terrain3d(Memory::GetCoreAllocator(), collection->GetAppContext()->GetMeshManager());

    RenderCollection::CollectionHandle handle = RenderCollection::AddResource<GenericResource>(collection, static_cast<GenericResource*>(newTerrain));
    stream.SubmitReturn<RenderCollection::CollectionHandle>(handle);

}

static void DrawTerrain3d_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();    

    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        
        Terrain3d* theTerrain = static_cast<Terrain3d*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        theTerrain->Draw();
    }
    else
    {
        PG_LOG('ERR_', "Error: attempting to draw an invalid terrain.");
    }
}

static void SetCullCamera_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();    
    RenderCollection::CollectionHandle camHandle  = stream.NextArgument<RenderCollection::CollectionHandle>();    

    if (thisHandle != RenderCollection::INVALID_HANDLE && camHandle != RenderCollection::INVALID_HANDLE)
    {
        Terrain3d* theTerrain = static_cast<Terrain3d*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        Pegasus::Camera::Camera* cam = static_cast<Pegasus::Camera::Camera*>(RenderCollection::GetResource<GenericResource>(collection, camHandle));
        theTerrain->SetCullCamera(cam);
    }
    else
    {
        PG_LOG('ERR_', "Error: attempting to draw an invalid terrain.");
    }
}

static void Generate_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>(); 

    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        Terrain3d* theTerrain = static_cast<Terrain3d*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        if (collection->GetRenderInfo() != nullptr)
        {
            theTerrain->Generate(collection->GetRenderInfo()->viewportWidth, collection->GetRenderInfo()->viewportHeight);
        }
        else
        {
            PG_LOG('ERR_', "Calling Terrain3d::Generate in the wrong context! must be called only on Render()");
        }
    }
    else
    {
        PG_LOG('ERR_', "Error: attempting to draw an invalid terrain.");
    }
}

void Terrain3d::GetBlockScriptApi(Utils::Vector<BlockScript::FunctionDeclarationDesc>& methods, Utils::Vector<BlockScript::FunctionDeclarationDesc>& functions)
{
    BlockScript::FunctionDeclarationDesc desc;
    desc.functionName = "CreateTerrain3d";
    desc.returnType = "Terrain3d";
    desc.argumentTypes[0] = "int";
    desc.argumentNames[0] = "height";
    desc.argumentTypes[1] = "int";
    desc.argumentNames[1] = "depth";
    desc.argumentTypes[2] = nullptr;
    desc.argumentNames[2] = nullptr;
    desc.callback = CreateTerrain3d_Callback;
    functions.PushEmpty() = desc; 

    desc.functionName = "Draw";
    desc.returnType = "int";
    desc.argumentTypes[0] = "Terrain3d";
    desc.argumentNames[0] = "this";
    desc.argumentTypes[1] = nullptr;
    desc.argumentNames[1] = nullptr;
    desc.callback = DrawTerrain3d_Callback;
    methods.PushEmpty() = desc;

    desc.functionName = "SetCullCamera";
    desc.returnType = "int";
    desc.argumentTypes[0] = "Terrain3d";
    desc.argumentNames[0] = "this";
    desc.argumentTypes[1] = "Camera";
    desc.argumentNames[1] = "cam";
    desc.argumentTypes[2] = nullptr;
    desc.argumentNames[2] = nullptr;
    desc.callback = SetCullCamera_Callback;
    methods.PushEmpty() = desc;

    desc.functionName = "Generate";
    desc.returnType = "int";
    desc.argumentTypes[0] = "Terrain3d";
    desc.argumentNames[0] = "this";
    desc.argumentTypes[1] = nullptr;
    desc.argumentNames[1] = nullptr;
    desc.callback = Generate_Callback;
    methods.PushEmpty() = desc;
    

}

void Terrain3d::Draw()
{
    //TODO: sort by depht!
    for (unsigned int i = 0; i < mMeshPoolCount; ++i)
    {
        MeshInfo& mi = mMeshPool[i];
        Render::SetMesh(mi.mesh);
        Render::Draw();
    }

#if PEGASUS_ENABLE_PROXIES
    gVolumesSystem->DrawDebugTerrain(this);
#endif
}

void Terrain3d::UpdateTerrainLodBoundingBoxes()
{
    if (mCamera != nullptr)
    {
        //Step 1, compute lod bounding boxes around camera.
        const Camera::Camera::Frustum& f = mCamera->GetWorldFrustum();
        mCamAabbMax = f.points[0];
        mCamAabbMin = f.points[0];
        //compute camera aabb.
        for (unsigned int i = 1; i < Camera::Camera::MAX_POINTS; ++i)
        { 
            mCamAabbMax = Math::Max(f.points[i], mCamAabbMax);
            mCamAabbMin = Math::Min(f.points[i], mCamAabbMin);
        }

        mCamAabbMin.y = 0.0f;
        mCamAabbMax.y = Math::Clamp(mCamAabbMax.y, 0.0f, static_cast<float>(GetBlockHeight() * SEGMENT_UNIT_SIZE));

        Vec4& camPos = mCamera->GetGpuData().camPos;
        Vec3 p3 = Floor(Vec3(camPos.x,camPos.y,camPos.z));
        int thresholds[MAXIMUM_LODS];
        thresholds[0] = GetBlockLod0Threshold();
        thresholds[1] = GetBlockLod1Threshold();
        thresholds[2] = GetBlockLod2Threshold();
        for (unsigned int i = 0; i < MAXIMUM_LODS; ++i)
        {
            int threshold = thresholds[i];
            Box& b = mLodBoundingBoxes[i];
            Vec3 v = Vec3((float)threshold * (float)SEGMENT_UNIT_SIZE);
            b.maxAabb = p3 + v;
            b.minAabb = p3 - v;
        }


        //Step 2, compute lod bounding boxes around terrain, in a spiral manner
        /*
    _________________
   | lod1       |    |
   |____________|    |
   |    |       |lod1|
   |    |       |    |
   |    | lod0  |    |
   |    |_______|____| 
   |    |       lod1 |
   |lod1|____________|
         
   Lod 2 would wrap the terrain again in the same manner as lod1 wraps lod0.  
   Lod 0 is placed in the xz plane of the projection of the camera center.

        */
        float lastLodSideSize = 0.0f;
        Vec3 prevOrigin;
        int nextCoverBoxIdx = 0;
        for (unsigned int lod = 0; lod < MAXIMUM_LODS; ++lod)
        {
            Box& b = mLodBoundingBoxes[lod];
            Vec3 originF = b.minAabb;
            Segment s;
            s.sizeResolution = 1 << lod;
            if (lod == 0)
            {
                s.offset[0] = (int)originF.x;
                s.offset[1] = 0;
                s.offset[2] = (int)originF.z;

                Math::Vec3 sz = (b.maxAabb - b.minAabb)/(float)SEGMENT_UNIT_SIZE;
                lastLodSideSize = sz.x;
                s.size[0] = (int)sz.x;
                s.size[1] = GetBlockHeight();
                s.size[2] = (int)sz.z;
                mTerrainCoverBoxes[nextCoverBoxIdx++] = s;
                prevOrigin = originF;
            }
            else
            {
                float lodBoxSize = (float)(SEGMENT_UNIT_SIZE << lod);                
                s.offset[1] = 0;
                s.size[1] = Max(GetBlockHeight() / (1 << lod),1);
                //divide further lods into 4 boxes.
                float sideDivisionSmall = Ceil(Abs((originF.x - prevOrigin.x) / lodBoxSize));
                float sideDivisionBig = Math::Ceil((lastLodSideSize*(float)SEGMENT_UNIT_SIZE)/lodBoxSize + sideDivisionSmall);
                s.offset[0] = (int)prevOrigin.x - (int)(sideDivisionSmall*lodBoxSize);
                s.offset[2] = (int)prevOrigin.z + (int)(lastLodSideSize*SEGMENT_UNIT_SIZE) - (int)(sideDivisionBig*lodBoxSize);
                s.size[0] = (int)sideDivisionSmall;
                s.size[2] = (int)sideDivisionBig;
                mTerrainCoverBoxes[nextCoverBoxIdx++] = s;

                s.offset[0] = (int)prevOrigin.x;
                s.offset[2] = (int)prevOrigin.z - (int)(sideDivisionSmall*lodBoxSize);
                s.size[0] = (int)sideDivisionBig;
                s.size[2] = (int)sideDivisionSmall;                
                mTerrainCoverBoxes[nextCoverBoxIdx++] = s;

                s.offset[0] = (int)prevOrigin.x + (int)(lastLodSideSize*SEGMENT_UNIT_SIZE) - (int)(sideDivisionBig*lodBoxSize);
                s.offset[2] = (int)prevOrigin.z + (int)lastLodSideSize*SEGMENT_UNIT_SIZE;
                s.size[0] = (int)sideDivisionBig;
                s.size[2] = (int)sideDivisionSmall;
                mTerrainCoverBoxes[nextCoverBoxIdx++] = s;

                s.offset[0] = (int)prevOrigin.x + (int)lastLodSideSize*SEGMENT_UNIT_SIZE;
                s.offset[2] = (int)prevOrigin.z;
                s.size[0] = (int)sideDivisionSmall;
                s.size[2] = (int)sideDivisionBig;
                mTerrainCoverBoxes[nextCoverBoxIdx++] = s;

                prevOrigin = Vec3(prevOrigin.x - (int)sideDivisionSmall*lodBoxSize,0.0f,prevOrigin.z + (lastLodSideSize*SEGMENT_UNIT_SIZE) - (sideDivisionBig*lodBoxSize));                
                lastLodSideSize = (sideDivisionBig + sideDivisionSmall) * s.sizeResolution;

            }
        }
        PG_ASSERT((unsigned)nextCoverBoxIdx == MAXIMUM_TERRAIN_COVER_BOXES);
    }
}

const Terrain3d::Segment* Terrain3d::GetCullResults(int& outSize) const
{
    outSize = mSegmentListSize;
    return mSegmentList.Data();
}

}
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
