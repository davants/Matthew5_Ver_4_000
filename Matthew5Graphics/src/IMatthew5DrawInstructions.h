#pragma once

#include "M5TriVertex.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		class IMatthew5DrawInstructions
		{
		public: 
			IMatthew5DrawInstructions();

			M5TriVertex & GetCameraPosition(void);
			M5TriVertex & GetCameraLookAt(void);
			M5TriVertex & GetCameraUpDirection(void);

			float mYaw;
			float m_fPitch;

		private:
			M5TriVertex mCameraPos;
			M5TriVertex mCameraLookAt;
			M5TriVertex mCameraUpDirection;

		};

	}
}

