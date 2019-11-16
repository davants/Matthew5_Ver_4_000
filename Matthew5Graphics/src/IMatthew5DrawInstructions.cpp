#include "IMatthew5DrawInstructions.h"
namespace Matthew5
{
	namespace Matthew5Graphics
	{
		IMatthew5DrawInstructions::IMatthew5DrawInstructions()
		{
			mCameraPos.mPositionZ = 4.0f;

			mCameraUpDirection.mPositionY = 1.0f;

			// Other Camera Defaults will be 0.0f;

			mYaw = 0.01f;
			m_fPitch = 0.01f;
		}

		M5TriVertex & IMatthew5DrawInstructions::GetCameraPosition(void)
		{
			return mCameraPos;
		}

		M5TriVertex & IMatthew5DrawInstructions::GetCameraLookAt(void)
		{
			return mCameraLookAt;
		}

		M5TriVertex & IMatthew5DrawInstructions::GetCameraUpDirection(void)
		{
			return mCameraUpDirection;
		}

	

	}
}