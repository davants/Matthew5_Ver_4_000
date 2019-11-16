#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5TriVertex.h"
#include "M5TriGpuResources.h"
#include "M5TriCpuResources.h"
#include "M5TriCpuGpuResources.h"
#include "M5Util.h"
#include "M5DxGpuCommand.h"
#include "tiny_obj_loader.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{				
		// Computer graphics uses triangles to form shapes. This factory class has static function to 
		// help create the needed triangle data on the Cpu and Gpu. 
		// Read class comments for M5TriCpuGpuResources and M5TriVertex.
		class M5TriFactory
		{
		public:

			// One Triangle - Used for testing
			// This will create the 3 vertex points, plus the 3 indicies, needed for a single triangle.
			// The returned value is a pointer to a M5TriCpuGpuResources object which contains
			// both Cpu and Gpu resources. 
			// See M5TriCpuGpuResources class comments for details.
			static std::unique_ptr< M5TriCpuGpuResources> GetTriangleSetResourcesForSimpleTriangle(
				ID3D12Device & vrID3D12Device);


			// This function will load the given obj file geometry onto the Gpu Default buffer.
			// This function will empty the command queue of the given M5DxGpuCommand.
			// The vrObjFilePathDirectory can also contain the .mtl file.
			static std::unique_ptr< M5TriCpuGpuResources> GetTriangleSetResourcesForObjFile(
				ID3D12Device& vrID3D12Device,
				M5DxGpuCommand& vrM5DxGpuCommand,
				std::string & vrObjFilePathDirectory,
				const std::string& vrObjFileName);


			private:

				// This function will take the Cpu memory triangles and create the cooresponding needed Gpu 
				// memory resources.
				static std::unique_ptr<M5TriGpuResources> GetM5TriGpuResources(
					ID3D12Device & vrID3D12Device,
					M5TriCpuResources & vrM5TriCpuResources);
				
				static std::unique_ptr<M5TriGpuResources> GetM5TriGpuDefaultBufferResources(
					ID3D12Device& vrID3D12Device,
					M5TriCpuResources& vrM5TriCpuResources,
					M5DxGpuCommand& vrM5DxGpuCommand);

				// See this function's definition for details on triangles and local space.
				static std::unique_ptr<M5TriCpuResources> GetCpuSimpleTriangle(void);
				static std::unique_ptr<M5TriCpuResources> GetCpuSimpleTriangle2(void);
		};		

	}
}