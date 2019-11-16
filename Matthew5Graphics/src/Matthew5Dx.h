// Davant Stewart
#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"
#include "IMatthew5Graphics.h"
#include "M5DxGpuDeviceNFactory.h"
#include "M5TriFactory.h"
#include "M5SceneObject.h"
#include "M5DxGpuCommand.h"
#include "Matthew5DxDesktopSwapChain.h"
#include "M5ViewProjectionCpuAndGpu.h"
#include "M5DxFrameResources.h"
#include "M5DxFrameSceneObjectList.h"
#include "M5Util.h"
#include "M5DxSceneObjectList.h"


// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This implements DirectX logic needed by both Desktop and Oculus and any future implementations 
		// that use DirectX. This is an abstract class and must be derived from by other classes 
		// that finish the requirements of the IMatthew5Graphics interface.
		class Matthew5Dx : public IMatthew5Graphics
		{
		public:
			Matthew5Dx(HWND vHWND);
			virtual ~Matthew5Dx();

			virtual IMatthew5SceneObjectList& GetSceneObjectList(void);

		protected:
			// Set the next available frame resource and wait for it to be available.
			int MoveToNextFrameResource(void);

			virtual void ThrowIfFail(HRESULT hr);

			void InitCommonBaseComponents(void);
			void CreateRootSignature(void);
			void CreateShader(void);
			void CreateVertexInputLayout(void);
			void BuildPSO(void);

			std::unique_ptr<M5DxGpuDeviceNFactory>  mpM5DxGpuDeviceNFactory;
			std::unique_ptr<M5DxGpuCommand> mpM5DxGpuCommand = nullptr;
			ComPtr<ID3D12PipelineState> mpID3D12PipelineState;
			ComPtr<ID3D12RootSignature> mpID3D12RootSignature = nullptr;
			ComPtr<ID3DBlob> mpID3DBlobVertexShader;
			ComPtr<ID3DBlob> mpID3DBlobPixelShader;
			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
			std::unique_ptr<M5DxFrameResources> mpM5DxFrameResources;
			std::unique_ptr<M5DxSceneObjectList> mpM5DxSceneObjectList;


			std::unique_ptr< IMatthew5SwapChain> mpIMatthew5SwapChain = nullptr;


		};
	}
}


