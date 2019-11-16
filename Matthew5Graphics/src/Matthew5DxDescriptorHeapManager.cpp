#include "Matthew5DxDescriptorHeapManager.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		Matthew5DxDescriptorHeapManager::Matthew5DxDescriptorHeapManager(D3D12_DESCRIPTOR_HEAP_DESC & vrD3D12_DESCRIPTOR_HEAP_DESC, 
			ID3D12Device & vrID3D12Device)
		{
			mCurrentCounterElementIndex = 0;

			HRESULT vHr = vrID3D12Device.CreateDescriptorHeap(&vrD3D12_DESCRIPTOR_HEAP_DESC, IID_PPV_ARGS(&mpID3D12DescriptorHeap));

			if (vHr != S_OK)
			{
				throw new std::exception("Error - Matthew5DxDescriptorHeapManager");
			}

			mElementSize = vrID3D12Device.GetDescriptorHandleIncrementSize(vrD3D12_DESCRIPTOR_HEAP_DESC.Type);

			mTotalNumberOfElements = vrD3D12_DESCRIPTOR_HEAP_DESC.NumDescriptors;

			mNextCD3DX12_CPU_DESCRIPTOR_HANDLE = CD3DX12_CPU_DESCRIPTOR_HANDLE(mpID3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());


		}

		Matthew5DxDescriptorHeapManager::~Matthew5DxDescriptorHeapManager()
		{
			mpID3D12DescriptorHeap->Release();
		}

		void Matthew5DxDescriptorHeapManager::ResetCounter(void)
		{
			mCurrentCounterElementIndex = 0;
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE Matthew5DxDescriptorHeapManager::GetNextUsingCounter()
		{
			if (mCurrentCounterElementIndex >= mTotalNumberOfElements)
			{
				throw new std::exception("The total number of elements has been hit!");
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE vNewCD3DX12_CPU_DESCRIPTOR_HANDLE = mNextCD3DX12_CPU_DESCRIPTOR_HANDLE;
			mNextCD3DX12_CPU_DESCRIPTOR_HANDLE.Offset(mElementSize);
			mCurrentCounterElementIndex++;
			return vNewCD3DX12_CPU_DESCRIPTOR_HANDLE;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE Matthew5DxDescriptorHeapManager::GetCpuHandletAtIndex(int32_t vIndex)
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(
				mpID3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
				vIndex,
				mElementSize);
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE Matthew5DxDescriptorHeapManager::GetGpuHandletAtIndex(int32_t vIndex)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE vGpuHandle(mpID3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			vGpuHandle.Offset(vIndex, mElementSize);

			return vGpuHandle;
		}


	}
}