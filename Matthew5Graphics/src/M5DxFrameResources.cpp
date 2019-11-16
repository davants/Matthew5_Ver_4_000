#include "M5DxFrameResources.h"
// Added for version 2.000

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		M5DxFrameResources::M5DxFrameResources(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, ID3D12CommandQueue & vrID3D12CommandQueue)
			: mrID3D12CommandQueue( vrID3D12CommandQueue)
		{

			// Instanciate the 4 M5DxFrameResource objects.
			for (int i = 0; i < 4; i++)
			{
				mM5DxFrameResourceList[i] = std::make_unique<M5DxFrameResource>(vrM5DxGpuDeviceNFactory, mrID3D12CommandQueue);
				
				mM5DxFrameResourceList[i]->SetTheFence();
				mM5DxFrameResourceList[i]->WaitForFence();
			}

			// We will start with the first one in the list.
			mAvailableFrameResourceIndex = 0;
		}

		M5DxFrameResource & M5DxFrameResources::GetAvailableResource(void)
		{
			// Dereference one of the pointers in the array and return it.
			return *(mM5DxFrameResourceList[mAvailableFrameResourceIndex]);
		}

		int M5DxFrameResources::GetAvailableFrameResourceIndex(void)
		{
			return mAvailableFrameResourceIndex;
		}

		void M5DxFrameResources::SetAvailableFrameResourceIndex(int vIndex)
		{
			// There are many ways to solve this problem and to validate things. This logic is designed to 
			// be easy to understand.
			// You could just create this function without the vIndex parameter at all and just move the 
			// mAvailableFrameResourceIndex property to the next number it should be set to. This code sort
			// of does that anyway.
			// However, this version makes the client code think a bit more.

			// Validate that the parameter passed by the client is 0 through 3.
			if (vIndex < 0 || vIndex > 3)
			{
				// Not a valid value.
				throw new std::exception("Error - M5DxFrameResources::SetAvailableFrameResourceIndex invalid range");
			}

			if (mAvailableFrameResourceIndex == 3)
			{
				// Next must be 0.
				if (vIndex != 0)
				{
					// Not a valid value.
					throw new std::exception("Error - M5DxFrameResources::SetAvailableFrameResourceIndex invalid value");
				}

				mAvailableFrameResourceIndex = vIndex;
			}
			else
			{
				// If here then the value of mAvailableFrameResourceIndex is 0, 1 or 2.

				// Next value must be + 1 of the mAvailableFrameResourceIndex value.
				// Here we don't actually "set" it to the passed parameter value, we just increment it to what it 
				// should be and then validate the client is doing things correctly.
				mAvailableFrameResourceIndex++;

				if (vIndex != mAvailableFrameResourceIndex)
				{
					// The client has some logic error(s) because they asked for the wrong index value to be set.
					throw new std::exception("Error - M5DxFrameResources::SetAvailableFrameResourceIndex invalid value");
				}
			}			

		}

		int M5DxFrameResources::GetPreviousIndex(int vIndex)
		{
			vIndex--;

			if (vIndex < 0)
			{
				// vIndex was zero, so the previous was 3.
				vIndex = 3;
			}
			return vIndex;
		}

		int M5DxFrameResources::GetNextIndex(int vIndex)
		{
			// There are 4 frame resources. We use an int index to get the one we want.
			
			// The new value of the index after incrementing it can only be 0, 1, 2 or 3.
			// If it is now zero, set it to one. If one, set it to two. If two, set to three. 
			// If three, set to zero.

			vIndex++;

			if (vIndex > 3)
			{
				// vIndex was three, so the next is zero.
				vIndex = 0;
			}
			return vIndex;
		}

		M5DxFrameResource& M5DxFrameResources::GetFrameResource(int vIndex)
		{
			// Dereference one of the pointers in the array and return it.
			return *(mM5DxFrameResourceList[vIndex]);

		}

	}
}