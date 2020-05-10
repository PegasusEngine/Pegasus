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
    DrawJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void SetConstantBuffer(unsigned registerId, BufferRef buffer);
    void SetRenderTarget(RenderTargetRef renderTargets);
    void SetDrawParams();
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

class DisplayJob : public GpuJob
{
public:
    DisplayJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
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
    GroupJob CreateGroupJob();

    void Delete(RootJob rootJob);

private:
    InternalJobBuilder* mImpl;
    IDevice* mDevice;
};


}
}
