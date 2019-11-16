#pragma once
#include <string>
#include <memory>
#include "M5Util.h"
#include "M5SceneObject.h"
#include "M5TriFactory.h"
#include "IMatthew5SceneObjectList.h"
#include "M5DxGpuDeviceNFactory.h"
#include "M5DxGpuCommand.h"
#include "M5DxFrameResources.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		class M5DxSceneObjectList : IMatthew5SceneObjectList
		{
		public:
			M5DxSceneObjectList(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory,
				M5DxGpuCommand & vrM5DxGpuCommand,
				M5DxFrameResources& vrM5DxFrameResources);

			virtual std::int32_t AddSceneObject(M5SceneObjectCpuPosition vM5SceneObjectCpuPosition,
				const std::string vrGeometryDefinitionFileName);

			// Update an existing scene object.
			virtual void UpdateSceneObject(std::int32_t vSceneObjectId,
				M5SceneObjectCpuPosition vM5SceneObjectCpuPosition);


		private:
			// Added so each new scene object can have a unique int Id. Note that all 4 frame resources 
			// have the same number for the same scene object.
			// This id is also an index value used to get at the scene objects.
			// Therefore, it starts at -1 so that when it is incremented for the first "Add Scene object"
			// the value will be zero, meaning the first element in the list.
			std::int32_t mSceneObjectIdCounter = -1;

			M5DxGpuDeviceNFactory& mrM5DxGpuDeviceNFactory;
			M5DxGpuCommand& mrM5DxGpuCommand;
			M5DxFrameResources& mrM5DxFrameResources;

			uint64_t mUpdateCounter;
		};
	}
}