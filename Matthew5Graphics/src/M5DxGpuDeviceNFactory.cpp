#include "M5DxGpuDeviceNFactory.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		M5DxGpuDeviceNFactory::M5DxGpuDeviceNFactory(HWND vHWND) : mHWND(vHWND),
			mHasGpuRayTracingTier1_0(false)
		{
			HRESULT hr = S_OK;

#if defined(DEBUG) || defined(_DEBUG) 
			{
				ComPtr<ID3D12Debug> vpDebugController;
				ThrowIfFail( D3D12GetDebugInterface(IID_PPV_ARGS(&vpDebugController)));

				vpDebugController->EnableDebugLayer();
			}
#endif

			// GPU Factory.
			ThrowIfFail(CreateDXGIFactory1(IID_PPV_ARGS(&mpIDXGIFactory4)));

			// Version 4.0 - Set the best LUID
			// This constructor is not given an LUID, so pick the best one.
			SetBestLUIDBasedOnVideo();

			CreateID3D12Device();
			
			Set4XMSAAQuality();

			SetRayTracingTier_1_0();

		}
		M5DxGpuDeviceNFactory::M5DxGpuDeviceNFactory(HWND vHWND, const LUID * pLuid) : mHWND(vHWND) ,
			mLUID(*pLuid)
		{
			// GPU Factory.
			ThrowIfFail(CreateDXGIFactory1(IID_PPV_ARGS(&mpIDXGIFactory4)));

			CreateID3D12Device();

			Set4XMSAAQuality();

			SetRayTracingTier_1_0();
		}
		IDXGIFactory4 & M5DxGpuDeviceNFactory::GetIDXGIFactory4(void)
		{
			return *(mpIDXGIFactory4.Get());
		}
		ID3D12Device & M5DxGpuDeviceNFactory::GetID3D12Device(void)
		{
			return *(mpID3D12Device.Get());			
		}
		HWND M5DxGpuDeviceNFactory::GetHWND(void)
		{
			return mHWND;
		}
		UINT M5DxGpuDeviceNFactory::Get4XMSAAQuality(void)
		{
			return m4XMSAAQuality;
		}
		DXGI_FORMAT M5DxGpuDeviceNFactory::GetBackBuffer_DXGI_FORMAT(void)
		{
			return mBackBuffer_DXGI_FORMAT;
		}
		DXGI_FORMAT M5DxGpuDeviceNFactory::GetDepthStencil_DXGI_FORMAT(void)
		{
			return mDepthStencil_DXGI_FORMAT;
		}
		double M5DxGpuDeviceNFactory::GetDedicatedVideoMemoryInGigs(void)
		{
			return mDedicatedVideoMemoryInGigs;
		}
		bool M5DxGpuDeviceNFactory::HasGpuRayTracingTier1_0(void)
		{
			return mHasGpuRayTracingTier1_0;
		}
		void M5DxGpuDeviceNFactory::ThrowIfFail(HRESULT hr)
		{
			if (hr != S_OK)
			{
				throw new std::exception("Error - M5DxGpuDeviceNFactory");
			}
		}
		void M5DxGpuDeviceNFactory::SetBestLUIDBasedOnVideo(void)
		{
			IDXGIAdapter* vpAdapter = nullptr;
			bool vFoundAdaptor = false;

			SIZE_T vMemory = 0;

			// Loop through the adaptors and find the one that matches the passed LUID.
			for (UINT iAdapter = 0; mpIDXGIFactory4->EnumAdapters(iAdapter, &vpAdapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
			{
				DXGI_ADAPTER_DESC adapterDesc;
				vpAdapter->GetDesc(&adapterDesc);

				if (vMemory < adapterDesc.DedicatedVideoMemory)
				{
					vMemory = adapterDesc.DedicatedVideoMemory;
					mLUID = adapterDesc.AdapterLuid;
				}

				vpAdapter->Release();
			}

		}
		void M5DxGpuDeviceNFactory::CreateID3D12Device(void)
		{
			mDedicatedVideoMemoryInGigs = 0.0;

			IDXGIAdapter* vpAdapter = nullptr;
			bool vFoundAdaptor = false;

			// Loop through the adaptors and find the one that matches the passed LUID.
			for (UINT iAdapter = 0; mpIDXGIFactory4->EnumAdapters(iAdapter, &vpAdapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
			{
				DXGI_ADAPTER_DESC adapterDesc;
				vpAdapter->GetDesc(&adapterDesc);


				// Note the memcmp, this is the C++ memory compair function. It will return 0 if the bytes at 
				// the two memory locations are the same.
				if ( memcmp(&adapterDesc.AdapterLuid, &mLUID, sizeof(LUID)) == 0)
				{
					vFoundAdaptor = true;

					mDedicatedVideoMemoryInGigs = adapterDesc.DedicatedVideoMemory / 1000000000.0;					

					break;
				}
				vpAdapter->Release();
			}

			if (vFoundAdaptor == false)
			{
				ThrowIfFail(E_FAIL);
			}

#if defined(DEBUG) || defined(_DEBUG) 
			{
				ComPtr<ID3D12Debug> debugController;
				ThrowIfFail(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));

				debugController->EnableDebugLayer();
			}
#endif
			// GPU Device
			ThrowIfFail(D3D12CreateDevice(vpAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mpID3D12Device)));

			vpAdapter->Release();		

		}
		void M5DxGpuDeviceNFactory::SetRayTracingTier_1_0(void)
		{
			// DTS - Version 4.0 - Check "RayTracing" support check
			D3D12_FEATURE_DATA_D3D12_OPTIONS5 vFeature;

			ThrowIfFail(mpID3D12Device->CheckFeatureSupport(
				D3D12_FEATURE_D3D12_OPTIONS5,
				&vFeature,
				sizeof(vFeature)));

			D3D12_RAYTRACING_TIER vRayTracingTier = vFeature.RaytracingTier;

			if (vRayTracingTier == D3D12_RAYTRACING_TIER_1_0)
			{
				mHasGpuRayTracingTier1_0 = true;
			}
			else
			{
				mHasGpuRayTracingTier1_0 = false;
			}
		}
		void M5DxGpuDeviceNFactory::Set4XMSAAQuality(void)
		{
			// Check Features
			D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS vMultiSampleQualityLevels;
			vMultiSampleQualityLevels.Format = mBackBuffer_DXGI_FORMAT;
			vMultiSampleQualityLevels.SampleCount = 4;
			vMultiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
			vMultiSampleQualityLevels.NumQualityLevels = 0;
			
			ThrowIfFail(mpID3D12Device->CheckFeatureSupport(
				D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
				&vMultiSampleQualityLevels,
				sizeof(vMultiSampleQualityLevels)));

			m4XMSAAQuality = vMultiSampleQualityLevels.NumQualityLevels;
			if (m4XMSAAQuality < 1)
			{
				throw new std::exception("Error - InitializeDirectX - m4XMSAAQuality is Zero");
			}
		}
	}
}