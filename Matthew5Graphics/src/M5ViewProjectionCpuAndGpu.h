#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5Util.h"
#include "M5ViewProjectionCpu.h"
#include "M5ViewProjectionGpu.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// For learning and organization it helps to name things either "Cpu" or "Gpu" 
		// because they are seperate. A C++ programmer has to create data in system 
		// memory ("Cpu"). Next, the C++ programmer must copy it to the "Gpu". 
		// Naming things "Cpu" and "Gpu" helps a bit conceptually with the process.
		// This class handles copying the data from Cpu to the Gpu.
		//
		// This always supports 2 eyes/cameras. 
		// If in desktop mode then only use one of the two.
		// If in Virtual Reality mode then you will use both.
		class M5ViewProjectionCpuAndGpu
		{
		public:
			M5ViewProjectionCpuAndGpu(ID3D12Device & vrpID3D12Device);

			// There is a Gpu Constant Buffer that holds the view projection 
			// data. This function will return the Gpu address of the constant buffer.
			D3D12_GPU_VIRTUAL_ADDRESS GetGpuConstBufferAddress(ENUMM5Eye vENUMM5Eye);

			M5ViewProjectionCpu &  GetCpuViewProjection(void);

			// Copy the Cpu data to the Gpu.
			void UpdateGpuWithCpuData(ENUMM5Eye vENUMM5Eye);
		private:
			M5ViewProjectionCpu mM5ViewProjectionCpu;
			M5ViewProjectionGpu mM5ViewProjectionGpu;
		};
	}
}