#include "Matthew5DxOculus.h"


namespace Matthew5
{
	namespace Matthew5GraphicsOculus
	{ 
		Matthew5DxOculus::Matthew5DxOculus(HWND vHWND) :
			Matthew5Dx(vHWND)
		{		
			if (SetupOculusComponents() == false)
			{
				return;
			}
	
			// Device and Factory
			mpM5DxGpuDeviceNFactory = std::make_unique<M5DxGpuDeviceNFactory>(vHWND, reinterpret_cast<LUID*>(&mOvrGraphicsLuid));

			// Updated to support frame resources.
			// Command objects; we need 8 command lists, one for each eye times 4 frames.
			size_t vNumberOfCommandLists = 8;
			mpM5DxGpuCommand = std::make_unique<M5DxGpuCommand>(*mpM5DxGpuDeviceNFactory, vNumberOfCommandLists);			

			mpIMatthew5SwapChain = std::make_unique< Matthew5DxOculusSwapChain>(*mpM5DxGpuDeviceNFactory, *(mpM5DxGpuCommand.get()),
				mOvrSession, mOvrHmdDesc);

			// Added to support frame resources. 
			mpM5DxFrameResources = std::make_unique< M5DxFrameResources>(*mpM5DxGpuDeviceNFactory, mpM5DxGpuCommand->GetCommandQueue());


			mpM5DxGpuCommand->EmptyCommandQueueUsingFence();

			// Reset command list
			ThrowIfFail(mpM5DxGpuCommand->GetCommandList(0).Reset(&(mpM5DxGpuCommand->GetCommandAllocator(0)), nullptr));

			// This will call Dx base class function that are common to all derived classes.
			InitCommonBaseComponents();

			// Execute the initialization commands.
			ThrowIfFail(mpM5DxGpuCommand->GetCommandList(0).Close());
			ID3D12CommandList* vpCmdsLists[] = { &(mpM5DxGpuCommand->GetCommandList(0)) };
			mpM5DxGpuCommand->GetCommandQueue().ExecuteCommandLists(_countof(vpCmdsLists), vpCmdsLists);

			// Block until all is done.
			mpM5DxGpuCommand->EmptyCommandQueueUsingFence();	

			HRESULT vHr;

			// Set second window
			RECT vSize = { 0, 0, mOvrHmdDesc.Resolution.w / 2, mOvrHmdDesc.Resolution.h / 2 };
			AdjustWindowRect(&vSize, WS_OVERLAPPEDWINDOW, false);
			const UINT vFlags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;

			if (!SetWindowPos(vHWND, nullptr, 0, 0, vSize.right - vSize.left, vSize.bottom - vSize.top, vFlags))
			{
				return;
			}
	
			// This sample does not support fullscreen transitions.
			vHr = mpM5DxGpuDeviceNFactory->GetIDXGIFactory4().MakeWindowAssociation(vHWND, DXGI_MWA_NO_ALT_ENTER);

			ThrowIfFail(vHr);

			Matthew5DxOculusSwapChain * vpM5OculusSwapChain = (Matthew5DxOculusSwapChain *)mpIMatthew5SwapChain.get();

			for (int eye = 0; eye < 2; ++eye)
			{
				ovrSizei vIdealSize = vpM5OculusSwapChain->GetIdealSize((ENUMM5Eye)eye);
							   
				mEyeRenderViewport[eye].Pos.x = 0;
				mEyeRenderViewport[eye].Pos.y = 0;
				mEyeRenderViewport[eye].Size = vIdealSize;

			}

			mOvrEyeRenderDesc[0] = ovr_GetRenderDesc(mOvrSession, ovrEye_Left, mOvrHmdDesc.DefaultEyeFov[0]);
			mOvrEyeRenderDesc[1] = ovr_GetRenderDesc(mOvrSession, ovrEye_Right, mOvrHmdDesc.DefaultEyeFov[1]);

			mFrameIndex = 0;

			// Create the M5DxSceneObjectList.
			mpM5DxSceneObjectList = std::make_unique<M5DxSceneObjectList>(
				*mpM5DxGpuDeviceNFactory,
				*mpM5DxGpuCommand,
				*mpM5DxFrameResources);


			mIsOculusAvailable = true;

		}
		Matthew5DxOculus::~Matthew5DxOculus()
		{
			// Clean out any remaining commands.
			if (mpM5DxGpuCommand != nullptr)
			{
				mpM5DxGpuCommand->EmptyCommandQueueUsingFence();
			}


			ovr_Shutdown();

			ovr_Destroy(mOvrSession);
		}
		bool Matthew5DxOculus::IsOculusAvailable(void)
		{
			return mIsOculusAvailable;
		}
		void Matthew5DxOculus::Draw( IMatthew5DrawInstructions & vrIMatthew5DrawInstructions)		
		{
			int vFrameResourceIndex = mpM5DxFrameResources->GetAvailableFrameResourceIndex();


			ovrSessionStatus vOvrSessionStatus;
			ovr_GetSessionStatus(mOvrSession, &vOvrSessionStatus);
			if (vOvrSessionStatus.ShouldQuit)
			{
				return;
			}

			if (vOvrSessionStatus.ShouldRecenter)
			{
				ovr_RecenterTrackingOrigin(mOvrSession);
			}

			if (vOvrSessionStatus.IsVisible)
			{
				XMVECTOR vMainCamPos = XMVectorSet(
					vrIMatthew5DrawInstructions.GetCameraPosition().mPositionX, 
					vrIMatthew5DrawInstructions.GetCameraPosition().mPositionY, 
					vrIMatthew5DrawInstructions.GetCameraPosition().mPositionZ, 
					0);

				XMVECTOR vMainCamRot = XMQuaternionIdentity();

				// Get both eye poses simultaneously, with IPD offset already included. 
				ovrPosef vEyeRenderPose[2];
				ovrPosef vHmdToEyePose[2] = { mOvrEyeRenderDesc[0].HmdToEyePose, mOvrEyeRenderDesc[1].HmdToEyePose };

				double vSensorSampleTime;    // sensorSampleTime is fed into the layer later
				ovr_GetEyePoses(mOvrSession, mFrameIndex, ovrTrue, vHmdToEyePose, vEyeRenderPose, &vSensorSampleTime);

				ovrTimewarpProjectionDesc vPosTimewarpProjectionDesc = {};

				// Cast to the Oculus swap chain
				Matthew5DxOculusSwapChain * vpM5OculusSwapChain = (Matthew5DxOculusSwapChain *)mpIMatthew5SwapChain.get();


				// Render Scene to Eye Buffers
				for (int eye = 0; eye < 2; ++eye)
				{
					int vAvailableCommandListIndexForEye = (vFrameResourceIndex * 2) + eye;

					// This will reset the command list and allocator. Reset leaves the command list in an "Open" 
					// state so that new commands can be recorded.
					InitCommandList(vAvailableCommandListIndexForEye);

					ID3D12GraphicsCommandList & vrAvailableCommandList = mpM5DxGpuCommand->GetCommandList(vAvailableCommandListIndexForEye);


					CD3DX12_RESOURCE_BARRIER vResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetD3DColorResource(),
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
						D3D12_RESOURCE_STATE_RENDER_TARGET);

					vrAvailableCommandList.ResourceBarrier(1, &vResourceBarrier);

					if (vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetD3DDepthResource())
					{
						vResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetD3DDepthResource(),
							D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
							D3D12_RESOURCE_STATE_DEPTH_WRITE);

						vrAvailableCommandList.ResourceBarrier(1, &vResourceBarrier);
					}


					{
						// Clear Render target
						vrAvailableCommandList.OMSetRenderTargets(1,
							&vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetRtv(), 		
							false,
							&vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetDsv());

						vrAvailableCommandList.ClearRenderTargetView(
							vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetRtv(), 
							Colors::LightSteelBlue, 0, nullptr);

						vrAvailableCommandList.ClearDepthStencilView(
							vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetDsv(), 
							D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
					}

					mpIMatthew5SwapChain->CommandListSetViewport(vrAvailableCommandList,(ENUMM5Eye)eye);

					mpIMatthew5SwapChain->CommandListSetRECTScissor(vrAvailableCommandList, (ENUMM5Eye)eye);

					//Get the pose information in XM format
					XMVECTOR vEyeQuat = XMVectorSet(vEyeRenderPose[eye].Orientation.x, vEyeRenderPose[eye].Orientation.y,
						vEyeRenderPose[eye].Orientation.z, vEyeRenderPose[eye].Orientation.w);
					XMVECTOR vEyePos = XMVectorSet(vEyeRenderPose[eye].Position.x, vEyeRenderPose[eye].Position.y, vEyeRenderPose[eye].Position.z, 0);


					XMVECTOR vPos = XMVectorAdd(vMainCamPos, XMVector3Rotate(vEyePos, vMainCamRot));
					XMVECTOR vRot = XMQuaternionMultiply(vEyeQuat, vMainCamRot);

					// Setting -1 "Z" as forward then adjust based on rotation.
					XMVECTOR vForward = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), vRot);

					XMMATRIX vView = (XMMatrixLookAtRH(vPos, XMVectorAdd(vPos, vForward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), vRot)));

					// Unlike desktop, a new projection is created for each frame based on eye position information.
					ovrMatrix4f vProjection = ovrMatrix4f_Projection(mOvrEyeRenderDesc[eye].Fov, 0.2f, 1000.0f, ovrProjection_None);

					vPosTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(vProjection, ovrProjection_None);
					XMMATRIX vProj = XMMatrixSet(vProjection.M[0][0], vProjection.M[1][0], vProjection.M[2][0], vProjection.M[3][0],
						vProjection.M[0][1], vProjection.M[1][1], vProjection.M[2][1], vProjection.M[3][1],
						vProjection.M[0][2], vProjection.M[1][2], vProjection.M[2][2], vProjection.M[3][2],
						vProjection.M[0][3], vProjection.M[1][3], vProjection.M[2][3], vProjection.M[3][3]);
					XMMATRIX vViewProj = XMMatrixMultiply(vView, vProj);


					vrAvailableCommandList.SetGraphicsRootSignature(mpID3D12RootSignature.Get());
					vrAvailableCommandList.SetPipelineState(mpID3D12PipelineState.Get());

					D3D12_GPU_VIRTUAL_ADDRESS vCB_ViewProjection_GpuAddress;

					/////////////////
					// NEW VIEW PROJECTION
					// Update new view projection 
					// Version 1.000 code
					// M5ViewProjectionCpu &  vrM5ViewProjectionCpu = mpM5ViewProjectionCpuAndGpu->GetCpuViewProjection();
					// Replaced in Version 2.000 with this.
					M5ViewProjectionCpu &  vrM5ViewProjectionCpu = mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().GetCpuViewProjection();

					

					if (eye == 0)
					{
						// Left
						XMStoreFloat4x4(&vrM5ViewProjectionCpu.GetViewProjection(ENUMM5Eye::M5LeftEye), XMMatrixTranspose(vViewProj));
						// Version 1.000 code
						//mpM5ViewProjectionCpuAndGpu->UpdateGpuWithCpuData(ENUMM5Eye::M5LeftEye);
						//vCB_ViewProjection_GpuAddress = mpM5ViewProjectionCpuAndGpu->GetGpuConstBufferAddress(ENUMM5Eye::M5LeftEye);
						// Replace in Version 2.000 with this.
						mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().UpdateGpuWithCpuData(ENUMM5Eye::M5LeftEye);
						vCB_ViewProjection_GpuAddress = mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().GetGpuConstBufferAddress(ENUMM5Eye::M5LeftEye);
					}
					else
					{
						// Right
						XMStoreFloat4x4(&vrM5ViewProjectionCpu.GetViewProjection(ENUMM5Eye::M5RightEye), XMMatrixTranspose(vViewProj));
						// Version 1.000 code
						//mpM5ViewProjectionCpuAndGpu->UpdateGpuWithCpuData(ENUMM5Eye::M5RightEye);
						//vCB_ViewProjection_GpuAddress = mpM5ViewProjectionCpuAndGpu->GetGpuConstBufferAddress(ENUMM5Eye::M5RightEye);
						// Replace in Version 2.000 with this.
						mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().UpdateGpuWithCpuData(ENUMM5Eye::M5RightEye);
						vCB_ViewProjection_GpuAddress = mpM5DxFrameResources->GetAvailableResource().GetM5ViewProjectionCpuAndGpu().GetGpuConstBufferAddress(ENUMM5Eye::M5RightEye);
					}

					// Reminder: Root parameter can be a table, root descriptor or root constants.
					//		See Matthew5Dx::CreateRootSignature(void) function for detailed notes 
					//			on the ambiguities use of the word "constant" in DirectX.

					// Passing an argument as a "root descriptor". 
					// The first parameter is the root parameter index.
					vrAvailableCommandList.SetGraphicsRootConstantBufferView(1, vCB_ViewProjection_GpuAddress);

					for (size_t vIndex = 0; vIndex < mpM5DxFrameResources->GetAvailableResource().GetSceneObjectList().GetSize(); vIndex++)
					{
						M5SceneObject & vrM5SceneObject = mpM5DxFrameResources->GetAvailableResource().GetSceneObjectList().GetM5DxFrameSceneObject(vIndex).GetM5SceneObject();

						D3D12_GPU_VIRTUAL_ADDRESS vCB_WorldSpace_GpuAddress
							= vrM5SceneObject.GetGpuConstBufferAddressThatHasWorldSpaceNMoreData();

						// Passing an argument as a "root descriptor". 
						// The first parameter is the root parameter index.
						vrAvailableCommandList.SetGraphicsRootConstantBufferView(0, vCB_WorldSpace_GpuAddress);

						//////////////////////////////
						// Now tell DirectX where to find our Triangles!

						// Create and give the Gpu a descriptor(view) of where the verticies are located.
						vrAvailableCommandList.IASetVertexBuffers(0, 1, &vrM5SceneObject.GetM5TriCpuGpuResources().GetGpuVertexBufferView());

						// Create and give the Gpu a descriptor of where the indicies are located.
						vrAvailableCommandList.IASetIndexBuffer(&vrM5SceneObject.GetM5TriCpuGpuResources().GetGpuIndexBufferView());

						// Tell DirectX we get it triangles.
						vrAvailableCommandList.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						// Everything is set. Now Draw/Render this scene object.

						// We pass the number of indicies that we want to draw.
						// We get the number of indicies to draw by getting the size of the 
						// index buffer in bytes and dividing by the size of each index.
						vrAvailableCommandList.DrawIndexedInstanced(vrM5SceneObject.GetM5TriCpuGpuResources().GetGpuIndexBufferView().SizeInBytes / sizeof(int16_t), 1, 0, 0, 0);


					}


					vResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
						vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetD3DColorResource(),
						D3D12_RESOURCE_STATE_RENDER_TARGET,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

					vrAvailableCommandList.ResourceBarrier(1, &vResourceBarrier);

					if (vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetD3DDepthResource())
					{
						vResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->GetD3DDepthResource(),
							D3D12_RESOURCE_STATE_DEPTH_WRITE,
							D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

						vrAvailableCommandList.ResourceBarrier(1, &vResourceBarrier);
					}

					{
						// Submit Command List
						ThrowIfFail(vrAvailableCommandList.Close());

						ID3D12CommandList* ppCommandLists[] = { &vrAvailableCommandList };
						mpM5DxGpuCommand->GetCommandQueue().ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
					}


					// Commit rendering to the swap chain
					vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->Commit();
				} 

				// Initialize our single full screen Fov layer.
				ovrLayerEyeFovDepth vOvrLayerEyeFovDepth = {};
				vOvrLayerEyeFovDepth.Header.Type = ovrLayerType_EyeFovDepth;
				vOvrLayerEyeFovDepth.Header.Flags = 0;
				vOvrLayerEyeFovDepth.ProjectionDesc = vPosTimewarpProjectionDesc;
				vOvrLayerEyeFovDepth.SensorSampleTime = vSensorSampleTime;

				for (int eye = 0; eye < 2; ++eye)
				{
					vOvrLayerEyeFovDepth.ColorTexture[eye] = vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->mOvrTextureChain;
					vOvrLayerEyeFovDepth.DepthTexture[eye] = vpM5OculusSwapChain->mpOculusEyeRenderTextureList[eye]->mOvrDepthTextureChain;
					vOvrLayerEyeFovDepth.Viewport[eye] = mEyeRenderViewport[eye];
					vOvrLayerEyeFovDepth.Fov[eye] = mOvrHmdDesc.DefaultEyeFov[eye];
					vOvrLayerEyeFovDepth.RenderPose[eye] = vEyeRenderPose[eye];
				}

				ovrLayerHeader* vpOvrLayerHeader = &vOvrLayerEyeFovDepth.Header;
				ovrResult vOvrResult = ovr_SubmitFrame(mOvrSession, mFrameIndex, nullptr, &vpOvrLayerHeader, 1);
				// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
				if (!OVR_SUCCESS(vOvrResult))
				{
					return; 
				}

				mFrameIndex++;
			}

			mpM5DxFrameResources->GetAvailableResource().SetTheFence();

			MoveToNextFrameResource();

			return; 
		}

		bool Matthew5DxOculus::SetupOculusComponents(void)
		{
			// Initializes LibOVR, and the Rift
			ovrInitParams vOvrInitParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
			ovrResult vOvrResult = ovr_Initialize(&vOvrInitParams);
			auto vOculusResult = OVR_SUCCESS(vOvrResult);

			if (vOculusResult == false)
			{
				mIsOculusAvailable = false;
				return false;
			}

			mFrameIndex = 0;

			vOvrResult = ovr_Create(&mOvrSession, &mOvrGraphicsLuid);

			if (!OVR_SUCCESS(vOvrResult))
			{
				mIsOculusAvailable = false;
				return false;
			}

			mOvrHmdDesc = ovr_GetHmdDesc(mOvrSession);

			return true;
		}

		void Matthew5DxOculus::InitCommandList(int vIndex)
		{
			HRESULT vHr = mpM5DxGpuCommand->GetCommandAllocator(vIndex).Reset();

			ThrowIfFail(vHr);

			vHr = mpM5DxGpuCommand->GetCommandList(vIndex).Reset(&mpM5DxGpuCommand->GetCommandAllocator(vIndex), nullptr);

			ThrowIfFail(vHr);
		}

	}
}