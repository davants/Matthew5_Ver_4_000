#pragma once

#include <vector>
#include "M5DxFrameSceneObject.h"
#include "M5SceneObject.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This holds a list of M5DxFrameSceneObject items.
		// Each frame resource will contain one of these list objects. That means a total 
		// of 4 of these list objects will be instantiated.
		class M5DxFrameSceneObjectList
		{
		public:
			M5DxFrameSceneObjectList();

			// Add a scene object to the list.
			bool AddSceneObject(int32_t vId, std::unique_ptr<M5SceneObject> vpM5SceneObject);

			// Get the number of scene objects in the list.
			size_t GetSize(void);

			// Get a reference to one of the scene objects in the list. Passing an invalid 
			// index parameter will result in an exception being thrown.
			M5DxFrameSceneObject & GetM5DxFrameSceneObject(size_t vIndex);

		private:
			std::vector<M5DxFrameSceneObject> mM5DxFrameSceneObjectDb;

		};
	}
}