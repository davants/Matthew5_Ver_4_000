#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5Util.h"
#include "M5SceneObjectGpuPosition.h"
#include "M5SceneObjectCpuPosition.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// For learning and organization it helps to name things either "Cpu" or "Gpu" 
		// because they are seperate. A C++ programmer has to create data in system 
		// memory ("Cpu"). Next, the C++ programmer must copy it to the "Gpu". 
		// Naming things "Cpu" and "Gpu" helps a bit conceptually with the process.
		class M5SceneObjectCpuAndGpuPosition
		{
		public:
			M5SceneObjectCpuAndGpuPosition(ID3D12Device & vrpID3D12Device);


			// Cpu data
			M5SceneObjectCpuPosition mM5SceneObjectCpuPosition;

			// Gpu data
			M5SceneObjectGpuPosition mM5SceneObjectGpuPosition;

		};

	}
}