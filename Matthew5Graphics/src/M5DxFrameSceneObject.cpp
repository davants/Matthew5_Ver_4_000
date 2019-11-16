#include "M5DxFrameSceneObject.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		
		M5DxFrameSceneObject::M5DxFrameSceneObject(std::int32_t vId, std::unique_ptr<M5SceneObject> vpM5SceneObject)
			: mpM5SceneObject(std::move(vpM5SceneObject)),
			mId(vId)
		{
		}



		M5DxFrameSceneObject::M5DxFrameSceneObject(M5DxFrameSceneObject && vrrOther)
		{
			mpM5SceneObject = std::move(vrrOther.mpM5SceneObject);
		}

		std::int32_t M5DxFrameSceneObject::GetId(void)
		{
			return mId;
		}

		M5SceneObject & M5DxFrameSceneObject::GetM5SceneObject(void)
		{
			return *mpM5SceneObject;
		}

		

	}
}