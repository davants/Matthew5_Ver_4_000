#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5DxGpuCommand.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This class handles allocation of memory resources on the Gpu.
		// This class now supports allocating Gpu memory in two places, the update buffer and the 
		// default buffer. The update buffer allows for data to be changed. The default buffer 
		// does not allow for the data to be changed. For example, the geometry of a chair will not 
		// change over its lifetime so this should go in the default buffer. The default buffer is 
		// more efficient than the update buffer if all you do is read from it. However, for the 
		// location of the chair the update buffer is more efficent because it must change and 
		// changing the default buffer is expensive.
		class M5DxGpuMemoryUploadBuffer
		{
		public:
			// Use this constructor for the Update buffer. The update buffer is designed to be 
			// changed often.
			M5DxGpuMemoryUploadBuffer(ID3D12Device & vrID3D12Device, 
				const void * vpRawDataToCopyToGpu,
				size_t vBufferSize);

			// Added for version 3.000
			// This constructor is used to add the given data to the "Gpu default buffer".
			// If this constructor is used, the client should NOT call the "Update" function.
			M5DxGpuMemoryUploadBuffer(ID3D12Device& vrID3D12Device,
				const void* vpRawDataToCopyToGpu,
				size_t vBufferSize, 
				M5DxGpuCommand& vrM5DxGpuCommand);


			~M5DxGpuMemoryUploadBuffer();

			// Update the Gpu allocated memory with this new data. 
			// The size of the vpRawDataToCopyToGpu in Cpu memory must be the same as it was 
			// originally allocated.
			// This is actually copying the data over at this moment!
			// "At this moment" means Now! Notice there is no command list being used, just the 
			// C++ function "memcpy".
			void Update(const void * vpRawDataToCopyToGpu);

			ID3D12Resource & GetID3D12Resource(void);

			size_t GetBufferSize(void);

		private:
			size_t mBufferSize;

			Microsoft::WRL::ComPtr<ID3D12Resource> mpID3D12Resource;

			UINT8* mpBufferHead;

			// Added for version 3.000
			// This will let the destructor know what to deallocate.
			bool mIsUsingDefaultBuffer;
		};


	}
}
