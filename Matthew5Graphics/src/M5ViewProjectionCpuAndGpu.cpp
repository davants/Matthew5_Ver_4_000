#include "M5ViewProjectionCpuAndGpu.h"
namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5ViewProjectionCpuAndGpu::M5ViewProjectionCpuAndGpu(ID3D12Device & vrpID3D12Device) :
			mM5ViewProjectionGpu(vrpID3D12Device)
		{
		}

		D3D12_GPU_VIRTUAL_ADDRESS M5ViewProjectionCpuAndGpu::GetGpuConstBufferAddress(ENUMM5Eye vENUMM5Eye)
		{
			return mM5ViewProjectionGpu.GetM5DxGpuMemoryUploadBuffer(vENUMM5Eye).GetID3D12Resource().GetGPUVirtualAddress();
		}

		M5ViewProjectionCpu & M5ViewProjectionCpuAndGpu::GetCpuViewProjection(void)
		{
			return mM5ViewProjectionCpu;
		}

		void M5ViewProjectionCpuAndGpu::UpdateGpuWithCpuData(ENUMM5Eye vENUMM5Eye)
		{
			mM5ViewProjectionGpu.UpdateDataOnGpua(mM5ViewProjectionCpu, vENUMM5Eye);
		}		

	}
}