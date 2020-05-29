#include "TimelineBlocks/SimpleTriangleBlock.h"
#include "Pegasus/Render/IDevice.h"

using namespace Pegasus::Render;

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
}

void SimpleTriangleBlock::Shutdown()
{
}

void SimpleTriangleBlock::Update(const Pegasus::Timeline::UpdateInfo& updateInfo)
{
}

void SimpleTriangleBlock::Render(const Pegasus::Timeline::RenderInfo& renderInfo)
{
}
