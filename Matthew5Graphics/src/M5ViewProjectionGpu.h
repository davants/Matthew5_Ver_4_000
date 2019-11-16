#pragma once
#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5Util.h"
#include "M5ViewProjectionCpu.h"
#include "M5DxGpuMemoryUploadBuffer.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This struct has the same data layout as the Shader variable "cbPerObject".
		// This supports 2 eyes in case where a HMD is being used. If only one camera 
		// is needed then just use the left eye.

		struct M5ViewProjectionPrepareGpuData
		{
			DirectX::XMFLOAT4X4 mViewProjection = M5Util::Identity();
		};

		// This always supports 2 eyes/cameras. 
		// If in desktop mode then only use one of the two.
		// If in Virtual Reality mode then you will use both.
		class M5ViewProjectionGpu
		{
		public:
			// Constructor needs to use the ID3D12Device.
			M5ViewProjectionGpu(ID3D12Device & vrpID3D12Device);

			// See function definition for notes.
			void UpdateDataOnGpua(M5ViewProjectionCpu & vrM5ViewProjectionCpu, ENUMM5Eye vENUMM5Eye);

			M5DxGpuMemoryUploadBuffer & GetM5DxGpuMemoryUploadBuffer(ENUMM5Eye vENUMM5Eye);

		private:
			std::unique_ptr<M5DxGpuMemoryUploadBuffer> mpM5DxGpuMemoryUploadBuffer[2];

			ID3D12Device & mrpID3D12Device;

			M5ViewProjectionPrepareGpuData mM5ViewProjectionPrepareGpuData[2];
		};

	}
}