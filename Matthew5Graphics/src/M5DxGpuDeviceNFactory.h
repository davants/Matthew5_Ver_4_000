// Davant Stewart
#pragma once

#include "Matthew5StandardMicrosoftHeaders.h"

using namespace DirectX;

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// This class handles the creation of the factory and device interfaces used to work with a Gpu.
		class M5DxGpuDeviceNFactory
		{
		public:
			// This constructor will look for the default graphics desplay device which will be the desktop.
			M5DxGpuDeviceNFactory(HWND vHWND);

			// This constructor will look for a specific display device indicated by the LUID.
			// This used to specify the Oculus HMD (Head Mounted Display).
			// This constructor will loop through the Adapters until if finds the right one.
			M5DxGpuDeviceNFactory(HWND vHWND, const LUID* pLuid);

			IDXGIFactory4 & GetIDXGIFactory4(void);
			ID3D12Device & GetID3D12Device(void);

			HWND GetHWND(void);
			UINT Get4XMSAAQuality(void);
			DXGI_FORMAT GetBackBuffer_DXGI_FORMAT(void);
			DXGI_FORMAT GetDepthStencil_DXGI_FORMAT(void);

			// Version 4.0 
			double GetDedicatedVideoMemoryInGigs(void);

			// Version 4.0 
			bool HasGpuRayTracingTier1_0(void);

		private:
			Microsoft::WRL::ComPtr<IDXGIFactory4> mpIDXGIFactory4;
			Microsoft::WRL::ComPtr<ID3D12Device> mpID3D12Device;
			HWND mHWND = nullptr;
			void ThrowIfFail(HRESULT hr);
			UINT      m4XMSAAQuality = 0;
			DXGI_FORMAT mBackBuffer_DXGI_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
			DXGI_FORMAT mDepthStencil_DXGI_FORMAT = DXGI_FORMAT_D32_FLOAT;

			// DTS - Version 4.0 "RayTracing"
			bool mHasGpuRayTracingTier1_0;

			// DTS - Version 4.0 video memory
			double mDedicatedVideoMemoryInGigs = 0.0;

			// DTS - Version 4.0 LUID of the adaptor chosen.
			LUID mLUID;

			// DTS - Version 4.0 - This only sets the mLUID value based on the one 
			// with the most video memory. This does not create the device.
			void SetBestLUIDBasedOnVideo(void);

			// DTS - Version 4.0 - Now both constructors use the same logic to create 
			// the device. The mLUID must be set before calling this function.
			void CreateID3D12Device(void);

			// DTS - Version 4.0 - Set ray tracing support Tier 1
			void SetRayTracingTier_1_0(void);

			// DTS - Version 4.0 - Set Multi-Sample
			void Set4XMSAAQuality(void);
		};

	}
}