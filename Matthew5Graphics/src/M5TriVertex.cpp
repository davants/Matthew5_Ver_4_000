#include "M5TriVertex.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5TriVertex::M5TriVertex(float vPositionX, float vPositionY, float vPositionZ)
			:
			mPositionX(vPositionX),
			mPositionY(vPositionY),
			mPositionZ(vPositionZ)
		{
		}

		M5TriVertex::M5TriVertex(void)
		{
			mPositionX = 0.0f;
			mPositionY = 0.0f;
			mPositionZ = 0.0f;
		}

	}
}