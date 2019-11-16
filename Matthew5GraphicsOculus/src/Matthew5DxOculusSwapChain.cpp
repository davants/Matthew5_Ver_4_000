#include "Matthew5DxOculusSwapChain.h"
namespace Matthew5
{
	namespace Matthew5GraphicsOculus
	{
		Matthew5DxOculusSwapChain::Matthew5DxOculusSwapChain(
			M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, 
			M5DxGpuCommand & vrM5DxGpuCommand,
			ovrSession & vrovrSession, ovrHmdDesc & vrovrHmdDesc)
			:
			IMatthew5SwapChain(vrM5DxGpuDeviceNFactory, vrM5DxGpuCommand),
			mrovrSession(vrovrSession),
			mrovrHmdDesc(vrovrHmdDesc)
		{
			// Oculus does support 4x 
			m4XMSAAIndicator = true;

			int vNumberOfOculusSwapChainRenderTargets = 3;
			int vNumberOfOculsEyes = 2;

			{
				// Create/Allocate Views/Descriptors for Render Targets
				// The render targets are created by the Oculus Api using the Oculus swap chain.
				// The default Oculus swap chain length is 3, not 2 like the desktop. 
				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
				rtvHeapDesc.NumDescriptors = vNumberOfOculusSwapChainRenderTargets * vNumberOfOculsEyes;
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

				mpRtvDescriptorHeapManager = std::make_unique<Matthew5DxDescriptorHeapManager>(
					rtvHeapDesc,
					vrM5DxGpuDeviceNFactory.GetID3D12Device());
			}


			{
				// Create/Allocate Views/Descriptors for Depth Stencil
				D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
				dsvHeapDesc.NumDescriptors = vNumberOfOculusSwapChainRenderTargets * vNumberOfOculsEyes;
				dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

				mpDsvDescriptorHeapManager = std::make_unique<Matthew5DxDescriptorHeapManager>(
					dsvHeapDesc, vrM5DxGpuDeviceNFactory.GetID3D12Device());
			}

			int vEyeMsaaRate = 4;

			for (int iEye = 0; iEye < 2; iEye++)
			{
				mpOculusEyeRenderTextureList[iEye] = new OculusEyeTexture();

				mIdealSizeList[iEye] = ovr_GetFovTextureSize(mrovrSession, (ovrEyeType)iEye, mrovrHmdDesc.DefaultEyeFov[iEye], 1.0f);

				if (!mpOculusEyeRenderTextureList[iEye]->Init(mrovrSession, mIdealSizeList[iEye].w,
					mIdealSizeList[iEye].h, true, vEyeMsaaRate,
					vrM5DxGpuDeviceNFactory.GetDepthStencil_DXGI_FORMAT()
					, vrM5DxGpuDeviceNFactory.GetID3D12Device(), vrM5DxGpuCommand.GetCommandQueue(),
				*mpRtvDescriptorHeapManager, *mpDsvDescriptorHeapManager))
				{
					throw new std::exception("error in Oculus swap chain");
					return;
				}

				if (!mpOculusEyeRenderTextureList[iEye]->mOvrTextureChain)
				{
					throw new std::exception("error in Oculus swap chain");

					return;
				}

				// Update the base class view port object.
				D3D12_VIEWPORT D3Dvp;
				D3Dvp.Width = (float)mIdealSizeList[iEye].w;
				D3Dvp.Height = (float)mIdealSizeList[iEye].h;
				D3Dvp.MinDepth = 0;
				D3Dvp.MaxDepth = 1;
				D3Dvp.TopLeftX = (float)0;
				D3Dvp.TopLeftY = (float)0;

				D3D12_RECT scissorRect;
				scissorRect.left = static_cast<LONG>((float)0);
				scissorRect.right = static_cast<LONG>((float)0 + (float)mIdealSizeList[iEye].w);
				scissorRect.top = static_cast<LONG>((float)0);
				scissorRect.bottom = static_cast<LONG>((float)0 + (float)mIdealSizeList[iEye].h);

				SetViewPort((ENUMM5Eye)iEye, D3Dvp);
				SetScissor((ENUMM5Eye)iEye, scissorRect);
			}
		}
		Matthew5DxOculusSwapChain::~Matthew5DxOculusSwapChain()
		{
			for (int eye = 0; eye < 2; ++eye)
			{
				delete mpOculusEyeRenderTextureList[eye];
			}					   
		}
		

		int Matthew5DxOculusSwapChain::GetSwapChainBufferCountPerEye(void)
		{
			return mpOculusEyeRenderTextureList[0]->mPerEyeSwapChainBufferCount;
		}
		ovrSizei & Matthew5DxOculusSwapChain::GetIdealSize(ENUMM5Eye vENUMM5Eye)
		{
			return mIdealSizeList[vENUMM5Eye];
		}
		void Matthew5DxOculusSwapChain::ThrowIfFail(HRESULT vHr)
		{
			if (vHr != S_OK)
			{
				throw new std::exception("Error - Matthew5Dx");
			}
		}
	}
}