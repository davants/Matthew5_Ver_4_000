#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5Util.h"
#include "M5SceneObjectCpuPosition.h"
#include "M5DxGpuMemoryUploadBuffer.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This struct has the same data layout as the Shader variable "cbPerObject".
		struct M5SceneObjectPrepareGpuData
		{		
			DirectX::XMFLOAT4X4 mWorldSpace = M5Util::Identity();
		};

		// This object takes Cpu data and puts it on the Gpu.
		class M5SceneObjectGpuPosition
		{
		public:
			// Constructor needs to use the ID3D12Device.
			M5SceneObjectGpuPosition(ID3D12Device & vrpID3D12Device);

			// See function definition for notes.
			void UpdateDataOnGpu(M5SceneObjectCpuPosition & vrM5SceneObjectCpuPosition);

			M5DxGpuMemoryUploadBuffer & GetM5DxGpuMemoryUploadBuffer(void);


		private:
			std::unique_ptr<M5DxGpuMemoryUploadBuffer> mpM5DxGpuMemoryUploadBuffer;

			ID3D12Device & mrpID3D12Device;

			M5SceneObjectPrepareGpuData mM5SceneObjectPrepareGpuData;			

		};

	}
}



