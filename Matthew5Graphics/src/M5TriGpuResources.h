#pragma once


#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5TriVertex.h"
#include "M5DxGpuMemoryUploadBuffer.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		struct M5TriGpuResources
		{
			std::unique_ptr<M5DxGpuMemoryUploadBuffer> mpVertexArray;

			std::unique_ptr<M5DxGpuMemoryUploadBuffer> mpIndexArray;			

			// C++ notes: int32_t means it is guaranteed to be a 32 bit integer.
			// This data type was added for the C99 (year 1999) standard update release.
			// The reason for the update is that "int" can be 16, 32, etc.. up to the compiler.
		};
	}
}