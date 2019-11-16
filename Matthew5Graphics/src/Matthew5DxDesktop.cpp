#include "Matthew5DxDesktop.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		Matthew5DxDesktop::Matthew5DxDesktop(HWND vHWND) :
			Matthew5Dx(vHWND)			
		{
			// Device and Factory
			mpM5DxGpuDeviceNFactory = std::make_unique<M5DxGpuDeviceNFactory>(vHWND);

			// Updated to support frame resources.
			// Command objects, we need 4 to support 4 frames.
			size_t vNumberOfCommandLists = 4;
			mpM5DxGpuCommand = std::make_unique<M5DxGpuCommand>(*mpM5DxGpuDeviceNFactory, vNumberOfCommandLists);

			mpIMatthew5SwapChain = std::make_unique<Matthew5DxDesktopSwapChain>(*mpM5DxGpuDeviceNFactory, *(mpM5DxGpuCommand.get()));			

			// Added to support frame resources.
			mpM5DxFrameResources = std::make_unique<M5DxFrameResources>(*mpM5DxGpuDeviceNFactory, mpM5DxGpuCommand->GetCommandQueue());
			
			mpM5DxGpuCommand->EmptyCommandQueueUsingFence();

			// Reset command list
			ThrowIfFail(mpM5DxGpuCommand->GetCommandList(0).Reset(&(mpM5DxGpuCommand->GetCommandAllocator(0)), nullptr));

			// This will call Dx base class function that are common to all derived classes.
			InitCommonBaseComponents();

			// The Desktop projection is stable once things have started. 
			CreateProjection();

			// Execute the initialization commands.
			ThrowIfFail(mpM5DxGpuCommand->GetCommandList(0).Close());
			ID3D12CommandList* cmdsLists[] = { &(mpM5DxGpuCommand->GetCommandList(0)) };
			mpM5DxGpuCommand->GetCommandQueue().ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

			// Block until all is done.
			mpM5DxGpuCommand->EmptyCommandQueueUsingFence();

			// Create the M5DxSceneObjectList.
			mpM5DxSceneObjectList = std::make_unique<M5DxSceneObjectList>(
				*mpM5DxGpuDeviceNFactory,
				*mpM5DxGpuCommand,
				*mpM5DxFrameResources);


		}

		Matthew5DxDesktop::~Matthew5DxDesktop()
		{
			// Clean out any remaining commands.
			if (mpM5DxGpuCommand != nullptr)
			{
				mpM5DxGpuCommand->EmptyCommandQueueUsingFence();
			}

		}

		void Matthew5DxDesktop::Draw(IMatthew5DrawInstructions & vrIMatthew5DrawInstructions)
		{
			Update(vrIMatthew5DrawInstructions);

			DrawScene();

			// Note that below we set a fence and then move to another frame resource 
			// and wait for that fence; however, they are not the same fence. So, this 
			// means the fence we set here is not the one we are waiting on in the 
			// call to "MoveToNextFrameResource".

			// Add a fence now that all the commands have been added to the queue.
			mpM5DxFrameResources->GetAvailableResource().SetTheFence();

			MoveToNextFrameResource();
		}

		void Matthew5DxDesktop::Update(IMatthew5DrawInstructions & vrIMatthew5DrawInstructions)
		{
			// For this version, all we have to update is the camera view projection.

			M5ViewProjectionCpu &  vrM5ViewProjectionCpu = mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().GetCpuViewProjection();
				
			XMMATRIX vXMMATRIXProjection = XMLoadFloat4x4(&mProjection);

			XMVECTOR vCameraPositionInWorldSpace = XMVectorSet(
				vrIMatthew5DrawInstructions.GetCameraPosition().mPositionX,
				vrIMatthew5DrawInstructions.GetCameraPosition().mPositionY,
				vrIMatthew5DrawInstructions.GetCameraPosition().mPositionZ, 1.0f);

			XMVECTOR vWorldSpacePointTheCameraIsLookingAt = XMVectorSet(
				vrIMatthew5DrawInstructions.GetCameraLookAt().mPositionX,
				vrIMatthew5DrawInstructions.GetCameraLookAt().mPositionY,
				vrIMatthew5DrawInstructions.GetCameraLookAt().mPositionZ, 0.0f);

			XMVECTOR vWhichWayIsUp = XMVectorSet(
				vrIMatthew5DrawInstructions.GetCameraUpDirection().mPositionX,
				vrIMatthew5DrawInstructions.GetCameraUpDirection().mPositionY,
				vrIMatthew5DrawInstructions.GetCameraUpDirection().mPositionZ, 0.0f);


			// Using DirectX Right Handed
			// DTS - Changing over to Right Handed so it matches Oculus and Bullet.
			XMMATRIX vXMMATRIXViewA = XMMatrixLookAtRH(vCameraPositionInWorldSpace, vWorldSpacePointTheCameraIsLookingAt, vWhichWayIsUp);
			XMStoreFloat4x4(&mView, vXMMATRIXViewA);

			XMMATRIX vXMMATRIXViewProj = XMMatrixMultiply(vXMMATRIXViewA, vXMMATRIXProjection);
			XMStoreFloat4x4(&vrM5ViewProjectionCpu.GetViewProjection(ENUMM5Eye::M5LeftEye), XMMatrixTranspose(vXMMATRIXViewProj));

			mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().UpdateGpuWithCpuData(ENUMM5Eye::M5LeftEye);
			
		}

		void Matthew5DxDesktop::DrawScene(void)
		{
			int vAvailableFrameResourceIndex =  mpM5DxFrameResources->GetAvailableFrameResourceIndex();

			ID3D12GraphicsCommandList & vrAvailableCommandList = mpM5DxGpuCommand->GetCommandList(vAvailableFrameResourceIndex);

			Matthew5DxDesktopSwapChain * vpMatthew5DxDesktopSwapChain = (Matthew5DxDesktopSwapChain *)mpIMatthew5SwapChain.get();

			// Analogy: For the command list, command allocator and command Queue we will use the tape cassette and 
			//		tape recorder analogy. 
			// The Command List is the tape recorder. 
			// The Command Allocator is the tape cassette. 
			// The Command Queue is a pile of tape cassettes that need to be played.

			// Reset the command allocator. 
			// Analogy: This is the tape cassette and is being cleaned of its data. The tape cassette is in the Gpu.
			ThrowIfFail(mpM5DxGpuCommand->GetCommandAllocator(vAvailableFrameResourceIndex).Reset());

			// Reset the command list, giving it the command allocator. 
			// Analogy: Clean out the tape recorder and put the tape cassette in.
			ThrowIfFail(vrAvailableCommandList.Reset(&(mpM5DxGpuCommand->GetCommandAllocator(vAvailableFrameResourceIndex)), mpID3D12PipelineState.Get()));


			/////////////////////////////////////////////////////////////////////////
			// Now record commands using the command list, recorded to the command allocator.
			// Analogy: Use the tape recorder to record things to the tape cassette.

			vpMatthew5DxDesktopSwapChain->CommandListSetViewport(vrAvailableCommandList,ENUMM5Eye::M5LeftEye);

			vpMatthew5DxDesktopSwapChain->CommandListSetRECTScissor(vrAvailableCommandList,ENUMM5Eye::M5LeftEye);

			// Let the Gpu know we are changing the buffer, back buffer, from "read only" to a "write". 
			// From "Present to the screen mode" to "Render/Write mode". We will be writing/drawing the 
			//		image to the buffer.
			vrAvailableCommandList.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vpMatthew5DxDesktopSwapChain->CurrentBackBuffer(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			// Write to the back buffer(render target) a nice steel blue color and also reset the depth buffer.
			vrAvailableCommandList.ClearRenderTargetView(vpMatthew5DxDesktopSwapChain->CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
			vrAvailableCommandList.ClearDepthStencilView(vpMatthew5DxDesktopSwapChain->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

			// Set where the Shaders will render(draw) to. OM means "output merger".
			vrAvailableCommandList.OMSetRenderTargets(1,
				&vpMatthew5DxDesktopSwapChain->CurrentBackBufferView(), true, &vpMatthew5DxDesktopSwapChain->DepthStencilView());

			// Set the root signature we will be using to pass data to the Shaders.
			vrAvailableCommandList.SetGraphicsRootSignature(mpID3D12RootSignature.Get());


			//////////////////////////////////////////
			// Render the image to the back buffer which means draw all the scene objects.
			DrawSceneObjects(&vrAvailableCommandList);
			///////////////////////////////////////

			// Let the Gpu know we are changing the buffer, back buffer, from "write" to "read only".			
			// From "Render/Write mode" to "Present to the screen mode". We are done writing/drawing the 
			//		image to the buffer so we can tell the Gpu it can turn off "Rendering" to this buffer.
			vrAvailableCommandList.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vpMatthew5DxDesktopSwapChain->CurrentBackBuffer(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

			// Done recording commands.
			// Analogy: Turn off the cassette recorder.
			ThrowIfFail(vrAvailableCommandList.Close());

			// Add the command list recorded commands to the command queue for execution at some future point in time.
			// Analogy: Put the cassette tape in the pile of cassettes to be played at some future point in time.
			ID3D12CommandList* cmdsLists[] = { &vrAvailableCommandList };
			mpM5DxGpuCommand->GetCommandQueue().ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

			// Swap the back and front buffers. The image/picture has been drawn and is finished. Now show 
			// it to the screen.
			vpMatthew5DxDesktopSwapChain->Present();

		}

		void Matthew5DxDesktop::DrawSceneObjects(ID3D12GraphicsCommandList * vpCommandList)
		{
			D3D12_GPU_VIRTUAL_ADDRESS vCB_ViewProjection = mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().GetGpuConstBufferAddress(ENUMM5Eye::M5LeftEye);

			// Passing an argument as a "root descriptor". 
			// The first parameter is the root parameter index.
			vpCommandList->SetGraphicsRootConstantBufferView(1, vCB_ViewProjection);

			for (size_t vIndex = 0; vIndex < mpM5DxFrameResources->GetAvailableResource().GetSceneObjectList().GetSize(); vIndex++)
			{
				// Check to see if the previous frame resource scene object has a newer update.
				int vPreviousFrameResourceIndex = mpM5DxFrameResources->GetPreviousIndex(
					mpM5DxFrameResources->GetAvailableFrameResourceIndex());

				// Get a reference to the previous scene object.
				M5SceneObject& vrPreviousM5SceneObject = mpM5DxFrameResources->GetFrameResource(vPreviousFrameResourceIndex).GetSceneObjectList().GetM5DxFrameSceneObject(vIndex).GetM5SceneObject();

				// Get the currently available scene object.
				M5SceneObject & vrM5SceneObject = mpM5DxFrameResources->GetAvailableResource().GetSceneObjectList().GetM5DxFrameSceneObject(vIndex).GetM5SceneObject();


				// Does the currently available scene object need to be updated using the previous 
				// scene object?

				if (vrM5SceneObject.GetUpdateCounter() < vrPreviousM5SceneObject.GetUpdateCounter())
				{
					// The previous scene object is newer than this one we are about to draw, so 
					// update the one we are about to draw!

					vrM5SceneObject.UpdatePosition(vrPreviousM5SceneObject.GetM5SceneObjectCpuAndGpuPosition().mM5SceneObjectCpuPosition,
						vrPreviousM5SceneObject.GetUpdateCounter());

				}



				D3D12_GPU_VIRTUAL_ADDRESS vCB_WorldSpace_GpuAddress 
					= vrM5SceneObject.GetGpuConstBufferAddressThatHasWorldSpaceNMoreData();

				// Passing an argument as a "root descriptor". 
				// The first parameter is the root parameter index.
				vpCommandList->SetGraphicsRootConstantBufferView(0, vCB_WorldSpace_GpuAddress);

				//////////////////////////////
				// Now tell DirectX where to find our Triangles!

				// Create and give the Gpu a descriptor(view) of where the verticies are located.
				vpCommandList->IASetVertexBuffers(0, 1, &vrM5SceneObject.GetM5TriCpuGpuResources().GetGpuVertexBufferView());

				// Create and give the Gpu a descriptor of where the indicies are located.
				vpCommandList->IASetIndexBuffer(&vrM5SceneObject.GetM5TriCpuGpuResources().GetGpuIndexBufferView());

				// Tell DirectX we get it triangles.
				vpCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// Everything is set. Now Draw/Render this scene object.
				
				// We pass the number of indicies that we want to draw.
				// We get the number of indicies to draw by getting the size of the 
				// index buffer in bytes and dividing by the size of each index.
				vpCommandList->DrawIndexedInstanced(vrM5SceneObject.GetM5TriCpuGpuResources().GetGpuIndexBufferView().SizeInBytes / sizeof(int16_t), 1, 0, 0, 0);

			}
					   			 
		}

		void Matthew5DxDesktop::CreateProjection(void)
		{
			// The view projection has two parts; the view and the projection. :)
			// The projection will not change per frame so it can be updated here.
			// The view part of the view projection is dependent on the camera location which 
			// will change; therefore, view will be updated before each draw call and not set here.

			// Our swap chain object has the screen width and height which we need.
			Matthew5DxDesktopSwapChain  * vpMatthew5DxDesktopSwapChain = (Matthew5DxDesktopSwapChain *)mpIMatthew5SwapChain.get();

			float vFovAngleY = 0.25f*MyPi;
			float vAspectRatio = (float)vpMatthew5DxDesktopSwapChain->GetScreenAreaWidth() / (float)vpMatthew5DxDesktopSwapChain->GetScreenAreaHeight();
			float vNearZ = 1.0f;
			float vFarZ = 1000.0f;

			// Using DirectX Right Handed
			// DTS - Changing over to Right Handed so it matches Oculus and Bullet.
			XMMATRIX vXMMATRIXProjection = XMMatrixPerspectiveFovRH(vFovAngleY, vAspectRatio, vNearZ, vFarZ);
			XMStoreFloat4x4(&mProjection, vXMMATRIXProjection);

			// Notice what is NOT being done here. The view projection is not being set because
			// we can not do the view here in setup code. The view will change as the Player moves around.
			// The view will be set before each draw call.
			// Then the view projection will be set using the new view and the stable projection 
			// that we setup here. After the view projection is set the draw can be made.
		}

	}
}