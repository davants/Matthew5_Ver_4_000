#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5Util.h"
#include "Matthew5Enums.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// Remember, data is kept on both the Cpu and Gpu; this represents the Cpu data. 
		// Eventually this data will be copied to the Gpu by some other class.
		// 
		// This always supports 2 eyes/cameras. 
		// If in desktop mode then only use one of the two.
		// If in Virtual Reality mode then you will use both.
		class M5ViewProjectionCpu
		{
		public:
			M5ViewProjectionCpu(void);

			XMFLOAT4X4 & GetViewProjection(ENUMM5Eye vENUMM5Eye);

		private:
		
			XMFLOAT4X4 mViewProjection[2];
		};
	}
}