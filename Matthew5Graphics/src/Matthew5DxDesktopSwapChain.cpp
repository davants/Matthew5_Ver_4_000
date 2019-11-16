#include "Matthew5DxDesktopSwapChain.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{

		Matthew5DxDesktopSwapChain::Matthew5DxDesktopSwapChain(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, M5DxGpuCommand & vrM5DxGpuCommand)
			:
			IMatthew5SwapChain(vrM5DxGpuDeviceNFactory, vrM5DxGpuCommand)
		{
			InitializeSwapChain();

			// Descriptor heaps for Render target and depth
			InitializeRenderTargetVDepthSVDescriptorHeaps();

			ResetScreenNSwapChainNDepthNViewport();
		}

		void Matthew5DxDesktopSwapChain::InitializeSwapChain(void)
		{
			m4XMSAAIndicator = true;
			if (InitializeSwapChainTrySample(m4XMSAAIndicator))
			{
				// DTS - 4 MSAA Worked!
#ifdef _DEBUG
				::OutputDebugString(L"4 MSAA Worked\n");
#endif	
				return;
			}
			else
			{
				// DTS - Failed, so revert to 1 sample count.
				m4XMSAAIndicator = false;

				if (!InitializeSwapChainTrySample(m4XMSAAIndicator))
				{
					throw new std::exception("Error - Can't create swap chain.");
				}
				else
				{
					// 1 MSAA Worked.
#ifdef _DEBUG
					::OutputDebugString(L"1 MSAA was used\n");
#endif	
					return;
				}
			}
		}

		bool Matthew5DxDesktopSwapChain::InitializeSwapChainTrySample(bool TryMsaaX4Sample)
		{
			// The client of this function can pass a true for the TryMsaaX4Sample and 
			// it is created then this will return true. If it fails then this 
			// will return false and the client will know there is no support for 4X sample
			// and can try again with TryMsaaX4Sample set to false.

			mpIDXGISwapChain.Reset();

			SetClientWindowSizes();

			HRESULT hr;

			int vSampleCount = 0;
			if (TryMsaaX4Sample)
			{
				vSampleCount = 4;
			}
			else
			{
				vSampleCount = 1;
			}


			DXGI_SWAP_CHAIN_DESC vDXGI_SWAP_CHAIN_DESC;

			// Try Sample Count!!
			vDXGI_SWAP_CHAIN_DESC.SampleDesc.Count = vSampleCount;

			vDXGI_SWAP_CHAIN_DESC.BufferDesc.Width = mScreenAreaWidth;
			vDXGI_SWAP_CHAIN_DESC.BufferDesc.Height = mScreenAreaHeight;
			vDXGI_SWAP_CHAIN_DESC.BufferDesc.RefreshRate.Denominator = 1;
			vDXGI_SWAP_CHAIN_DESC.BufferDesc.RefreshRate.Numerator = 60;
			vDXGI_SWAP_CHAIN_DESC.BufferDesc.Format = mrM5DxGpuDeviceNFactory.GetBackBuffer_DXGI_FORMAT();
			vDXGI_SWAP_CHAIN_DESC.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			vDXGI_SWAP_CHAIN_DESC.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			vDXGI_SWAP_CHAIN_DESC.SampleDesc.Quality = TryMsaaX4Sample ? (mrM5DxGpuDeviceNFactory.Get4XMSAAQuality() - 1) : 0;

			vDXGI_SWAP_CHAIN_DESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			vDXGI_SWAP_CHAIN_DESC.BufferCount = mSwapChainBufferCount;
			vDXGI_SWAP_CHAIN_DESC.OutputWindow = mrM5DxGpuDeviceNFactory.GetHWND();
			vDXGI_SWAP_CHAIN_DESC.Windowed = true;
			vDXGI_SWAP_CHAIN_DESC.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			vDXGI_SWAP_CHAIN_DESC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			hr = mrM5DxGpuDeviceNFactory.GetIDXGIFactory4().CreateSwapChain(
				&(mrM5DxGpuCommand.GetCommandQueue()),
				&vDXGI_SWAP_CHAIN_DESC,
				mpIDXGISwapChain.GetAddressOf());

			return (hr == S_OK);
		}

		void Matthew5DxDesktopSwapChain::SetClientWindowSizes(void)
		{
			// Ask the Windows 10 operating system how big the window is.
			// This is not the monitor size, but the application window.

			// Create a rectangle struct to hold data.
			RECT vRect;

			// Pass the Windows Handle, mHWND. We also pass the address of our RECT so that Windows 10 
			// will set it with the height and width of the user's window.
			if (GetClientRect(mrM5DxGpuDeviceNFactory.GetHWND(), &vRect))
			{
				mScreenAreaWidth = vRect.right - vRect.left;
				mScreenAreaHeight = vRect.bottom - vRect.top;
			}

			// If debug mode, output the window sizes.
#ifdef _DEBUG
			std::wstring vWidth = std::to_wstring(mScreenAreaWidth);
			std::wstring vHeight = std::to_wstring(mScreenAreaHeight);
			std::wstring vOutput = L"Screen Size Info - WidthAndHeight(" + vWidth + L"," + vHeight + L")\n";
			::OutputDebugString(vOutput.c_str());
#endif	

		}

		void Matthew5DxDesktopSwapChain::InitializeRenderTargetVDepthSVDescriptorHeaps()
		{
			D3D12_DESCRIPTOR_HEAP_DESC vD3D12_DESCRIPTOR_HEAP_DESC_RenderTargetView;
			vD3D12_DESCRIPTOR_HEAP_DESC_RenderTargetView.NumDescriptors = 2;
			vD3D12_DESCRIPTOR_HEAP_DESC_RenderTargetView.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			vD3D12_DESCRIPTOR_HEAP_DESC_RenderTargetView.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			vD3D12_DESCRIPTOR_HEAP_DESC_RenderTargetView.NodeMask = 0;

			mpRtvMatthew5DxDescriptorHeapManager = std::make_unique< Matthew5DxDescriptorHeapManager>(
				vD3D12_DESCRIPTOR_HEAP_DESC_RenderTargetView,
				mrM5DxGpuDeviceNFactory.GetID3D12Device()
				);


			D3D12_DESCRIPTOR_HEAP_DESC vD3D12_DESCRIPTOR_HEAP_DESC_DepthStencilView;
			vD3D12_DESCRIPTOR_HEAP_DESC_DepthStencilView.NumDescriptors = 1;
			vD3D12_DESCRIPTOR_HEAP_DESC_DepthStencilView.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			vD3D12_DESCRIPTOR_HEAP_DESC_DepthStencilView.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			vD3D12_DESCRIPTOR_HEAP_DESC_DepthStencilView.NodeMask = 0;


			mpDsvMatthew5DxDescriptorHeapManager = std::make_unique<Matthew5DxDescriptorHeapManager>(
				vD3D12_DESCRIPTOR_HEAP_DESC_DepthStencilView,
				mrM5DxGpuDeviceNFactory.GetID3D12Device());

		}

		void Matthew5DxDesktopSwapChain::ResetScreenNSwapChainNDepthNViewport(void)
		{
			mrM5DxGpuCommand.EmptyCommandQueueUsingFence();

			SetClientWindowSizes();

			mrM5DxGpuCommand.GetCommandList(0).Close();

			ThrowIfFail(mrM5DxGpuCommand.GetCommandList(0).Reset(&(mrM5DxGpuCommand.GetCommandAllocator(0)), nullptr));

			// Reset the quick references we have to the swap chain and the depth stencil.
			for (int i = 0; i < mSwapChainBufferCount; ++i)
			{
				mSwapChainBufferQuickReference[i].Reset();
			}

			mDepthStencilBufferQuickReference.Reset();

			// Resize the swap chain.
			ThrowIfFail(mpIDXGISwapChain->ResizeBuffers(
				mSwapChainBufferCount,
				mScreenAreaWidth, mScreenAreaHeight,
				mrM5DxGpuDeviceNFactory.GetBackBuffer_DXGI_FORMAT(),
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

			mSwapChainCurrentBackBufferIndex = 0;

			for (UINT i = 0; i < mSwapChainBufferCount; i++)
			{
				ThrowIfFail(mpIDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBufferQuickReference[i])));

				D3D12_CPU_DESCRIPTOR_HANDLE vD3D12_CPU_DESCRIPTOR_HANDLE = mpRtvMatthew5DxDescriptorHeapManager->GetNextUsingCounter();

				mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateRenderTargetView(mSwapChainBufferQuickReference[i].Get(), nullptr,
					vD3D12_CPU_DESCRIPTOR_HANDLE);
			}

			// Depth stencil buffer and the view.
			D3D12_RESOURCE_DESC vDepthStencilBufferDesc;
			vDepthStencilBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			vDepthStencilBufferDesc.Alignment = 0;
			vDepthStencilBufferDesc.Width = mScreenAreaWidth;
			vDepthStencilBufferDesc.Height = mScreenAreaHeight;
			vDepthStencilBufferDesc.DepthOrArraySize = 1;
			vDepthStencilBufferDesc.MipLevels = 1;

			vDepthStencilBufferDesc.Format = mrM5DxGpuDeviceNFactory.GetDepthStencil_DXGI_FORMAT();

			vDepthStencilBufferDesc.SampleDesc.Count = m4XMSAAIndicator ? 4 : 1;
			vDepthStencilBufferDesc.SampleDesc.Quality = m4XMSAAIndicator ? (mrM5DxGpuDeviceNFactory.Get4XMSAAQuality() - 1) : 0;
			vDepthStencilBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			vDepthStencilBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE vClearValue;
			vClearValue.Format = mrM5DxGpuDeviceNFactory.GetDepthStencil_DXGI_FORMAT();
			vClearValue.DepthStencil.Depth = 1.0f;
			vClearValue.DepthStencil.Stencil = 0;
			ThrowIfFail(mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&vDepthStencilBufferDesc,
				D3D12_RESOURCE_STATE_COMMON,
				&vClearValue,
				IID_PPV_ARGS(mDepthStencilBufferQuickReference.GetAddressOf())));

			// Create descriptor to mip level 0 of entire resource using the format of the resource.
			D3D12_DEPTH_STENCIL_VIEW_DESC vDepthStencilViewDesc;
			vDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			vDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			vDepthStencilViewDesc.Format = mrM5DxGpuDeviceNFactory.GetDepthStencil_DXGI_FORMAT();
			vDepthStencilViewDesc.Texture2D.MipSlice = 0;
			mrM5DxGpuDeviceNFactory.GetID3D12Device().CreateDepthStencilView(mDepthStencilBufferQuickReference.Get(), &vDepthStencilViewDesc, DepthStencilView());

			// Transition the resource from its initial state to be used as a depth buffer.
			mrM5DxGpuCommand.GetCommandList(0).ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBufferQuickReference.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

			// Execute the resize commands.
			ThrowIfFail(mrM5DxGpuCommand.GetCommandList(0).Close());
			ID3D12CommandList* vCommandLists[] = { &(mrM5DxGpuCommand.GetCommandList(0)) };
			mrM5DxGpuCommand.GetCommandQueue().ExecuteCommandLists(_countof(vCommandLists), vCommandLists);

			// Wait for all previous commands to finish.
			mrM5DxGpuCommand.EmptyCommandQueueUsingFence();

			// Update the viewport transform to cover the client area.
			mD3D12_VIEWPORTScreen[ENUMM5Eye::M5LeftEye].TopLeftX = 0;
			mD3D12_VIEWPORTScreen[ENUMM5Eye::M5LeftEye].TopLeftY = 0;
			mD3D12_VIEWPORTScreen[ENUMM5Eye::M5LeftEye].Width = static_cast<float>(mScreenAreaWidth);
			mD3D12_VIEWPORTScreen[ENUMM5Eye::M5LeftEye].Height = static_cast<float>(mScreenAreaHeight);
			mD3D12_VIEWPORTScreen[ENUMM5Eye::M5LeftEye].MinDepth = 0.0f;
			mD3D12_VIEWPORTScreen[ENUMM5Eye::M5LeftEye].MaxDepth = 1.0f;

			D3D12_RECT vD3D12_RECT = { 0, 0, mScreenAreaWidth, mScreenAreaHeight };
			SetScissor(ENUMM5Eye::M5LeftEye, vD3D12_RECT);
		
		}

		D3D12_CPU_DESCRIPTOR_HANDLE Matthew5DxDesktopSwapChain::DepthStencilView(void) const
		{
			return mpDsvMatthew5DxDescriptorHeapManager->GetCpuHandletAtIndex(0);
		}

		ID3D12Resource * Matthew5DxDesktopSwapChain::CurrentBackBuffer() const
		{
			return mSwapChainBufferQuickReference[mSwapChainCurrentBackBufferIndex].Get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE Matthew5DxDesktopSwapChain::CurrentBackBufferView() const
		{
			return mpRtvMatthew5DxDescriptorHeapManager->GetCpuHandletAtIndex(mSwapChainCurrentBackBufferIndex);
		}


		int Matthew5DxDesktopSwapChain::GetSwapChainBufferCountPerEye(void)
		{
			return mSwapChainBufferCount;
		}

		void Matthew5DxDesktopSwapChain::Present(void)
		{
			// "Present" will switch, flip, the back buffer to be the front buffer which means 
			// the user's computer screen will change to the newly rendered image. The one that was 
			// the front buffer will be the back buffer and is not available to be drawn to.
			ThrowIfFail(mpIDXGISwapChain->Present(0, 0));

			// We use this index, int, value to track which of our 2 buffers is now the back buffer.
			// Since we just called "Present" we need to switch the index.
			// This is needed for our mSwapChainBufferQuickReference[2]; see the function CurrentBackBuffer().
			// This int will only ever have 0 or 1 as its value.
			mSwapChainCurrentBackBufferIndex = (mSwapChainCurrentBackBufferIndex + 1) % mSwapChainBufferCount;

		}

		int Matthew5DxDesktopSwapChain::GetScreenAreaHeight(void)
		{
			return mScreenAreaHeight;
		}

		int Matthew5DxDesktopSwapChain::GetScreenAreaWidth(void)
		{
			return mScreenAreaWidth;
		}

		void Matthew5DxDesktopSwapChain::ThrowIfFail(HRESULT hr)
		{
			if (hr != S_OK)
			{
				throw new std::exception("Error - Matthew5DxDesktopSwapChain");
			}
		}

	}
}