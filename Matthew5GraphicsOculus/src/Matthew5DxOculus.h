#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "Matthew5GraphicsOculusStandardHeaders.h"

#include "IMatthew5Graphics.h"
#include "M5DxGpuDeviceNFactory.h"
#include "M5DxGpuCommand.h"
#include "Matthew5Dx.h"
#include "Matthew5DxOculusSwapChain.h"

#include "OculusEyeTexture.h"

using namespace Matthew5::Matthew5Graphics;

namespace Matthew5
{
	namespace Matthew5GraphicsOculus
	{
		// This implements all needed Oculus specific code to support the IMatthew5Graphics.
		class Matthew5DxOculus : public Matthew5Dx
		{
		public:			 
			Matthew5DxOculus(HWND vHWND);
			~Matthew5DxOculus();

			bool IsOculusAvailable(void);

			virtual void Draw( IMatthew5DrawInstructions & vrIMatthew5DrawInstructions);


		private:

			bool SetupOculusComponents(void);
			void InitCommandList(int vIndex);

			bool mIsOculusAvailable = false;
			long long mFrameIndex;
			int mEyeMsaaRate = 4;
			ovrSession mOvrSession;
			ovrGraphicsLuid mOvrGraphicsLuid;
			ovrHmdDesc mOvrHmdDesc;
			ovrRecti mEyeRenderViewport[2];
			ovrEyeRenderDesc mOvrEyeRenderDesc[2];		
		};

	}
}
