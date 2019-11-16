#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5TriVertex.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		// This will hold the triangles that make up a shape in system memory which we will
		// call Cpu resources.
		struct M5TriCpuResources
		{
			std::unique_ptr<M5TriVertex> mpVertexArray;
			int32_t mNumberOfVerticies;

			std::unique_ptr<int16_t> mpIndexArray;
			int32_t mNumberOfIndicies;

			size_t GetVerticiesTotalSizeInBytes(void)
			{
				return mNumberOfVerticies * sizeof(M5TriVertex);
			}

			size_t GetIndiciesTotalSizeInBytes(void)
			{
				return mNumberOfIndicies * sizeof(int16_t);
			}

			// C++ notes: int32_t means it is guaranteed to be a 32 bit integer.
			// int16_t means it is guaranteed to be a 16 bit integer.
			// Thesse data types were added for the C99 (year 1999) standard update release.
			// The reason for the update is that "int" can be 16, 32, etc.. up to the compiler.

			// Notes: The above type "std::unique_ptr<Vertex>" could have been 
			//		"std::unique_ptr<Vertex[]>". However, I'm wanting to keep the idea in our 
			//		heads that an array on the heap is kept in a variable as a pointer; a 
			//		pointer to the first element in the array.
		};
	}

}
