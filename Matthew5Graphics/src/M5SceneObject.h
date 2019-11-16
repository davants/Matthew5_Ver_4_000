#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "M5TriCpuGpuResources.h"
#include "M5SceneObjectCpuAndGpuPosition.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This container class is for one thing in a scene. 
		// For example, a "thing in a scene" could be a chair, street, car, player, etc.
		// This contains the following:
		//		Geometry triangle data in Cpu and Gpu memory is contained in this class.
		//		The location of the "thing in a scene" is contained in this class.
		class M5SceneObject
		{
		public:
			M5SceneObject(ID3D12Device & vrpID3D12Device);

			// There is a Gpu Constant Buffer that holds the world space  
			// data for this object. This function will return the Gpu address of the constant buffer.
			D3D12_GPU_VIRTUAL_ADDRESS GetGpuConstBufferAddressThatHasWorldSpaceNMoreData(void);

			M5TriCpuGpuResources & GetM5TriCpuGpuResources(void);
			void SetM5TriCpuGpuResources(std::unique_ptr<M5TriCpuGpuResources> vpM5TriCpuGpuResources);

			void UpdatePosition(M5SceneObjectCpuPosition vM5SceneObjectCpuPosition,
				uint64_t vUpdateCounter);

			M5SceneObjectCpuAndGpuPosition & GetM5SceneObjectCpuAndGpuPosition(void);

			const uint64_t & GetUpdateCounter(void);

		private:
			// This is the geometry triangles needed to render this "thing".
			std::unique_ptr<M5TriCpuGpuResources> mpM5TriCpuGpuResources;

			M5SceneObjectCpuAndGpuPosition mM5SceneObjectCpuAndGpuPosition;

			// This is set each time something is updated on the Gpu. This is used 
			// to keep track of udpates so that one frame resourse, which has 
			// scene objects, can check if it has the latest data.
			uint64_t mUpdateCounter;
			
		};
	}
}