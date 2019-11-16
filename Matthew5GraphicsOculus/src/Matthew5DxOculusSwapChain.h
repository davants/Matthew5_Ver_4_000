#pragma once

#include "OculusEyeTexture.h"

#include "IMatthew5SwapChain.h"
#include "Matthew5DxDescriptorHeapManager.h"

using namespace Matthew5::Matthew5Graphics;

namespace Matthew5
{
	namespace Matthew5GraphicsOculus
	{

		// Notes on Swap Chain and Frame Resources
		// Reminder, the swap chain is for display to the screen without tearing, where as the 
		//	frame resources list is about the Gpu command queue and keeping it busy by adding 
		//	multiple "draw" command lists to it. They are not trying to solve the same problem. 
		//	The only time they get together and sync up is when we are about to draw to a 
		//	render target by adding a command list to the command queue and then again when a 
		// previously added draw command list has finished where the swap chain will present it.	
		//  Step 1) Get the next available render target from the swap chain.
		//  Step 2) Get the next available frame resource from the frame resource list.
		//  Step 3) Give both of these to the Gpu for drawing via a command list. The work 
		//			is placed in the Gpu command queue and will be done in the future.
		//	Step 4) Swap Chain Presents the finished frame when one of the previously added
		//			"draw" command lists has finished.
		//
		// Oculus Swap Chain:
		//	For Oculus HMD the OculusEyeTexture object keeps the swap chain for one eye; therefore, 
		//	there are 2 OculusEyeTexture. The OculusEyeTexture contains a ovrTextureSwapChain that 
		//	always has 3 render textures for the swap chain. It also has the same for depth stencil
		//	as well as other functionality like a function to get a view descriptor for the current 
		//	render target texture in the swap chain.

		class Matthew5DxOculusSwapChain : public IMatthew5SwapChain
		{
		public:
			Matthew5DxOculusSwapChain(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, M5DxGpuCommand & vrM5DxGpuCommand,
				ovrSession & vrovrSession, ovrHmdDesc & vrovrHmdDesc);

			~Matthew5DxOculusSwapChain();

			virtual int GetSwapChainBufferCountPerEye(void);

			ovrSizei & GetIdealSize(ENUMM5Eye vENUMM5Eye);

			// Each of these 2 objects will keep its own swap chain.
			OculusEyeTexture*           mpOculusEyeRenderTextureList[2] = { nullptr, nullptr };

		private:
			void ThrowIfFail(HRESULT vHr);
			ovrSizei mIdealSizeList[2];

			ovrSession & mrovrSession;
			ovrHmdDesc & mrovrHmdDesc;

			std::unique_ptr<Matthew5DxDescriptorHeapManager> mpRtvDescriptorHeapManager;
			std::unique_ptr<Matthew5DxDescriptorHeapManager> mpDsvDescriptorHeapManager;

		};
	}
}