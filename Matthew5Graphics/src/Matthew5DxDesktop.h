#pragma once

#include "Matthew5Dx.h"
#include "IMatthew5DrawInstructions.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This implements all needed Desktop specific code to support the IMatthew5Graphics.
		class Matthew5DxDesktop : public Matthew5Dx
		{
		public:
			Matthew5DxDesktop(HWND vHWND);
			virtual ~Matthew5DxDesktop();

			virtual void Draw(IMatthew5DrawInstructions & vrIMatthew5DrawInstructions);


		private:
			void CreateProjection(void);

			// This will update the GPU data that only changes once per frame draw.
			// For example, the camera position is the same for all objects drawn.
			void Update(IMatthew5DrawInstructions & vrIMatthew5DrawInstructions);
			void DrawScene(void);
			void DrawSceneObjects(ID3D12GraphicsCommandList* vpCommandList);

			XMFLOAT4X4 mProjection = M5Util::Identity();
			XMFLOAT4X4 mView = M5Util::Identity();
		};
	}
}