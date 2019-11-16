#pragma once
#include "Matthew5StandardMicrosoftHeaders.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// A Gpu descriptor heap is just an array on the GPU. Because it is an 
		// array, it can also be through of as a blob of memory.
		// Each element in the array is a descriptor waiting to be populated.
		// This is a simple wrapper around an allocated Gpu heap of descriptors.
		// This allows the client to call "GetNextUsingCounter()" and this wrapper will 
		// return the next CD3DX12_CPU_DESCRIPTOR_HANDLE in the heap. If the client want 
		// to go back to the beginning they can call "ResetCounter()".
		// There are also 2 functions that allow the client to get at things by supplying 
		// in index.
		class Matthew5DxDescriptorHeapManager
		{
		public:
			// This constructor will allocate the needed heap on the Gpu.
			Matthew5DxDescriptorHeapManager(D3D12_DESCRIPTOR_HEAP_DESC & vrD3D12_DESCRIPTOR_HEAP_DESC,
				ID3D12Device & vrID3D12Device);

			~Matthew5DxDescriptorHeapManager();

			// Reset back to the front of the descriptor heap.
			void ResetCounter(void);
			CD3DX12_CPU_DESCRIPTOR_HANDLE GetNextUsingCounter();

			D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandletAtIndex(int32_t vIndex);

			CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandletAtIndex(int32_t vIndex);

		private:
			ID3D12DescriptorHeap*         mpID3D12DescriptorHeap;

			// The size of each element in the array is determined by the type of descriptor and the 
			// Gpu hardware. In the constructor we ask the Gpu how big each element is.
			UINT mElementSize;
			UINT mTotalNumberOfElements;
			UINT mCurrentCounterElementIndex;
			CD3DX12_CPU_DESCRIPTOR_HANDLE mNextCD3DX12_CPU_DESCRIPTOR_HANDLE;

		};

	}
}