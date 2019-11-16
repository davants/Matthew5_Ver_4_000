#include "M5DxFrameSceneObjectList.h"
namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5DxFrameSceneObjectList::M5DxFrameSceneObjectList()
		{
		}

		bool M5DxFrameSceneObjectList::AddSceneObject(int32_t vId, std::unique_ptr<M5SceneObject> vpM5SceneObject)
		{
			// First check to make sure vId does not already exist; if it does then do nothing and return false;
			for (auto item = mM5DxFrameSceneObjectDb.begin(); item != mM5DxFrameSceneObjectDb.end(); item++)
			{
				if (item->GetId() == vId)
				{
					// This already exists so it can not be added again.
					throw new std::exception("Error - already added");
					return false;
				}
			}
			

			M5DxFrameSceneObject vM5DxFrameSceneObject(vId, std::move(vpM5SceneObject));

			// The mM5DxFrameSceneObjectDb list does not hold pointers, it holds objects.
			// Calling "std::move(..)" will tell the compiler the parameter to push_back(..) an "r-value".
			// Since it is an r-value the compiler will call the r-value constructor you created for the 
			// M5DxFrameSceneObject.
			mM5DxFrameSceneObjectDb.push_back(std::move(vM5DxFrameSceneObject));

			return true;
		}

		size_t M5DxFrameSceneObjectList::GetSize(void)
		{
			return mM5DxFrameSceneObjectDb.size();
		}

		M5DxFrameSceneObject & M5DxFrameSceneObjectList::GetM5DxFrameSceneObject(size_t vIndex)
		{
			if (vIndex >= mM5DxFrameSceneObjectDb.size())
			{
				throw new std::exception("ERROR in M5DxFrameSceneObjectList::GetM5DxFrameSceneObject");
			}

			return mM5DxFrameSceneObjectDb[vIndex];
		}

	}
}