#include "Matthew5Dx.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		Matthew5Dx::Matthew5Dx(HWND vHWND) 			
		{
		}

		Matthew5Dx::~Matthew5Dx()
		{
		}

		IMatthew5SceneObjectList& Matthew5Dx::GetSceneObjectList(void)
		{
			return *((IMatthew5SceneObjectList*)mpM5DxSceneObjectList.get());
		}


		void Matthew5Dx::InitCommonBaseComponents(void)
		{
			// First the non-virtual functions.
			// This means the derived classes share the same root signature, shaders, input layout and PSO.

			// Root Signature
			CreateRootSignature();

			// Input Layout and Shaders
			CreateShader();

			// Vertex Input Layout
			CreateVertexInputLayout();

			// The BuildPSO uses the compiled shader, root signature and so on to create the PSO.
			BuildPSO();

		}

		void Matthew5Dx::CreateRootSignature(void)
		{
			// Root parameter can be a table, root descriptor or root constants.
			
			// SIDE NOTE - VERY BAD TERMS!  
			// The term "root constant" has the word "CONSTANT"!
			// Also note allocated memory on the GPU is called a constant buffer, which 
			// also has "CONSTANT"!
			// They are not related.
			// A constant buffer is a blob on the GPU.
			// A root constant parameter is raw data in a parameter without the indirection of a 
			// view or table.
			// All of these naming collisions makes it very hard to learn. 
			// Also note that "const" in C++ means it is not to be changed; however, a GPU const buffer 
			// is very much changeable.


			// For this version, we will only have 2 parameter!
			// For an object in the world, like a triangle, this will 
			// include the world space for the triangle. The other will  
			// include the "view projection" data.
			CD3DX12_ROOT_PARAMETER vRootParameterArray[2];

			const static int vShaderRegisterB0 = 0;
			// The parameter, 0, is the shaderRegister
			//		This one is for the object's world space position.
			const static int vShaderRegisterB1 = 1;
			// The paramter, 1, is the shaderRegister
			//		This one is for the camera - view projection

			// SIDE NOTE - Even though this function name has the word "Constant" it is setting a 
			// "root descriptor" and NOT a "root constant";
			// A "root descriptor" meaning that we will pass a descriptor as the argument and will 
			// not be using a descriptor heap.
			vRootParameterArray[0].InitAsConstantBufferView(vShaderRegisterB0); // register b0
			vRootParameterArray[1].InitAsConstantBufferView(vShaderRegisterB1); // register b1

			// A root signature is an array of root parameters.
			CD3DX12_ROOT_SIGNATURE_DESC vCD3DX12_ROOT_SIGNATURE_DESC(2, vRootParameterArray,
				0, NULL,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
			ComPtr<ID3DBlob> vpS = nullptr;
			ComPtr<ID3DBlob> vpErrorBlob = nullptr;
			HRESULT hr = D3D12SerializeRootSignature(&vCD3DX12_ROOT_SIGNATURE_DESC, D3D_ROOT_SIGNATURE_VERSION_1,
				vpS.GetAddressOf(), vpErrorBlob.GetAddressOf());

			if (vpErrorBlob != nullptr)
			{
				::OutputDebugStringA((char*)vpErrorBlob->GetBufferPointer());
			}
			ThrowIfFail(hr);

			ThrowIfFail(mpM5DxGpuDeviceNFactory->GetID3D12Device().CreateRootSignature(
				0,
				vpS->GetBufferPointer(),
				vpS->GetBufferSize(),
				IID_PPV_ARGS(mpID3D12RootSignature.GetAddressOf())));
		}

		void Matthew5Dx::CreateShader(void)
		{
			std::wstring vFilePath = L"Shaders\\Default.hlsl";

			std::wstring vFullPath = M5Util::FindFullPath(vFilePath);

			if (vFullPath.length() == 0)
			{
				throw new std::exception("Error opening Shader.");
			}

			UINT vCompileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
			vCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

			HRESULT hr = S_OK;

			ComPtr<ID3DBlob> vpErrorsVertex;
			hr = D3DCompileFromFile(vFullPath.c_str(), nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"VS", "vs_5_0", vCompileFlags, 0, &mpID3DBlobVertexShader, &vpErrorsVertex);

			if (vpErrorsVertex != nullptr)
			{
				OutputDebugStringA((char*)vpErrorsVertex->GetBufferPointer());
			}

			ThrowIfFail(hr);

			ComPtr<ID3DBlob> vpErrorsPixel;
			hr = D3DCompileFromFile(vFullPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"PS", "ps_5_0", vCompileFlags, 0, &mpID3DBlobPixelShader, &vpErrorsPixel);

			if (vpErrorsPixel != nullptr)
			{
				OutputDebugStringA((char*)vpErrorsPixel->GetBufferPointer());
			}

			ThrowIfFail(hr);
		}


		void Matthew5Dx::CreateVertexInputLayout(void)
		{
			mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};
		}

		void Matthew5Dx::BuildPSO(void)
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC vPsoDesc;

			ZeroMemory(&vPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			vPsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
			vPsoDesc.pRootSignature = mpID3D12RootSignature.Get();

			vPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mpID3DBlobVertexShader->GetBufferPointer()),
				mpID3DBlobVertexShader->GetBufferSize()
			};

			vPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mpID3DBlobPixelShader->GetBufferPointer()),
				mpID3DBlobPixelShader->GetBufferSize()
			};

			vPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			vPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			vPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			vPsoDesc.SampleMask = UINT_MAX;
			vPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			vPsoDesc.NumRenderTargets = 1;
			vPsoDesc.RTVFormats[0] = mpM5DxGpuDeviceNFactory->GetBackBuffer_DXGI_FORMAT(); //DXGI_FORMAT_R8G8B8A8_UNORM
			vPsoDesc.SampleDesc.Count = mpIMatthew5SwapChain->GetMSAACount();
			vPsoDesc.SampleDesc.Quality = mpIMatthew5SwapChain->Get4XMSAAIndicator() ? (mpM5DxGpuDeviceNFactory->Get4XMSAAQuality() - 1) : 0;
			vPsoDesc.DSVFormat = mpM5DxGpuDeviceNFactory->GetDepthStencil_DXGI_FORMAT();

			ThrowIfFail(mpM5DxGpuDeviceNFactory->GetID3D12Device().CreateGraphicsPipelineState(&vPsoDesc, IID_PPV_ARGS(&mpID3D12PipelineState)));
		}

		int Matthew5Dx::MoveToNextFrameResource(void)
		{
			// Set the next available frame resource and wait for it to be available.
			int vFrameResourceIndex = mpM5DxFrameResources->GetNextIndex(
				mpM5DxFrameResources->GetAvailableFrameResourceIndex());

			mpM5DxFrameResources->SetAvailableFrameResourceIndex(vFrameResourceIndex);

			// Validate it is available.
			mpM5DxFrameResources->GetAvailableResource().WaitForFence();

			return vFrameResourceIndex;
		}

		void Matthew5Dx::ThrowIfFail(HRESULT hr)
		{
			if (hr != S_OK)
			{
				throw new std::exception("Error - Matthew5Dx");
			}
		}		

	}
}