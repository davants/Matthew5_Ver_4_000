#include "M5DxGpuCommand.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5DxGpuCommand::M5DxGpuCommand(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, size_t vNumberOfCommandLists):
			mrM5DxGpuDeviceNFactory(vrM5DxGpuDeviceNFactory)
		{

			// GPU Command Queue
			D3D12_COMMAND_QUEUE_DESC vD3D12_COMMAND_QUEUE_DESC = {};
			vD3D12_COMMAND_QUEUE_DESC.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			vD3D12_COMMAND_QUEUE_DESC.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			ThrowIfFail(mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateCommandQueue(&vD3D12_COMMAND_QUEUE_DESC,
				IID_PPV_ARGS(&mpID3D12CommandQueue)));

			// Default is 1 allocator and command list.
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> vpID3D12CommandAllocator;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> vpID3D12GraphicsCommandList;
			
			for (int i = 0; i < vNumberOfCommandLists; i++)
			{
				// GPU Command Allocator
				ThrowIfFail(mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(vpID3D12CommandAllocator.GetAddressOf())));
				mpID3D12CommandAllocator.push_back(vpID3D12CommandAllocator);

				ID3D12PipelineState * vpPipelineStateObject = nullptr;

				// GPU Command List
				ThrowIfFail(mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateCommandList(
					0,
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					vpID3D12CommandAllocator.Get(),
					vpPipelineStateObject,
					IID_PPV_ARGS(vpID3D12GraphicsCommandList.GetAddressOf())));

				vpID3D12GraphicsCommandList->Close();

				mpID3D12GraphicsCommandList.push_back(vpID3D12GraphicsCommandList);
			}


			// Fence
			ThrowIfFail(mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateFence(0, D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(&mpID3D12Fence)));

		}

		ID3D12CommandQueue & M5DxGpuCommand::GetCommandQueue(void)
		{
			return *(mpID3D12CommandQueue.Get());
		}


		ID3D12CommandAllocator & M5DxGpuCommand::GetCommandAllocator(size_t vIndex)
		{
			return *(mpID3D12CommandAllocator[vIndex].Get());

		}

		ID3D12GraphicsCommandList & M5DxGpuCommand::GetCommandList(size_t vIndex)
		{
			return *(mpID3D12GraphicsCommandList[vIndex].Get());
		}

		// The Command Queue has a bunch of work items to do. It contains a list of things to do.
		// The Command Queue will do one work item at a time in the order they were added.
		// Fence: By adding a number as a work item, int, to the Queue; we can wait until it gets 
		//		processes at which time we know all the items that were added before must have been 
		//		completed because things are done in order. 
		void M5DxGpuCommand::EmptyCommandQueueUsingFence(void)
		{
			// Move the fence value forward by one.
			mCurrentFenceNumber++;

			ThrowIfFail(GetCommandQueue().Signal(mpID3D12Fence.Get(), mCurrentFenceNumber));

			// Check to see if the fence number on the Gpu is greater than the current fence number.
			// Why is this check here? It could have happened so fast that it is already done.
			UINT64 vGpuNumber = mpID3D12Fence->GetCompletedValue();
			if (mCurrentFenceNumber > vGpuNumber)
			{
				HANDLE vEventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

				// Set the GPU to send event when it reaches the current fence number.
				ThrowIfFail(mpID3D12Fence->SetEventOnCompletion(mCurrentFenceNumber, vEventHandle));

				// Block the current thread and wait for the Gpu to send an event letting us know 
				// the number has been reached.
				WaitForSingleObject(vEventHandle, INFINITE);
				CloseHandle(vEventHandle);
			}

		}

		void M5DxGpuCommand::ThrowIfFail(HRESULT hr)
		{
			if (hr != S_OK)
			{
				throw new std::exception("Error - M5DxGpuCommand");
			}
		}

	}
}