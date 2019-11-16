#pragma once

#include <vector>
#include "Matthew5StandardMicrosoftHeaders.h"

#include "M5DxGpuDeviceNFactory.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This holds one command queue and N number of command lists and allocators.
		class M5DxGpuCommand
		{
		public:
			M5DxGpuCommand(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, size_t vNumberOfCommandLists);

			ID3D12CommandQueue & GetCommandQueue(void);

			ID3D12CommandAllocator & GetCommandAllocator(size_t vIndex);
			ID3D12GraphicsCommandList & GetCommandList(size_t vIndex);

			void EmptyCommandQueueUsingFence(void);

		private:
			M5DxGpuDeviceNFactory & mrM5DxGpuDeviceNFactory;

			// One Queue that can hold many command lists/allocators.
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> mpID3D12CommandQueue;


			std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mpID3D12CommandAllocator;
			std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> mpID3D12GraphicsCommandList;
					   
			// Fences are about command lists, checking to see if commands placed in a command list 
			// have finished.
			Microsoft::WRL::ComPtr<ID3D12Fence> mpID3D12Fence;
			UINT64 mCurrentFenceNumber = 0;

			void ThrowIfFail(HRESULT hr);
			
		};
	}
}