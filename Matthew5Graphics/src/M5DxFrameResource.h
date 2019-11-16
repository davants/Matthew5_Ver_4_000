#pragma once

// Added for Version 2.000

#include "M5DxGpuDeviceNFactory.h"
#include "M5ViewProjectionCpuAndGpu.h"
#include "M5DxFrameSceneObjectList.h"
#include "M5DxGpuCommand.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		class M5DxFrameResource
		{
		public:
			M5DxFrameResource(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, ID3D12CommandQueue & vrID3D12CommandQueue);

			M5ViewProjectionCpuAndGpu & GetM5ViewProjectionCpuAndGpu(void);
			M5DxFrameSceneObjectList & GetSceneObjectList(void);

			void SetTheFence(void);
			void WaitForFence(void);

		private:
			// The view projection can be thought of as the camera's point of view and an empty picture frame.
			// It includes both the camera's point of view and the picture frame the camera is looking through.
			// Imaging someone holding a camera in one hand and an outstretched empty picture frame in the 
			// other hand; the camera is pointed at the frame.
			// What the camera can see through the picture frame is the view projection.
			std::unique_ptr<M5ViewProjectionCpuAndGpu> mpM5ViewProjectionCpuAndGpu;


			// Added so the client can have more than one scene object.
			M5DxFrameSceneObjectList mM5DxFrameSceneObjectList;

			// Fence variables
			Microsoft::WRL::ComPtr<ID3D12Fence> mpID3D12Fence;
			UINT64 mFence;

			ID3D12CommandQueue & mrID3D12CommandQueue;


			void ThrowIfFail(HRESULT hr);

		};
	}
}