#pragma once

#include <cstdint>
#include <string>

#include "M5SceneObjectCpuPosition.h"


namespace Matthew5
{
	namespace Matthew5Graphics
	{
		class IMatthew5SceneObjectList
		{
		public:
			IMatthew5SceneObjectList();

			// Add another scene object at the given location.
			virtual std::int32_t AddSceneObject(M5SceneObjectCpuPosition vM5SceneObjectCpuPosition,
				const std::string vrGeometryDefinitionFileName) = 0;

			// Update an existing scene object.
			virtual void UpdateSceneObject(std::int32_t vSceneObjectId,
				M5SceneObjectCpuPosition vM5SceneObjectCpuPosition) = 0;

		};

	}
}