#include "M5DxSceneObjectList.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		M5DxSceneObjectList::M5DxSceneObjectList(M5DxGpuDeviceNFactory& vrM5DxGpuDeviceNFactory,
			M5DxGpuCommand& vrM5DxGpuCommand,
			M5DxFrameResources& vrM5DxFrameResources) :
			mrM5DxGpuDeviceNFactory(vrM5DxGpuDeviceNFactory),
			mrM5DxGpuCommand(vrM5DxGpuCommand),
			mrM5DxFrameResources(vrM5DxFrameResources),
			mUpdateCounter(0)
		{

		}
		std::int32_t M5DxSceneObjectList::AddSceneObject(M5SceneObjectCpuPosition vM5SceneObjectCpuPosition,
			const std::string vrGeometryDefinitionFileName)
		{
			// Create the next scene object id by incrementing mSceneObjectIdCounter.
			// Note that all 4 frame resources will have the same id for this scene object.
			mSceneObjectIdCounter++;

			mUpdateCounter++;

			// Directory to search for. 
			std::string vObjFileDirectory = "Matthew5ObjFiles";
			// Create relative path to the file.
			std::string vRelFilePath = vObjFileDirectory + "\\" + vrGeometryDefinitionFileName;
			// Use search util function to get the full path to the file.
			std::string vRelFilePathSearch = M5Util::FindFullPath(vRelFilePath);
			// Validate it was found.
			if (vRelFilePathSearch.length() == 0)
			{
				// Error - file not found.
				return -1;
			}
			// From the full file path, get just the full directory path.
			size_t vStartPosition = vRelFilePathSearch.length() - vrGeometryDefinitionFileName.length();
			std::string vFullDirectoryPath = vRelFilePathSearch.replace(
				vStartPosition, vrGeometryDefinitionFileName.length(), "");

			// Repeat the "create scene object" 4 times because there are 4 frame resources.
			for (int i = 0; i < 4; i++)
			{
				// Create a scene object.
				std::unique_ptr<M5SceneObject> vpSimpleM5SceneObjectTriangle =
					std::make_unique<M5SceneObject>(mrM5DxGpuDeviceNFactory.GetID3D12Device());

				vpSimpleM5SceneObjectTriangle->SetM5TriCpuGpuResources(
					M5TriFactory::GetTriangleSetResourcesForObjFile(
						mrM5DxGpuDeviceNFactory.GetID3D12Device(), mrM5DxGpuCommand,
						vFullDirectoryPath, vrGeometryDefinitionFileName));

				vpSimpleM5SceneObjectTriangle->UpdatePosition(vM5SceneObjectCpuPosition,
					mUpdateCounter);

				// There are 4 frame resources. We use an int index to get the one we want.
				// Get the frame resource index and increment it.
				// The new value of the index after incrementing it can only be 0, 1, 2 or 3.
				// If it is now zero, set it to one. If one, set it to two. If two, set to three. 
				// If three, set to zero.
				int vFrameResourceIndex = mrM5DxFrameResources.GetNextIndex(
					mrM5DxFrameResources.GetAvailableFrameResourceIndex());

				mrM5DxFrameResources.SetAvailableFrameResourceIndex(vFrameResourceIndex);

				// Get a reference to the current frame resource.
				M5DxFrameResource& vrM5DxFrameResource = mrM5DxFrameResources.GetAvailableResource();

				// Add the scene object.
				vrM5DxFrameResource.GetSceneObjectList().AddSceneObject(mSceneObjectIdCounter, std::move(vpSimpleM5SceneObjectTriangle));
			}

			return mSceneObjectIdCounter;
		}

		void M5DxSceneObjectList::UpdateSceneObject(std::int32_t vSceneObjectId, 
			M5SceneObjectCpuPosition vM5SceneObjectCpuPosition)
		{
			// Increment the update counter so each frame resource scene object will always 
			// have a higher value. This is used during the draw process to make sure 
			// each frame resource has the latest data.
			mUpdateCounter++;

			// Using the "Current Frame Resource", get the scene object.
			M5DxFrameSceneObject& vrFrameSceneObject = 
				mrM5DxFrameResources.GetAvailableResource().GetSceneObjectList().GetM5DxFrameSceneObject(vSceneObjectId);

			// Update the scene object giving it the newly updated counter.
			vrFrameSceneObject.GetM5SceneObject().UpdatePosition(vM5SceneObjectCpuPosition,
				mUpdateCounter);
		}


	}
}