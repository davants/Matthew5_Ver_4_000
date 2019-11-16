#include "M5DxFrameResource.h"
namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5DxFrameResource::M5DxFrameResource(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, ID3D12CommandQueue & vrID3D12CommandQueue)
			: mrID3D12CommandQueue(vrID3D12CommandQueue)
		{
			// New Camera view projection.
			mpM5ViewProjectionCpuAndGpu = std::make_unique<M5ViewProjectionCpuAndGpu>(vrM5DxGpuDeviceNFactory.GetID3D12Device());

			// Setup the Fence 
			mFence = 0;

			ThrowIfFail(vrM5DxGpuDeviceNFactory.GetID3D12Device().CreateFence(0, D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(&mpID3D12Fence)));
			
			
		}

		M5ViewProjectionCpuAndGpu & M5DxFrameResource::GetM5ViewProjectionCpuAndGpu(void)
		{
			return *mpM5ViewProjectionCpuAndGpu;
		}

		M5DxFrameSceneObjectList & M5DxFrameResource::GetSceneObjectList(void)
		{
			return mM5DxFrameSceneObjectList;
		}

		void M5DxFrameResource::SetTheFence(void)
		{
			mFence++;
			ThrowIfFail(mrID3D12CommandQueue.Signal(mpID3D12Fence.Get(), mFence));
		}

		void M5DxFrameResource::WaitForFence(void)
		{			
			// Maybe we don't have to wait. First get the fence and check its value.
			UINT64 vGpuFenceNumber = mpID3D12Fence->GetCompletedValue();

			// The Gpu fence number should be as big as our mFence that was used in the "SetTheFence" 
			// function. If it is is less, it means the Gpu has not gotten to it yet and we need to wait.
			if (mFence > vGpuFenceNumber)
			{
				// Gpu number is not as big as the last fence added so it has not cleared.
				// So we need to wait here.
				 
				HANDLE vEventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

				// Set the GPU to send event when it reaches the current fence number.
				ThrowIfFail(mpID3D12Fence->SetEventOnCompletion(mFence, vEventHandle));

				// Block the current thread and wait for the Gpu to send an event letting us know 
				// the number has been reached.
				WaitForSingleObject(vEventHandle, INFINITE);
				CloseHandle(vEventHandle);
			}
		}

		void M5DxFrameResource::ThrowIfFail(HRESULT hr)
		{
			if (hr != S_OK)
			{
				throw new std::exception("Error - M5DxFrameResource");
			}

		}

	}
}