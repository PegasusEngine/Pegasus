#include "Pegasus/UnitTests/RenderTests.h"
#include "Pegasus/UnitTests/UnitTests.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Memory/mallocfreeallocator.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Render/IDevice.h"
#include <Pegasus/Core/Shared/ISourceCodeProxy.h>
#include <Pegasus/Core/Shared/CompilerEvents.h>
#include "../Source/Pegasus/Render/Common/ResourceStateTable.h"
#include "../Source/Pegasus/Render/Common/JobTreeVisitors.h"
#include <string>
#include <iostream>

using namespace Pegasus::Render;

namespace Pegasus
{
	namespace Tests
	{

		static void RenderLogFn(Core::LogChannel logChannel, const char* msgStr)
		{
			char logChnl[5];
			for (int i = 0; i < 4; ++i)
			{
				logChnl[i] = reinterpret_cast<char*>(&logChannel)[3 - i];
			}

			logChnl[4] = 0;
			std::cout << "[" << logChnl << "]" << msgStr << std::endl;
		}

		static Pegasus::Core::AssertReturnCode AssertHandler(const char * testStr,
			const char * fileStr,
			int line,
			const char * msgStr)
		{
			std::cout << "ASSERT[" << fileStr << ":" << line << "] ";
			if (msgStr != nullptr)
				std::cout << "\"" << msgStr << "\"";
			std::cout << std::endl;
			return Pegasus::Core::ASSERTION_CONTINUE;
		}

#if PEGASUS_USE_EVENTS
		class CompilerListener : public Pegasus::Core::CompilerEvents::ICompilerEventListener
		{
		public:
			virtual void OnInitUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name) override
			{
			}

			virtual void OnDestroyUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name)override
			{
			}
			virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::SourceLoadedEvent& e)  override
			{
			}
			virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::CompilationEvent& e)  override
			{
			}
			virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::CompilationNotification& e)  override
			{
				if (e.GetType() == Pegasus::Core::CompilerEvents::CompilationNotification::COMPILATION_ERROR)
				{
					std::cout << "Gpu Program Error: " << e.GetDescription() << std::endl;
				}
			}
			virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Core::CompilerEvents::LinkingEvent& e)  override
			{
			}
		};
#endif

		class RenderHarness : public TestHarness
		{
		public:
			RenderHarness();
			virtual void Initialize();
			virtual void Destroy();
			virtual const TestFunc* GetTests(int& funcSz) const;

			IDevice* CreateDevice();
			void DestroyDevice();

			Pegasus::Alloc::IAllocator* GetAllocator() { return &mAllocator; }

#if PEGASUS_USE_EVENTS
			CompilerListener& GetCompilerListener() { return mCompilerListener; }
#endif

		private:
			IDevice* mDevice;
			Pegasus::Memory::MallocFreeAllocator mAllocator;

#if PEGASUS_USE_EVENTS
			CompilerListener mCompilerListener;
#endif
		};


		RenderHarness::RenderHarness()
			: mAllocator(0), mDevice(nullptr)
		{
		}

		void RenderHarness::Initialize()
		{
			auto* logManager = Core::Singleton<Core::LogManager>::CreateInstance(&mAllocator);
			logManager->RegisterHandler(RenderLogFn);

			auto* assertMgr = Core::Singleton<Core::AssertionManager>::CreateInstance(&mAllocator);
			assertMgr->RegisterHandler(AssertHandler);
		}

		void RenderHarness::Destroy()
		{
			Core::Singleton<Core::LogManager>::DestroyInstance();
		}

		IDevice* RenderHarness::CreateDevice()
		{
			if (mDevice == nullptr)
			{
				Pegasus::Render::DeviceConfig config;
				config.platform = DevicePlat::Dx12;
				config.mModuleHandle = 0;
				config.mIOManager = nullptr;
				mDevice = IDevice::CreatePlatformDevice(config, &mAllocator);
			}

			return mDevice;
		}

		void RenderHarness::DestroyDevice()
		{
			PG_DELETE(&mAllocator, mDevice);
			mDevice = nullptr;
		}

		bool runCreateDevice(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			return rh->CreateDevice() != nullptr;
		}

		bool runCreateSimpleGpuPipeline(TestHarness* harness)
		{
            static const char* simpleProgram0 = R"(
                SamplerState gSampler : register(s0,space0);
                Texture2D tex0 : register(t0, space0);
            
                struct VsIn
                {
                    float4 pos : POSITION0;
                    float3 n : NORMAL0;
                };
            
                struct VsOut
                {
                    float4 p : SV_Position;
                    float3 normal : TEXCOORD;
                };
            
                cbuffer Constants : register(b0)
                {
                    float4x4 g_viewProjTransform;
                };
            
                void vsMain(VsIn vsIn, out VsOut vsOut)
                {
                    vsOut.p = mul(vsIn.pos, g_viewProjTransform);
                    vsOut.normal = float3(0,0,0);//vsIn.n;
                }
            
                void psMain(in VsOut vsOut, out float4 c : SV_Target0)
                {
                    c = float4(tex0.Sample(gSampler, vsOut.normal.xy).rgb, 1.0);
                }
            )";

			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();
			GpuPipelineRef gpuPipeline = device->CreateGpuPipeline();

#if PEGASUS_USE_EVENTS
			gpuPipeline->SetEventListener(&rh->GetCompilerListener());
#endif

			GpuPipelineConfig gpuPipelineConfig;
			gpuPipelineConfig.source = simpleProgram0;
			gpuPipelineConfig.mainNames[Pipeline_Vertex] = "vsMain";
			gpuPipelineConfig.mainNames[Pipeline_Pixel] = "psMain";

			bool result = gpuPipeline->Compile(gpuPipelineConfig);
			if (!result)
				return false;

			return true;
		}

		bool runTestSimpleCompute(TestHarness* harness)
		{
            const char* simpleCs0 = R"(
                Buffer<float4> input : register(t0, space0);
                RWBuffer<float4> output : register(u0, space0);
            
                cbuffer Constants : register(b0)
                {
                    uint4 gCounts;
                };
            
                [numthreads(32,1,1)]
                void csMain(uint di : SV_DispatchThreadID)
                {
                    output[di] = input[di];
                }
            )";

			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();
			GpuPipelineConfig gpuPipelineConfig;
			gpuPipelineConfig.source = simpleCs0;
			gpuPipelineConfig.mainNames[Pipeline_Compute] = "csMain";

			GpuPipelineRef gpuPipeline = device->CreateGpuPipeline();

#if PEGASUS_USE_EVENTS
			gpuPipeline->SetEventListener(&rh->GetCompilerListener());
#endif

			if (!gpuPipeline->Compile(gpuPipelineConfig))
				return false;

			return true;
		}

		bool runDestroyDevice(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			rh->DestroyDevice();
			return true;
		}

		bool runTestResourceStateTable(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();
			ResourceStateTable* resourceStateTable = device->GetResourceStateTable();

			uintptr_t state0 = 0xdeadbeef;
			uintptr_t state1 = 0xdeadbeea;
			uintptr_t state2 = 0xffffffff;
			uintptr_t state3 = 0xaaaaaaaa;

			int slot0 = resourceStateTable->CreateStateSlot();
			int slot1 = resourceStateTable->CreateStateSlot();
			ResourceStateTable::Domain d0 = resourceStateTable->CreateDomain();

			resourceStateTable->StoreState(d0, slot0, state0);
			resourceStateTable->StoreState(d0, slot1, state1);

			ResourceStateTable::Domain d1 = resourceStateTable->CreateDomain();
			resourceStateTable->StoreState(d1, slot0, state2);
			resourceStateTable->StoreState(d1, slot1, state3);

			uintptr_t result;
			bool success;
			success = resourceStateTable->GetState(d0, slot0, result);
			if (!success || result != state0)
				return false;

			success = resourceStateTable->GetState(d0, slot1, result);
			if (!success || result != state1)
				return false;

			success = resourceStateTable->GetState(d1, slot0, result);
			if (!success || result != state2)
				return false;

			success = resourceStateTable->GetState(d1, slot1, result);
			if (!success || result != state3)
				return false;

			resourceStateTable->RemoveDomain(d0);
			success = resourceStateTable->GetState(d0, slot1, result);
			if (success)
				return false;

			return true;
		}

		bool runTestCanonicalCmdListBuilder(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();

			JobBuilder jobBuilder(device);

			RootJob rj1 = jobBuilder.CreateRootJob();
			rj1.SetName("rj1");
			{
				DrawJob dj1 = jobBuilder.CreateDrawJob();
				dj1.AddDependency(rj1);
				dj1.SetName("R1_dj1");
				DrawJob dj2 = dj1.Next();
				dj2.SetName("R1_dj2");
				DrawJob dj3 = dj2.Next();
				dj3.SetName("R1_dj3");

				ComputeJob cj1 = jobBuilder.CreateComputeJob();
				cj1.SetName("R1_cj1");
				cj1.AddDependency(rj1);
				ComputeJob cj2 = cj1.Next();
				cj2.SetName("R1_cj2");
				cj2.AddDependency(dj2);
				ComputeJob cj2Alt = cj1.Next();
				cj2Alt.SetName("R1_Cj2Alt");
				cj2Alt.AddDependency(dj1);
			}

			RootJob rj2 = jobBuilder.CreateRootJob();
			rj2.SetName("rj2");
			{
				ComputeJob cj1 = jobBuilder.CreateComputeJob();
				cj1.SetName("R2_cj1");
				cj1.AddDependency(rj2);
				ComputeJob cj2 = cj1.Next();
				cj2.SetName("R2_cj2");

				DrawJob dj1 = jobBuilder.CreateDrawJob();
				dj1.AddDependency(rj2);
				dj1.SetName("R2_dj1");
				DrawJob dj2 = dj1.Next();
				dj2.SetName("R2_dj2");
				DrawJob dj3 = dj2.Next();
				dj3.SetName("R2_dj3");

				dj2.AddDependency(cj2);
				cj1.AddDependency(dj3);
			}

			RootJob rj3 = jobBuilder.CreateRootJob();
			rj3.SetName("rj3");
			{
				ComputeJob cj1 = jobBuilder.CreateComputeJob();
				cj1.SetName("cj1");
				cj1.AddDependency(rj3);
				ComputeJob cj2 = cj1.Next();
				cj2.SetName("cj2");

				DrawJob dj1 = jobBuilder.CreateDrawJob();
				dj1.AddDependency(rj3);
				dj1.SetName("dj1");
				DrawJob dj2 = dj1.Next();
				dj2.SetName("dj2");
				DrawJob dj3 = dj2.Next();
				dj3.SetName("dj3");
				dj3.AddDependency(cj2);
			}


			CanonicalCmdListBuilder cmdListBuilder(device->GetAllocator(), *device->GetResourceStateTable());
			auto evaluateJob = [&](RootJob& rj, unsigned expectedStaleJobs, unsigned expectedLists)
			{
				CanonicalCmdListResult result;
				cmdListBuilder.Build(rj, result);
				if (result.cmdListsCounts != expectedLists)
				{
					PG_LOG('ERR_', "Failed job result %s, expected %d got %d command lists",
						rj.GetName(), expectedLists, result.cmdListsCounts);
					return false;
				}

				if (result.staleJobCounts != expectedStaleJobs)
				{
					PG_LOG('ERR_', "Failed job result %s, expected %d got %d stale jobs",
						rj.GetName(), expectedStaleJobs, result.staleJobCounts);
					return false;
				}

				return true;
			};

			unsigned errors = 0;
			if (!evaluateJob(rj1, 0u, 7u))
				++errors;
			if (!evaluateJob(rj2, 2u, 2u))
				++errors;
			if (!evaluateJob(rj3, 0u, 3u))
				++errors;

			jobBuilder.Delete(rj1);
			jobBuilder.Delete(rj2);
			jobBuilder.Delete(rj3);

			return errors == 0u;
		}

		bool runTestAutomaticBarriers(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();

			//resources
			BufferConfig bufferConfig = {};
			bufferConfig.format = Core::FORMAT_R32_UINT;
			bufferConfig.stride = sizeof(unsigned);
			bufferConfig.elementCount = 20u;
			bufferConfig.bufferType = BufferType_Default;
			bufferConfig.bindFlags = BindFlags(BindFlags_Srv | BindFlags_Uav);

			bufferConfig.name = "TestBufferA";
			BufferRef buffA = device->CreateBuffer(bufferConfig);

			bufferConfig.name = "TestBufferB";
			BufferRef buffB = device->CreateBuffer(bufferConfig);


			bufferConfig.name = "TestBufferC";
			BufferRef buffC = device->CreateBuffer(bufferConfig);

			bufferConfig.name = "TestBufferD";
			BufferRef buffD = device->CreateBuffer(bufferConfig);

			bufferConfig.name = "TestBufferE";
			BufferRef buffE = device->CreateBuffer(bufferConfig);

			bufferConfig.name = "TestBufferF";
			BufferRef buffF = device->CreateBuffer(bufferConfig);


			JobBuilder jobBuilder(device);
			CanonicalCmdListBuilder cmdListBuilder(device->GetAllocator(), *device->GetResourceStateTable());

			RootJob rj1 = jobBuilder.CreateRootJob();
			rj1.SetName("Rj1");
			{
				auto copyAtoB = jobBuilder.CreateCopyJob();
				copyAtoB.AddDependency(rj1);
				copyAtoB.SetName("Rj1_A2B");
				copyAtoB.Set(buffA, buffB);

				auto copyBtoA = copyAtoB.Next();
				copyBtoA.SetName("Rj1_B2A");
				copyBtoA.Set(buffB, buffA);
			}

			RootJob rj2 = jobBuilder.CreateRootJob();
			rj2.SetName("Rj2");
			{
				auto copyAtoB = jobBuilder.CreateCopyJob();
				copyAtoB.AddDependency(rj2);
				copyAtoB.SetName("Rj2_A2B");
				copyAtoB.Set(buffA, buffB);

				auto copyAtoB2 = jobBuilder.CreateCopyJob();
				copyAtoB2.AddDependency(rj2);
				copyAtoB2.SetName("Rj2_A2B2");
				copyAtoB2.Set(buffA, buffB);

				auto copyBtoA = copyAtoB2.Next();
				copyBtoA.SetName("Rj2_B2A");
				copyBtoA.Set(buffB, buffA);
			}

			RootJob rj3 = jobBuilder.CreateRootJob();
			rj3.SetName("Rj3");
			{
				auto copyAtoB = jobBuilder.CreateCopyJob();
				copyAtoB.AddDependency(rj3);
				copyAtoB.SetName("Rj3_A2B");
				copyAtoB.Set(buffA, buffB);

				auto copyAtoB2 = jobBuilder.CreateCopyJob();
				copyAtoB2.AddDependency(rj3);
				copyAtoB2.AddDependency(copyAtoB);
				copyAtoB2.SetName("Rj3_A2B2");
				copyAtoB2.Set(buffA, buffB);

				auto copyBtoA = copyAtoB2.Next();
				copyBtoA.AddDependency(copyAtoB);
				copyBtoA.SetName("Rj3_B2A");
				copyBtoA.Set(buffB, buffA);
			}

			RootJob rj4 = jobBuilder.CreateRootJob();
			rj4.SetName("Rj4");
			{
				auto copyAtoB = jobBuilder.CreateCopyJob();
				copyAtoB.AddDependency(rj4);
				copyAtoB.SetName("Rj4_A2B");
				copyAtoB.Set(buffA, buffB);

				auto copyAtoC = jobBuilder.CreateCopyJob();
				copyAtoC.AddDependency(rj4);
				copyAtoC.SetName("Rj4_A2C");
				copyAtoC.Set(buffA, buffC);

				auto copyCtoA = copyAtoB.Next();
				copyCtoA.AddDependency(copyAtoC);
				copyCtoA.SetName("Rj4_C2A");
				copyCtoA.Set(buffC, buffA);
			}

			RootJob rj5 = jobBuilder.CreateRootJob();
			rj5.SetName("Rj5");
			{
				auto copyAtoB = jobBuilder.CreateCopyJob();
				copyAtoB.AddDependency(rj5);
				copyAtoB.SetName("Rj5_A2B");
				copyAtoB.Set(buffA, buffB);

				auto copyAtoC = jobBuilder.CreateCopyJob();
				copyAtoC.AddDependency(rj5);
				copyAtoC.SetName("Rj5_A2C");
				copyAtoC.Set(buffA, buffC);

				auto copyCtoA = copyAtoB.Next();
				copyCtoA.SetName("Rj5_C2A");
				copyCtoA.Set(buffC, buffA);
			}

			RootJob rj6 = jobBuilder.CreateRootJob();
			rj6.SetName("Rj6");
			{
				auto copyAtoB = jobBuilder.CreateCopyJob();
				copyAtoB.AddDependency(rj6);
				copyAtoB.SetName("Rj6_A2B");
				copyAtoB.Set(buffA, buffB);

				auto copyBtoA = copyAtoB.Next();
				copyBtoA.SetName("Rj6_B2A");
				copyBtoA.Set(buffB, buffA);

				auto copyAtoB2 = copyBtoA.Next();
				copyAtoB2.SetName("Rj6_A2B2");
				copyAtoB2.Set(buffA, buffB);

				auto copyBtoC = copyAtoB2.Next();
				copyBtoC.SetName("Rj6_B2C");
				copyBtoC.Set(buffB, buffC);

				auto copyAtoD = jobBuilder.CreateCopyJob();
				copyAtoD.AddDependency(rj6);
				copyBtoA.AddDependency(copyAtoD);
				copyAtoD.SetName("Rj6_A2D");
				copyAtoD.Set(buffA, buffD);

				auto copyDtoB = copyAtoD.Next();
				copyAtoB2.AddDependency(copyDtoB);
				copyDtoB.SetName("Rj6_D2B");
				copyDtoB.Set(buffD, buffB);
			}

			RootJob rj7 = jobBuilder.CreateRootJob();
			rj7.SetName("Rj7");
			{
				auto cAE = jobBuilder.CreateCopyJob();
				cAE.SetName("cAE");
				cAE.AddDependency(rj7);
				cAE.Set(buffA, buffE);

				auto cEC = cAE.Next();
				cEC.SetName("cEC");
				cEC.Set(buffE, buffC);

				auto cBA = cAE.Next();
				cBA.SetName("cBA");
				cBA.Set(buffB, buffA);

				auto cAB = cEC.Next();
				cAB.SetName("cAB");
				cAB.Set(buffA, buffB);
				cBA.AddDependency(cAB);

				auto cAB2 = cBA.Next();
				cAB2.SetName("cAB2");
				cAB2.Set(buffA, buffB);

				auto cBC = cAB2.Next();
				cBC.SetName("cBC");
				cBC.Set(buffB, buffC);

				auto cCD = cEC.Next();
				cCD.SetName("cCD");
				cCD.Set(buffC, buffD);

				auto cCF = cCD.Next();
				cCF.SetName("cCF");
				cCF.Set(buffC, buffF);
				cAB2.AddDependency(cCF);

				auto cCD2 = cCD.Next();
				cCD2.SetName("cCD2");
				cCD2.Set(buffC, buffD);
			}

			auto evaluateJob = [&](RootJob rj, unsigned expectedBarrierViolations, unsigned expectedCmdLists)
			{
				CanonicalCmdListResult result;
				cmdListBuilder.Build(rj, result);
				if (result.cmdListsCounts != expectedCmdLists)
				{
					PG_LOG('ERR_', "Failed job result %s, expected %d got %d command lists",
						rj.GetName(), expectedCmdLists, result.cmdListsCounts);
					return false;
				}

				if (result.barrierViolationsCount != expectedBarrierViolations)
				{
					PG_LOG('ERR_', "Failed job result %s, expected %d barrier violations got %d.",
						rj.GetName(), expectedBarrierViolations, result.barrierViolationsCount);
					return false;
				}

				return true;
			};

			unsigned errors = 0u;
			if (!evaluateJob(rj1, 0u, 2u))
				++errors;
			if (!evaluateJob(rj2, 1u, 3u))
				++errors;
			if (!evaluateJob(rj3, 0u, 4u))
				++errors;
			if (!evaluateJob(rj4, 0u, 3u))
				++errors;
			if (!evaluateJob(rj5, 1u, 3u))
				++errors;
			if (!evaluateJob(rj6, 1u, 6u))
				++errors;
			if (!evaluateJob(rj7, 1u, 9u))
				++errors;

			jobBuilder.Delete(rj1);

			return errors == 0u;
		}

		bool runTestGpuMemoryLoop(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();
			Render::BufferConfig bufferConfig = {};
			bufferConfig.bufferType = BufferType_Default;
			bufferConfig.elementCount = 16;
			bufferConfig.stride = 4;
			bufferConfig.format = Pegasus::Core::FORMAT_R32_UINT;

			bufferConfig.name = "TestUpload";
			bufferConfig.usage = Render::ResourceUsage_Dynamic;
			Render::BufferRef uploadBuff = device->CreateBuffer(bufferConfig);

			bufferConfig.name = "TestGpu";
			bufferConfig.usage = Render::ResourceUsage_Static;
			Render::BufferRef gpuBuffer = device->CreateBuffer(bufferConfig);
			Render::BufferRef gpuBuffer2 = device->CreateBuffer(bufferConfig);

			bufferConfig.name = "TestReadback";
			bufferConfig.usage = Render::ResourceUsage_Staging;
			Render::BufferRef resultBuffer = device->CreateBuffer(bufferConfig);

			if (uploadBuff->GetGpuPtr() == nullptr)
				return false;

			unsigned* integers = static_cast<unsigned*>(uploadBuff->GetGpuPtr());
			for (unsigned i = 0; i < bufferConfig.elementCount; ++i)
				integers[i] = i;

			if (resultBuffer->GetGpuPtr() == nullptr)
				return false;

			JobBuilder jb(device);
			RootJob rj = jb.CreateRootJob();
			CopyJob cj = jb.CreateCopyJob();
			cj.AddDependency(rj);
			cj.Set(uploadBuff, gpuBuffer);
			cj = cj.Next();
			cj.Set(gpuBuffer, gpuBuffer2);
			cj = cj.Next();
			cj.Set(gpuBuffer2, resultBuffer);
			Render::GpuSubmitResult result = device->Submit(rj);
			if (result.resultCode != Render::GpuWorkResultCode::Success)
				return false;

			device->Wait(result.handle);

			unsigned* resultBufferData = static_cast<unsigned*>(resultBuffer->GetGpuPtr());
			for (unsigned i = 0; i < bufferConfig.elementCount; ++i)
				if (i != resultBufferData[i])
					return false;
			device->ReleaseWork(result.handle);
			return true;
		}

		bool runTestComputeProgram(TestHarness* harness)
		{
			RenderHarness* rh = static_cast<RenderHarness*>(harness);
			IDevice* device = rh->CreateDevice();
			JobBuilder jobBuilder(device);
			GpuPipelineRef initializeBufferPipeline = device->CreateGpuPipeline();
			{
				static const char* simpleCsInitialize = R"(
				    RWBuffer<uint4> outputBuffer : register(u0);
				    cbuffer Constants : register(b0)
				    {
				        uint g_count;
				        uint g_multiplier;
				    }

				    [numthreads(32, 1, 1)]
				    void csMain(uint dti : SV_DispatchThreadID)
				    {
				        if (dti.x >= g_count)
				            return;

				        outputBuffer[dti.x] = g_multiplier * dti.x;
				    }
				)";

				GpuPipelineConfig gpuPipelineConfig;
#if PEGASUS_USE_EVENTS
				initializeBufferPipeline->SetEventListener(&rh->GetCompilerListener());
#endif

				gpuPipelineConfig.source = simpleCsInitialize;
				gpuPipelineConfig.mainNames[Pipeline_Compute] = "csMain";
				bool result = initializeBufferPipeline->Compile(gpuPipelineConfig);
				if (!result)
					return false;
			}

			auto createInitJob = [&](GpuJob parentJob, BufferRef outputBuffer, BufferRef stagingCpuReadBuffer, unsigned multiplier, unsigned elementCounts)
			{
				ResourceTableConfig initTableConfig;
				initTableConfig.type = ResourceTableType_Uav;

				initTableConfig.resources.push_back(outputBuffer);
				ResourceTableRef initTable = device->CreateResourceTable(initTableConfig);

				ComputeJob initializeJob = jobBuilder.CreateComputeJob();
				initializeJob.SetName("initializeJob");
				initializeJob.SetUavTable(0u, initTable);
				initializeJob.SetGpuPipeline(initializeBufferPipeline);
				BufferRef cbufferRef;
				struct InitializeCbuffer
				{
					unsigned count;
					unsigned multiplier;
				};
				auto* initializeCbuffer = device->CreateUploadBuffer<InitializeCbuffer>(cbufferRef);
				initializeCbuffer->count = elementCounts;
				initializeCbuffer->multiplier = multiplier;
				initializeJob.SetConstantBuffer(0u, cbufferRef);
				initializeJob.SetDispatchParams((elementCounts + 31u) / 32u, 1u, 1u);
				initializeJob.AddDependency(parentJob);


				CopyJob resultJob = jobBuilder.CreateCopyJob();
				resultJob.SetName("CopyResultJob");
				resultJob.Set(outputBuffer, stagingCpuReadBuffer);
				resultJob.AddDependency(initializeJob);

				return resultJob;
			};

			GpuPipelineRef modifyBufferPipeline = device->CreateGpuPipeline();
			{
				GpuPipelineConfig gpuPipelineConfig;
#if PEGASUS_USE_EVENTS
				modifyBufferPipeline->SetEventListener(&rh->GetCompilerListener());
#endif

				static const char* simpleCsModify = R"(
				    Buffer<uint4> input1 : register(t0, space0);
				    Buffer<uint4> input2 : register(t1, space0);
				    Buffer<uint4> input3 : register(t0, space1);
				    RWBuffer<uint4> output : register(u0, space0);
				    cbuffer Constants : register(b0)
				    {
				        uint g_counts;
				        uint g_offset;
				        uint pad0;
				        uint pad1;
				    };
				
				    [numthreads(32,1,1)]
				    void csMain(uint di : SV_DispatchThreadID)
				    {
				        if (di.x < g_counts)
				            output[di.x] = g_offset + input1[di.x] + input2[di.x] + input3[di.x];
				    }
				)";

				gpuPipelineConfig.source = simpleCsModify;
				gpuPipelineConfig.mainNames[Pipeline_Compute] = "csMain";
				bool result = modifyBufferPipeline->Compile(gpuPipelineConfig);
				if (!result)
					return false;
			}

			auto modifyBufferJob = [&](GpuJob parentJob, BufferRef inputs[3], BufferRef outputBuffer, unsigned offsetVal, unsigned elementCounts)
			{
				ResourceTableConfig inputTableConfig0;
				inputTableConfig0.type = ResourceTableType_Srv;
				inputTableConfig0.resources.push_back(inputs[0]);
				inputTableConfig0.resources.push_back(inputs[1]);
				ResourceTableRef inputTable0 = device->CreateResourceTable(inputTableConfig0);

				ResourceTableConfig inputTableConfig1;
				inputTableConfig1.type = ResourceTableType_Srv;
				inputTableConfig1.resources.push_back(inputs[2]);
				ResourceTableRef inputTable1 = device->CreateResourceTable(inputTableConfig1);

				ResourceTableConfig outputTableConfig;
				outputTableConfig.type = ResourceTableType_Uav;
				outputTableConfig.resources.push_back(outputBuffer);
				ResourceTableRef outputTable = device->CreateResourceTable(outputTableConfig);

				ComputeJob modifyBufferJob = jobBuilder.CreateComputeJob();
				modifyBufferJob.SetName("initializeJob");
				modifyBufferJob.SetResourceTable(0u, inputTable0);
				modifyBufferJob.SetResourceTable(1u, inputTable1);
				modifyBufferJob.SetUavTable(0u, outputTable);
				modifyBufferJob.SetGpuPipeline(modifyBufferPipeline);
				BufferRef cbufferRef;
				struct ModifyBufferConstants
				{
					unsigned count;
					unsigned offset;
				};
				auto* constants = device->CreateUploadBuffer<ModifyBufferConstants>(cbufferRef);
				constants->count = elementCounts;
				constants->offset = offsetVal;
				modifyBufferJob.SetConstantBuffer(0u, cbufferRef);
				modifyBufferJob.SetDispatchParams((elementCounts + 31u) / 32u, 1u, 1u);
				modifyBufferJob.AddDependency(parentJob);

				return modifyBufferJob;
			};

            RootJob r = jobBuilder.CreateRootJob();
            r.SetName("ComputeTest");


            unsigned elementCounts = 20u;
            unsigned multiplier = 2u;
			BufferConfig bufferConfig = {};
            bufferConfig.name = "inputBuffer";
			bufferConfig.format = Core::FORMAT_RGBA_32_UINT;
			bufferConfig.stride = sizeof(unsigned) * 4;
			bufferConfig.elementCount = elementCounts;
			bufferConfig.bufferType = BufferType_Default;
			bufferConfig.bindFlags = BindFlags(BindFlags_Srv | BindFlags_Uav);

            BufferRef inputBuffer = device->CreateBuffer(bufferConfig);
			BufferRef inputBuffer2 = device->CreateBuffer(bufferConfig);
			BufferRef inputBuffer3 = device->CreateBuffer(bufferConfig);
			BufferRef modifiedOutputBuffer = device->CreateBuffer(bufferConfig);

			BufferConfig resultBufferConfig = bufferConfig;
			resultBufferConfig.name = "TestReadback";
			resultBufferConfig.usage = Render::ResourceUsage_Staging;
			resultBufferConfig.bindFlags = 0u;
			Render::BufferRef resultBuffer = device->CreateBuffer(resultBufferConfig);
			Render::BufferRef resultBuffer2 = device->CreateBuffer(resultBufferConfig);
			Render::BufferRef resultBuffer3 = device->CreateBuffer(resultBufferConfig);
			Render::BufferRef modifiedResultBuffer = device->CreateBuffer(resultBufferConfig);

            GpuJob initJob = createInitJob(r, inputBuffer, resultBuffer, multiplier, elementCounts);
			GpuJob initJob2 = createInitJob(r, inputBuffer2, resultBuffer2, multiplier, elementCounts);
			GpuJob initJob3 = createInitJob(r, inputBuffer3, resultBuffer3, multiplier, elementCounts);
			BufferRef inputBuffers[] = { inputBuffer, inputBuffer2, inputBuffer3 };
			unsigned modifiedOffset = 16;
			GpuJob modJob = modifyBufferJob(r, inputBuffers, modifiedOutputBuffer, modifiedOffset, elementCounts);
			modJob.AddDependency(initJob);
			modJob.AddDependency(initJob2);
			modJob.AddDependency(initJob3);

			CopyJob finalJob = jobBuilder.CreateCopyJob();
			finalJob.AddDependency(modJob);
			finalJob.Set(modifiedOutputBuffer, modifiedResultBuffer);

            GpuSubmitResult jobResult = device->Submit(r);
            if (jobResult.resultCode != GpuWorkResultCode::Success)
                return false;

            device->Wait(jobResult.handle);

			auto verifyBuffer = [&](BufferRef stagingBuffer, unsigned elementCounts, unsigned multipliers)
			{
				for (unsigned el = 0; el < elementCounts; ++el)
				{
					struct V { unsigned x, y, z, w; };
					auto& v = ((V*)stagingBuffer->GetGpuPtr())[el];
					unsigned expected = multiplier * el;
					if (expected != v.x || expected != v.y || expected != v.z || expected != v.w)
						return false;
				}

				return true;
			};

			if (!verifyBuffer(resultBuffer, elementCounts, multiplier))
				return false;
			if (!verifyBuffer(resultBuffer2, elementCounts, multiplier))
				return false;
			if (!verifyBuffer(resultBuffer3, elementCounts, multiplier))
				return false;

			for (unsigned el = 0; el < elementCounts; ++el)
			{
				struct V { unsigned x, y, z, w; };
				auto& v = ((V*)modifiedResultBuffer->GetGpuPtr())[el];
				unsigned expected = 3u*(multiplier * el) + modifiedOffset;
				if (expected != v.x || expected != v.y || expected != v.z || expected != v.w)
					return false;
			}

			return true;
		}

		TestHarness* createRenderHarness()
		{
			return new RenderHarness();
		}

		const TestFunc gRenderTests[] = {
			DECLARE_TEST(CreateDevice),
			DECLARE_TEST(CreateSimpleGpuPipeline),
			DECLARE_TEST(TestSimpleCompute),
			DECLARE_TEST(TestResourceStateTable),
			DECLARE_TEST(TestCanonicalCmdListBuilder),
			DECLARE_TEST(TestAutomaticBarriers),
			DECLARE_TEST(TestGpuMemoryLoop),
			DECLARE_TEST(TestComputeProgram),
			DECLARE_TEST(DestroyDevice)
		};

		const TestFunc* RenderHarness::GetTests(int& funcSz) const
		{
			funcSz = sizeof(gRenderTests) / sizeof(gRenderTests[0]);
			return gRenderTests;
		}

	}
}
