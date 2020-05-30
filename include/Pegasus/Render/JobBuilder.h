/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   JobBuilder.h
//! \author Kleber Garcia
//! \date   August 24th, 2019
//! \brief  Dx12/Vulcan job based API for GPU rendering


#pragma once

#include <Pegasus/Render/Render.h>
#include <Pegasus/Render/IDisplay.h>
#include <vector>
#include <variant>
#include <map>
#include <set>

namespace Pegasus
{
namespace Render
{

class IDevice;
class InternalJobBuilder;

typedef int InternalJobHandle;
const InternalJobHandle InvalidJobHandle = -1;

class GpuJob
{
public:
    GpuJob(InternalJobHandle h, InternalJobBuilder* parent) : mParent(parent), mJobHandle(h) {}
    void SetName(const char* name);
    const char* GetName() const;
    void SetGpuPipeline(GpuPipelineRef gpuPipeline);
    void SetResourceTable(unsigned spaceRegister, ResourceTableRef resourceTable);
    void SetUavTable(unsigned spaceRegister, ResourceTableRef uavTable);
    void AddDependency(const GpuJob& otherJob);

    InternalJobBuilder* GetParent() const { return mParent; }
    InternalJobHandle GetInternalJobHandle() const { return mJobHandle; }

protected:
    InternalJobBuilder* mParent;
    InternalJobHandle mJobHandle;
};

class RootJob : public GpuJob
{
public:
    RootJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
};

class DrawJob : public GpuJob
{
public:

    struct NonIndexedParams
    {
        unsigned vertexCountPerInstance = 0u;
        unsigned instanceCount = 1u;
        unsigned vertexOffset = 0u;
        unsigned instanceOffset = 0u;
    };

    struct IndexedParams
    {
        unsigned indexCountPerInstance = 0u;
        unsigned instanceCount = 1u;
        unsigned indexOffset = 0u;
        unsigned vertexOffset = 0u;
        unsigned instanceOffset = 0u;
    };

    DrawJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void SetVertexBuffers(BufferRef buffers[], unsigned int count);
    void SetConstantBuffer(unsigned registerId, BufferRef buffer);
    void SetRenderTarget(RenderTargetRef renderTargets);
    void SetDrawParams(const NonIndexedParams& args);
    void SetDrawParams(const IndexedParams& args);
    DrawJob Next();
};

class ComputeJob : public GpuJob
{
public:
    ComputeJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void SetConstantBuffer(unsigned registerId, BufferRef buffer);
    void SetDispatchParams(unsigned x, unsigned y, unsigned z);
    ComputeJob Next();
};

class CopyJob : public GpuJob
{
public:
    CopyJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void Set(BufferRef src, BufferRef dst);
    void Set(TextureRef src, TextureRef dst);
    CopyJob Next();
};

class ClearRenderTargetJob : public GpuJob
{
public: 
    ClearRenderTargetJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void Set(RenderTargetRef rt, float color[4]); 
};

class DisplayJob : public GpuJob
{
public:
    DisplayJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void SetPresentable(IDisplayRef display);
};

class GroupJob : public GpuJob
{
public:
    GroupJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void AddJob(const GpuJob& other);
    void AddJobs(const GpuJob* jobs, unsigned counts);
};

class JobBuilder
{
public:
    JobBuilder(IDevice* device);
    ~JobBuilder();

    RootJob CreateRootJob();
    DrawJob CreateDrawJob();
    ComputeJob CreateComputeJob();
    CopyJob CreateCopyJob();
    DisplayJob CreateDisplayJob();
    ClearRenderTargetJob CreateClearRenderTargetJob();
    GroupJob CreateGroupJob();

    void Delete(RootJob rootJob);

private:
    InternalJobBuilder* mImpl;
    IDevice* mDevice;
};


}
}
