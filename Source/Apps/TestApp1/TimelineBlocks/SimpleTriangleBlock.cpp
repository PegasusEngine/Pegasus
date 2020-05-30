#include "TimelineBlocks/SimpleTriangleBlock.h"
#include "Pegasus/Render/IDevice.h"
#include "Pegasus/Render/JobBuilder.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Timeline/Timeline.h"

using namespace Pegasus::Render;
using namespace Pegasus::Math;

SimpleTriangleBlock::SimpleTriangleBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext)
: Pegasus::Timeline::Block(allocator, appContext)
{
}

SimpleTriangleBlock::~SimpleTriangleBlock()
{
}

void SimpleTriangleBlock::Initialize()
{
    GpuStateConfig simpleTriConfig;
    simpleTriConfig.dsDesc.depthEnable = false;
    simpleTriConfig.elementCounts = 2;
    auto& els = simpleTriConfig.elements;
    els[0].semantic = "POSITION";
    els[0].format = Pegasus::Core::FORMAT_RGBA_32_FLOAT;
    els[1].semantic = "COLOR";
    els[1].format = Pegasus::Core::FORMAT_RGB_32_FLOAT;
    IDevice* device = GetDevice();
    mGpuState = device->CreateGpuState(simpleTriConfig);

    const char* simepleTriProgram = R"(
        struct VsIn
        {
            float4 p : POSITION0;
            float3 c : COLOR0;
        };

        struct VsOut
        {
            float4 pos : SV_Position;
            float3 color : COLOR0;
        };

        void vsMain(in VsIn vsIn, out VsOut vsOut)
        {
            vsOut.pos = vsIn.p;
            vsOut.color = vsIn.c;
        }

        void psMain(in VsOut vsIn,  out float4 output : SV_Target0)
        {
            output = float4(vsIn.color, 1.0);
        }
    )";

    GpuPipelineConfig pipelineConfig;
    pipelineConfig.graphicsState = mGpuState;
    pipelineConfig.source = simepleTriProgram;
    pipelineConfig.mainNames[Pipeline_Vertex] = "vsMain";
    pipelineConfig.mainNames[Pipeline_Pixel]  = "psMain";
    mPso = device->CreateGpuPipeline();
    bool success = mPso->Compile(pipelineConfig);
    PG_ASSERTSTR(success, "Compilation of SimpleTriangleBlock failed.");

    struct Vertex
    {
        Vec4 pos;
        Vec3 col;
    };

    const unsigned vertCount = 3u;

    BufferConfig vbConfig;
    vbConfig.name = "Vertex";
    vbConfig.bufferType = BufferType_Default;
    vbConfig.format = Pegasus::Core::FORMAT_RGBA_32_FLOAT;
    vbConfig.stride = sizeof(Vertex);
    vbConfig.elementCount = vertCount;
    vbConfig.usage = ResourceUsage_Static;
    vbConfig.bindFlags = BindFlags_Vb;
    mVb = device->CreateBuffer(vbConfig);

    BufferRef tmpBuffer;
    Vertex* v = device->CreateUploadBuffer<Vertex>(tmpBuffer, vertCount);
    v[0] = { { 0.0f, 1.0f, 0.1f, 1.0f},  { 1.0f, 0.0f, 0.0f } };
    v[1] = { { -1.0f, 0.0f, 0.1f, 1.0f}, { 0.0f, 1.0f, 0.0f } };
    v[2] = { { 1.0f, 0.0f, 0.1f, 1.0f},  { 0.0f, 0.0f, 1.0f } };
    {
        JobBuilder jb(device); 
        RootJob rj = jb.CreateRootJob();
        CopyJob cj = jb.CreateCopyJob();
        cj.AddDependency(rj);
        cj.SetGpuPipeline(mPso);
        cj.Set(tmpBuffer, mVb);
        auto result = device->Submit(rj);
        PG_ASSERT(result.resultCode == GpuWorkResultCode::Success);
        device->Wait(result.handle);
        device->ReleaseWork(result.handle);
    }
}

void SimpleTriangleBlock::Shutdown()
{
}

void SimpleTriangleBlock::Update(const Pegasus::Timeline::UpdateInfo& updateInfo)
{
}

void SimpleTriangleBlock::Render(const Pegasus::Timeline::RenderInfo& renderInfo)
{
    JobBuilder jb(GetDevice()); 
    RootJob rj = jb.CreateRootJob();
    DrawJob dj = jb.CreateDrawJob();
    dj.AddDependency(rj);
    dj.SetVertexBuffers(&mVb, 1u);
    dj.SetRenderTarget(renderInfo.displayRenderTarget);
    DrawJob::NonIndexedParams p;
    p.vertexCountPerInstance = 3u;
    dj.SetDrawParams(p);

    auto result = GetDevice()->Submit(rj);
    PG_ASSERT(result.resultCode == GpuWorkResultCode::Success);
}
