#include "M5SceneObjectGpuPosition.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{

		M5SceneObjectGpuPosition::M5SceneObjectGpuPosition(ID3D12Device & vrpID3D12Device) :
			mrpID3D12Device(vrpID3D12Device)
		{
			// To create the const buffer on the Gpu, we will set the data to an empty value.
			M5SceneObjectPrepareGpuData vDefaultValue;

			mpM5DxGpuMemoryUploadBuffer = std::make_unique< M5DxGpuMemoryUploadBuffer>(mrpID3D12Device,
				&vDefaultValue,
				sizeof(M5SceneObjectPrepareGpuData));

		}

		void M5SceneObjectGpuPosition::UpdateDataOnGpu(M5SceneObjectCpuPosition & vrM5SceneObjectCpuPosition)
		{			
			// The M5SceneObjectPrepareGpuData class has data layed out in the exact same 
			// way as the cooresponding Shader variable named "cbPerObject".

			memcpy(&mM5SceneObjectPrepareGpuData.mWorldSpace, &vrM5SceneObjectCpuPosition, sizeof(DirectX::XMFLOAT4X4));

			// Now copy to Gpu.  This is actually copying the data over at this moment!
			// At this moment means Now! Notice in the "Update(..)" function there is 
			// no command list being used, just memcpy.
			mpM5DxGpuMemoryUploadBuffer->Update(&mM5SceneObjectPrepareGpuData);			
		}

		M5DxGpuMemoryUploadBuffer & M5SceneObjectGpuPosition::GetM5DxGpuMemoryUploadBuffer(void)
		{
			return *(mpM5DxGpuMemoryUploadBuffer.get());
		}

	}
}