#pragma once
#include "Matthew5StandardMicrosoftHeaders.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		struct M5SceneObjectCpuPosition
		{

			//XMFLOAT4X4 matrix;

			// 11 means row one, column one.
			// 12 means row one, column two.
			// 21 means row two, column one.
			// etc....

			float m11, m12, m13, m14;
			float m21, m22, m23, m24;
			float m31, m32, m33, m34;
			float m41, m42, m43, m44;


			//float mPositionX = 0.0f;
			//float mPositionY = 0.0f;
			//float mPositionZ = 0.0f;

			//float mRotationX = 0.0f;
			//float mRotationY = 0.0f;
			//float mRotationZ = 0.0f;
			//float mRotationW = 0.0f;


		};
	}
}