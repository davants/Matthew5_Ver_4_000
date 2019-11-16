#pragma once

#include "M5DxGpuDeviceNFactory.h"
#include "M5DxGpuCommand.h"
#include "M5Util.h"
#include "Matthew5DxDescriptorHeapManager.h"
#include "IMatthew5SwapChain.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This imlements the needed functionality of the swap chair for the Desktop.

		// Notes on Swap Chain and Frame Resources
		// Reminder, the swap chain is for display to the screen without tearing, where as the 
		//	frame resources list is about the Gpu command queue and keeping it busy by adding 
		//	multiple "draw" command lists to it. They are not trying to solve the same problem. 
		//	The only time they get together and sync up is when we are about to draw to a 
		//	render target by adding a command list to the command queue and then again when a 
		//  previously added draw command list has finished where the swap chain will present it.	
		//  Step 1) Get the next available render target from the swap chain.
		//  Step 2) Get the next available frame resource from the frame resource list.
		//  Step 3) Give both of these to the Gpu for drawing via a command list. The work 
		//			is placed in the Gpu command queue and will be done in the future.
		//	Step 4) Swap Chain Presents the finished frame when one of the previously added
		//			"draw" command lists has finished.

		class Matthew5DxDesktopSwapChain : public IMatthew5SwapChain
		{
		public:
			Matthew5DxDesktopSwapChain(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, M5DxGpuCommand & vrM5DxGpuCommand);

			virtual int GetSwapChainBufferCountPerEye(void);

			ID3D12Resource* CurrentBackBuffer()const;
			D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
			D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView(void) const;

			void Present(void);

			int GetScreenAreaHeight(void);
			int GetScreenAreaWidth(void);

		private:
			void InitializeSwapChain(void);

			// This function is designed to be "tried". It does not have to succeed.
			// The client will check the returned value to see if it worked.
			// See definition for more details.
			bool InitializeSwapChainTrySample(bool TryMsaaX4Sample);
			void SetClientWindowSizes(void);
			void InitializeRenderTargetVDepthSVDescriptorHeaps();
			void ResetScreenNSwapChainNDepthNViewport(void);

			Microsoft::WRL::ComPtr<IDXGISwapChain> mpIDXGISwapChain;
			const static int mSwapChainBufferCount = 2;
			int mSwapChainCurrentBackBufferIndex = 0;

			int mScreenAreaHeight = 0;
			int mScreenAreaWidth = 0;

			// When we created the swap chain, IDXGISwapChain, it created 2 buffers; front and back.
			// Here we are simply creating a ID3D12Resource that points to those buffers.
			// This will make it easy to work with the buffers, so we don't have to 
			// constantly ask the swap chair for them over and over.
			Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBufferQuickReference[2];
			Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBufferQuickReference;

			// When drawing we have to give the command list descriptors/views for 
			// the render target and depth stencil. These heaps will hold those descriptors/views.

			// Render Target Descriptor Heap
			std::unique_ptr< Matthew5DxDescriptorHeapManager> mpRtvMatthew5DxDescriptorHeapManager;

			// Depth Stencil Descriptor Heap
			std::unique_ptr< Matthew5DxDescriptorHeapManager> mpDsvMatthew5DxDescriptorHeapManager;


			void ThrowIfFail(HRESULT hr);
		};

	}
}