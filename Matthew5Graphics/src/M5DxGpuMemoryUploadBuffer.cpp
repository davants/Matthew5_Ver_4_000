#include "M5DxGpuMemoryUploadBuffer.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{


		M5DxGpuMemoryUploadBuffer::M5DxGpuMemoryUploadBuffer(ID3D12Device & vrID3D12Device, 
			const void * vpRawDataToCopyToGpu, 
			size_t vBufferSize):			
			mBufferSize(vBufferSize)
		{
			// This constructor is for the Gpu Upload buffer.


			// Added the mIsUsingDefaultBuffer variable for Version 3.000 so the destructor 
			// will know what to do.
			mIsUsingDefaultBuffer = false;

			CD3DX12_HEAP_PROPERTIES vCD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC vCD3DX12_RESOURCE_DESC = CD3DX12_RESOURCE_DESC::Buffer(mBufferSize);

			// Allocate Gpu memory and setup the vrpID3D12Resource parameter.
			HRESULT hr = vrID3D12Device.CreateCommittedResource(
				&vCD3DX12_HEAP_PROPERTIES,
				D3D12_HEAP_FLAG_NONE,
				&vCD3DX12_RESOURCE_DESC,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mpID3D12Resource));

			if (FAILED(hr))
			{
				throw new std::exception("Error - GPU allocation failed.");
			}

			// Copy the Cpu data to the Gpu allocated memory.
			// The "Map(...)" function kind of "opens the door" for memory to be copied from 
			// the Cpu to the Gpu and "Unmap(...)" closes the door after the copy is done.			
			hr = mpID3D12Resource->Map(0, nullptr, reinterpret_cast<void**>(&mpBufferHead));

			if (FAILED(hr))
			{
				throw new std::exception("error in AllocateGpuMemoryAndCopyDataToIt");
			}

			Update(vpRawDataToCopyToGpu);

			// Don't close the door, meaning don't call mpID3D12Resource->Unmap(..), leave it open 
			// for continual updating as needed. It will not be closed until the destructor.
		}

		M5DxGpuMemoryUploadBuffer::M5DxGpuMemoryUploadBuffer(ID3D12Device& vrID3D12Device, 
			const void* vpRawDataToCopyToGpu, size_t vBufferSize, M5DxGpuCommand& vrM5DxGpuCommand) :
			mBufferSize(vBufferSize)
		{
			// This constructor was added for Version 3.000 to support the Gpu Default Buffer.
			// The Gpu default buffer can be created from C++, but it can not be written to directly 
			// in C++. We must create a temp Gpu Upload buffer that we can write to and then give the Gpu 
			// "Copy" commands to copy from the Gpu Upload buffer to the Gpu default buffer.

			// Added the mIsUsingDefaultBuffer variable for Version 3.000 so the destructor 
			// will know what to do.
			mIsUsingDefaultBuffer = true;


			// Allocate the "Gpu Default buffer". Notice the "D3D12_HEAP_TYPE_DEFAULT".
			HRESULT hr = vrID3D12Device.CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mBufferSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(mpID3D12Resource.GetAddressOf()));

			if (FAILED(hr))
			{
				throw new std::exception("Error - GPU allocation failed.");
			}

			// We must use a command list to do the work of copying over data from 
			// the Gpu Upload buffer to the Gpu Default buffer.
			// Make sure everything in the command list is done.
			vrM5DxGpuCommand.EmptyCommandQueueUsingFence();

			// We will use command list and allocator at index zero.
			int vAvailableFrameResourceIndex = 0;

			hr = vrM5DxGpuCommand.GetCommandAllocator(vAvailableFrameResourceIndex).Reset();
			if (FAILED(hr))
			{
				throw new std::exception("Error - Upload And Empty CommandQueue.");
			}

			// Reset the command list, giving it the command allocator. 
			hr = vrM5DxGpuCommand.GetCommandList(vAvailableFrameResourceIndex).Reset(
				&(vrM5DxGpuCommand.GetCommandAllocator(vAvailableFrameResourceIndex)), nullptr);

			if (FAILED(hr))
			{
				throw new std::exception("Error - Upload And Empty CommandQueue.");
			}

			// Create our temp Gpu Upload buffer. Notice the word D3D12_HEAP_TYPE_UPLOAD.
			Microsoft::WRL::ComPtr<ID3D12Resource> vpID3D12ResourceUploadBuffer;

			CD3DX12_HEAP_PROPERTIES vCD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC vCD3DX12_RESOURCE_DESC = CD3DX12_RESOURCE_DESC::Buffer(mBufferSize);

			hr = vrID3D12Device.CreateCommittedResource(
				&vCD3DX12_HEAP_PROPERTIES,
				D3D12_HEAP_FLAG_NONE,
				&vCD3DX12_RESOURCE_DESC,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&vpID3D12ResourceUploadBuffer));

			if (FAILED(hr))
			{
				throw new std::exception("Error - GPU allocation failed.");
			}

			// We will use the DirectX helper function "UpdateSubresources" to do several steps.
			// It will copy data to the "Gpu Upload buffer" and then add commands to the command list 
			// that will copy from "Gpu Upload buffer" to the "Gpu Default buffer".

			D3D12_SUBRESOURCE_DATA vD3D12_SUBRESOURCE_DATA = {};
			vD3D12_SUBRESOURCE_DATA.pData = vpRawDataToCopyToGpu;
			vD3D12_SUBRESOURCE_DATA.RowPitch = mBufferSize;
			vD3D12_SUBRESOURCE_DATA.SlicePitch = vD3D12_SUBRESOURCE_DATA.RowPitch;

			vrM5DxGpuCommand.GetCommandList(0).ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
				mpID3D12Resource.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

			UpdateSubresources<1>(&(vrM5DxGpuCommand.GetCommandList(0)),
				mpID3D12Resource.Get(),
				vpID3D12ResourceUploadBuffer.Get(), 0, 0, 1, &vD3D12_SUBRESOURCE_DATA);

			vrM5DxGpuCommand.GetCommandList(0).ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
				mpID3D12Resource.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));


			vrM5DxGpuCommand.GetCommandList(0).Close();

			// Add the command list recorded commands to the command queue for execution at some future point in time.
			// Analogy: Put the cassette tape in the pile of cassettes to be played at some future point in time.
			ID3D12CommandList* cmdsLists[] = { &vrM5DxGpuCommand.GetCommandList(0) };
			vrM5DxGpuCommand.GetCommandQueue().ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


			// Now empty the command queue to make sure all the commands have run.
			vrM5DxGpuCommand.EmptyCommandQueueUsingFence();
		}

		M5DxGpuMemoryUploadBuffer::~M5DxGpuMemoryUploadBuffer()
		{
			if (mIsUsingDefaultBuffer == false)
			{
				// "Close the door" because the copy is done.
				mpID3D12Resource->Unmap(0, nullptr);

			}
		}

		void M5DxGpuMemoryUploadBuffer::Update(const void * vpRawDataToCopyToGpu)
		{
			// Copy the data using the C "Memory Copy" (memcpy) function.
			memcpy(mpBufferHead, vpRawDataToCopyToGpu, mBufferSize);
		}

		ID3D12Resource & M5DxGpuMemoryUploadBuffer::GetID3D12Resource(void)
		{
			return *(mpID3D12Resource.Get());
		}

		size_t M5DxGpuMemoryUploadBuffer::GetBufferSize(void)
		{
			return mBufferSize;
		}

	}
}