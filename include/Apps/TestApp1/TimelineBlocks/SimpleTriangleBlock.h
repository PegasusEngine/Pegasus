/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

#pragma once

#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Timeline/Block.h"

namespace Pegasus {
    namespace Timeline {
        struct UpdateInfo;
        struct RenderInfo;
    }
}

class SimpleTriangleBlock : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(SimpleTriangleBlock, "SimpleTriangle");

public:

    SimpleTriangleBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext);
    virtual ~SimpleTriangleBlock();
    virtual void Initialize() override;
    virtual void Shutdown() override;
    virtual void Update(const Pegasus::Timeline::UpdateInfo& updateInfo) override;
    virtual void Render(const Pegasus::Timeline::RenderInfo& renderInfo) override;

private:
    Pegasus::Render::GpuStateRef mGpuState;
    Pegasus::Render::GpuPipelineRef mPso;
    Pegasus::Render::BufferRef mVb;
};

