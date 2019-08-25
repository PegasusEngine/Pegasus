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
class InternalJobBuilder;

class GpuJob
{
public:
    GpuJob(InternalJobHandle h, InternalJobBuilder* parent) : mParent(parent) {}
    void SetGpuPipeline(GpuPipelineRef gpuPipeline);
    void SetResourceTable(unsigned spaceRegister, ResourceTableRef resourceTable);
    void DependsOn(const GpuJob& otherJob);
    InternalJobHandle GetInternalJobHandle() const { return mJobHandle; }

protected:
    InternalJobBuilder* mParent;
    InternalJobHandle mJobHandle;
};

class DrawJob : public GpuJob
{
public:
    DrawJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    void SetRenderTarget(RenderTargetRef renderTargets);
    void Draw();
};

class ComputeJob : public GpuJob
{
public:
    ComputeJob(InternalJobHandle h, InternalJobBuilder* parent) : GpuJob(h, parent) {}
    virtual void SetUavTable(unsigned spaceRegister, ResourceTableRef uavTable);
    void Dispatch(unsigned x, unsigned y, unsigned z);
};

class JobBuilder
{
public:
    JobBuilder(IDevice* device);
    GpuJob RootJob();
    void SubmitRootJob();
	ComputeJob CreateComputeJob();
	DrawJob CreateDrawJob();

private:
    InternalJobBuilder* mImpl;
    IDevice* mDevice;
};


}
}
