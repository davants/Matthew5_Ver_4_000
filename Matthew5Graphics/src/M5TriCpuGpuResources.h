#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5TriVertex.h"
#include "M5TriGpuResources.h"
#include "M5TriCpuResources.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// A computer has its own Cpu processors and it own memory; also, a computer will contain 
		// a Gpu graphics card that has its own processors and memory. Part of the work we have to 
		// do is give our Cpu data to the Gpu. 
		// For the Matthew5 project we call the Gpu card related classes "Gpu" and the non-Gpu, "Cpu". 
		// For example, if we refer to system memory we will say "Cpu memory".
		// 
		// All the triangle's data must be stored in the Cpu memory and Gpu memory.
		// This class organizes things so we have easy access to both Cpu and Gpu in one class.
		// This class contains both the Cpu and Gpu resources related to triangles.
		class M5TriCpuGpuResources
		{
		public:
			// Cpu Resources - Created in C++
			std::unique_ptr<M5TriCpuResources> mpM5TriCpuResources;

			// Gpu Resources - Copy of triangles on the Gpu.
			std::unique_ptr<M5TriGpuResources> mpM5TriGpuResources;

			
			// Gpu Helper function - Get a View / Descriptor for Vertex data.
			D3D12_VERTEX_BUFFER_VIEW GetGpuVertexBufferView()const;		

			// Gpu Helper function - Get a View / Descriptor for Index data.
			D3D12_INDEX_BUFFER_VIEW GetGpuIndexBufferView()const;

			// To have the Gpu draw a shape, we must have the triangles that make up 
			// the shape on the Gpu. We must first create them in C++
			// code and then load them onto the Gpu so we can use them for a draw call.
			// So, we need the triangle data on both the Cpu System and Gpu.
		};
	}
}