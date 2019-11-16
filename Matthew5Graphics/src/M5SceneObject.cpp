#include "M5SceneObject.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{

		M5SceneObject::M5SceneObject(ID3D12Device & vrpID3D12Device):
			mM5SceneObjectCpuAndGpuPosition(vrpID3D12Device),
			mUpdateCounter(0)
		{
		}

		D3D12_GPU_VIRTUAL_ADDRESS M5SceneObject::GetGpuConstBufferAddressThatHasWorldSpaceNMoreData(void)
		{
			return mM5SceneObjectCpuAndGpuPosition.mM5SceneObjectGpuPosition.GetM5DxGpuMemoryUploadBuffer().GetID3D12Resource().GetGPUVirtualAddress();
		}

		M5TriCpuGpuResources & M5SceneObject::GetM5TriCpuGpuResources(void)
		{
			return *(mpM5TriCpuGpuResources.get());
		}

		void M5SceneObject::SetM5TriCpuGpuResources(std::unique_ptr<M5TriCpuGpuResources> vpM5TriCpuGpuResources)
		{
			mpM5TriCpuGpuResources = std::move(vpM5TriCpuGpuResources);
		}

		void M5SceneObject::UpdatePosition(M5SceneObjectCpuPosition vM5SceneObjectCpuPosition,
			uint64_t vUpdateCounter)
		{
			mUpdateCounter = vUpdateCounter;

			mM5SceneObjectCpuAndGpuPosition.mM5SceneObjectCpuPosition = vM5SceneObjectCpuPosition;

			mM5SceneObjectCpuAndGpuPosition.mM5SceneObjectGpuPosition.UpdateDataOnGpu(
				mM5SceneObjectCpuAndGpuPosition.mM5SceneObjectCpuPosition);
		}

		M5SceneObjectCpuAndGpuPosition & M5SceneObject::GetM5SceneObjectCpuAndGpuPosition(void)
		{
			return mM5SceneObjectCpuAndGpuPosition;
		}

		const uint64_t & M5SceneObject::GetUpdateCounter(void)
		{
			return mUpdateCounter;
		}

	}
}