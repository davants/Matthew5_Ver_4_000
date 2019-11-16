#pragma once
#include "M5DxGpuDeviceNFactory.h"
#include "M5DxGpuCommand.h"
#include "M5Util.h"
#include "Matthew5Enums.h"



namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// The swap chain is all about managing the front and back buffers and 
		// then "Presenting" one of them to the screen while making the other available 
		// to draw/render to.
		class IMatthew5SwapChain
		{
		public:

			IMatthew5SwapChain(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory,	M5DxGpuCommand & vrM5DxGpuCommand);
			virtual ~IMatthew5SwapChain();

			virtual void CommandListSetViewport(ID3D12GraphicsCommandList & vrCmdList, ENUMM5Eye vENUMM5Eye);
			virtual void CommandListSetRECTScissor(ID3D12GraphicsCommandList & vrCmdList, ENUMM5Eye vENUMM5Eye) ;

			// Does the system support 4X multi-sampling?
			virtual bool Get4XMSAAIndicator(void);

			virtual int GetMSAACount(void);

			// For derived classes that only use one screen, like Desktop, ENUMM5Eye will always be  
			// left. For Oculus, both will need to be set with 2 calls.
			virtual void SetViewPort(ENUMM5Eye vENUMM5Eye, const D3D12_VIEWPORT & vD3D12_VIEWPORT);

			virtual void SetScissor(ENUMM5Eye vENUMM5Eye, const D3D12_RECT & vD3D12_RECT);

			virtual int GetSwapChainBufferCountPerEye(void) = 0;


		protected:

			// View Port
			//	The view port is set as one of the commands on the command list for each draw.
			//		mpCommandList->RSSetViewports(..);
			//	It contains the size of the screen being drawn and the range for the depth.
			//		For example, the depth of the farthest object possible can be set to 1.0 and the
			//		closest depth can be set to 0.
			//	Example of setting up a View Port's Properties:
			//		// Update the viewport transform to cover the client area.
			//		mD3D12_VIEWPORTScreen.TopLeftX = 0;
			//		mD3D12_VIEWPORTScreen.TopLeftY = 0;
			//		mD3D12_VIEWPORTScreen.Width = static_cast<float>(mScreenAreaWidth);
			//		mD3D12_VIEWPORTScreen.Height = static_cast<float>(mScreenAreaHeight);
			//		mD3D12_VIEWPORTScreen.MinDepth = 0.0f;
			//		mD3D12_VIEWPORTScreen.MaxDepth = 1.0f;
			//	Example of using the viewport durring a draw call. (Set 1 viewport, here is the address)
			//		mpCommandList->RSSetViewports(1, &mD3D12_VIEWPORTScreen);
			// 2 viewports in-case the graphics are for VR which has two screens.
			D3D12_VIEWPORT mD3D12_VIEWPORTScreen[2];

			D3D12_RECT mD3D12_RECTScissor[2];

			M5DxGpuDeviceNFactory & mrM5DxGpuDeviceNFactory;
			M5DxGpuCommand & mrM5DxGpuCommand;

			// Does the system support 4X multi-sampling?
			bool m4XMSAAIndicator = false;
		};

	}
}