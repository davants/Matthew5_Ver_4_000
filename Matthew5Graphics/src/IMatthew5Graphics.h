#pragma once

#include "IMatthew5DrawInstructions.h"
#include "IMatthew5SceneObjectList.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This is a simple set of components to support learning C++ and computer graphics.
		// The overall design is to keep graphics abstracted from the application, physics, networking etc.
		class IMatthew5Graphics
		{
		public:

			virtual ~IMatthew5Graphics() {}

			// Draw all the scene objects that have been added. 
			virtual void Draw(IMatthew5DrawInstructions & vrIMatthew5DrawInstructions) = 0;

			virtual IMatthew5SceneObjectList& GetSceneObjectList(void) = 0;
			

		};
	}
}