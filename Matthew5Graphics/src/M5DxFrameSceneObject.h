#pragma once

#include <memory>
#include "M5SceneObject.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This is a thin wrapper for the M5SceneObject and adds an int Id. This Id will be 
		// used by the client to communicate.
		// This also includes an R-Value constructor to move this efficiently. It also 
		// includes a copy constructor and assignment (=) operator.
		class M5DxFrameSceneObject
		{
		public:
			// Constructor
			M5DxFrameSceneObject(std::int32_t vId, std::unique_ptr< M5SceneObject> vpM5SceneObject);

			// R-Value Constructor
			M5DxFrameSceneObject(M5DxFrameSceneObject && vrrOther);

			// Copy Constructor - Delete
			// This should not be copied because it is placed in a vector which can grow 
			// and the std::vector will naturally use the R-Value Constructor.
			M5DxFrameSceneObject(const M5DxFrameSceneObject & vrOther) = delete;

			// Assignment Operator
			M5DxFrameSceneObject& operator=(M5DxFrameSceneObject & vrOther) = delete;

			std::int32_t GetId(void);

			M5SceneObject & GetM5SceneObject(void);

		private:
			std::unique_ptr< M5SceneObject> mpM5SceneObject;
			std::int32_t mId;
		};
	}
}