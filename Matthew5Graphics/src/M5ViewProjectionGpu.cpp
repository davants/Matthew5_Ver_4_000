#include "M5ViewProjectionGpu.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		M5ViewProjectionGpu::M5ViewProjectionGpu(ID3D12Device & vrpID3D12Device) :
			mrpID3D12Device(vrpID3D12Device)
		{
			// To create the const buffer on the Gpu, we will set the data to an empty value.
			M5ViewProjectionPrepareGpuData vDefaultValue;

			mpM5DxGpuMemoryUploadBuffer[M5LeftEye] = std::make_unique< M5DxGpuMemoryUploadBuffer>(mrpID3D12Device,
				&vDefaultValue,
				sizeof(M5ViewProjectionPrepareGpuData));

			mpM5DxGpuMemoryUploadBuffer[M5RightEye] = std::make_unique< M5DxGpuMemoryUploadBuffer>(mrpID3D12Device,
				&vDefaultValue,
				sizeof(M5ViewProjectionPrepareGpuData));
		}

		void M5ViewProjectionGpu::UpdateDataOnGpua(M5ViewProjectionCpu & vrM5ViewProjectionCpu, ENUMM5Eye vENUMM5Eye)
		{
			// The M5SceneObjectPrepareGpuData class has data layed out in the exact same 
			// way as the cooresponding Shader variable named "cbCamera".
			// So first update our Cpu "Prepare" object and then copy to the Gpu.

			// Update our "Prepare for Gpu" object.

			mM5ViewProjectionPrepareGpuData[vENUMM5Eye].mViewProjection = vrM5ViewProjectionCpu.GetViewProjection(vENUMM5Eye); 
			
			// Now copy to Gpu.  This is actually copying the data over at this moment!
			// At this moment means Now! Notice in the "Update(..)" function there is 
			// no command list being used, just the old C language memcpy.
			mpM5DxGpuMemoryUploadBuffer[vENUMM5Eye]->Update(&mM5ViewProjectionPrepareGpuData[vENUMM5Eye]);

		}

		M5DxGpuMemoryUploadBuffer & M5ViewProjectionGpu::GetM5DxGpuMemoryUploadBuffer(ENUMM5Eye vENUMM5Eye)
		{
			return *(mpM5DxGpuMemoryUploadBuffer[vENUMM5Eye].get());
		}

	}
}