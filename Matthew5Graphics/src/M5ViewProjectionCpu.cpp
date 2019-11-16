#include "M5ViewProjectionCpu.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5ViewProjectionCpu::M5ViewProjectionCpu(void)
		{
			mViewProjection[ENUMM5Eye::M5LeftEye] = M5Util::Identity();
			mViewProjection[ENUMM5Eye::M5RightEye] = M5Util::Identity();
		}

		XMFLOAT4X4 & M5ViewProjectionCpu::GetViewProjection(ENUMM5Eye vENUMM5Eye)
		{
			return mViewProjection[vENUMM5Eye];
		}

	}
}