#pragma once

#include <iostream>
#include <memory>
#include <Windows.h>
#include "Matthew5DxDescriptorHeapManager.h"

using namespace Matthew5::Matthew5Graphics;


/************************************************************************************
Filename    :   main.cpp
Content     :   First-person view test application for Oculus Rift
Created     :   10/28/2015

Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*************************************************************************************/
/// This is an entry-level sample, showing a minimal VR sample, 
/// in a simple environment.  Use WASD keys to move around, and cursor keys.
/// It runs with DirectX12.


// Include Windows.h
#include <sdkddkver.h>

// Include the Oculus SDK
#include "OVR_CAPI_D3D.h"

// DTS: Small changes like adding to Matthew5 namespace so Visual Studio class view 
//	looks a bit better. Also, naming of properties to include m if they are member 
//	variables of the struct.

namespace Matthew5
{
	namespace Matthew5GraphicsOculus
	{

		//------------------------------------------------------------
		// ovrSwapTextureSet wrapper class that also maintains the render target views
		// needed for D3D12 rendering.
		struct OculusEyeTexture
		{
			ovrSession               mOvrSession;
			ovrTextureSwapChain      mOvrTextureChain;
			ovrTextureSwapChain      mOvrDepthTextureChain;

			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> mTexRtv;
			std::vector<ID3D12Resource*> mTexResource;

			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> mDepthTexDsv;
			std::vector<ID3D12Resource*>             mDepthTex;

			int mPerEyeSwapChainBufferCount = 0;

			OculusEyeTexture() :
				mOvrSession(nullptr),
				mOvrTextureChain(nullptr),
				mOvrDepthTextureChain(nullptr)
			{
			}

			bool Init(ovrSession session, int sizeW, int sizeH, bool createDepth, int vEyeMsaaRate, DXGI_FORMAT vDepthFormat
				, ID3D12Device & vrID3D12Device, ID3D12CommandQueue & vrID3D12CommandQueue,
				Matthew5DxDescriptorHeapManager & vrRtvDescriptorHeapManager, Matthew5DxDescriptorHeapManager   & vrDsvDescriptorHeapManager)
			{
				mOvrSession = session;

				ovrTextureSwapChainDesc desc = {};
				desc.Type = ovrTexture_2D;
				desc.ArraySize = 1;
				desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
				desc.Width = sizeW;
				desc.Height = sizeH;
				desc.MipLevels = 1;
				desc.SampleCount = vEyeMsaaRate;
				desc.MiscFlags = ovrTextureMisc_DX_Typeless | ovrTextureMisc_AutoGenerateMips;
				desc.StaticImage = ovrFalse;
				desc.BindFlags = ovrTextureBind_DX_RenderTarget;


				ovrResult result = ovr_CreateTextureSwapChainDX(session, &vrID3D12CommandQueue, &desc, &mOvrTextureChain);
				if (!OVR_SUCCESS(result))
					return false;

				//int textureCount = 0;
				ovr_GetTextureSwapChainLength(mOvrSession, mOvrTextureChain, &mPerEyeSwapChainBufferCount);
				mTexRtv.resize(mPerEyeSwapChainBufferCount);
				mTexResource.resize(mPerEyeSwapChainBufferCount);

				for (int i = 0; i < mPerEyeSwapChainBufferCount; ++i)
				{
					result = ovr_GetTextureSwapChainBufferDX(mOvrSession, mOvrTextureChain, i, IID_PPV_ARGS(&mTexResource[i]));
					if (!OVR_SUCCESS(result))
						return false;
					mTexResource[i]->SetName(L"EyeColorRes");

					D3D12_RENDER_TARGET_VIEW_DESC rtvd = {};
					rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					rtvd.ViewDimension = (vEyeMsaaRate > 1) ? D3D12_RTV_DIMENSION_TEXTURE2DMS
						: D3D12_RTV_DIMENSION_TEXTURE2D;
					mTexRtv[i] = vrRtvDescriptorHeapManager.GetNextUsingCounter();
					vrID3D12Device.CreateRenderTargetView(mTexResource[i], &rtvd, mTexRtv[i]);
				}

				if (createDepth)
				{
					ovrTextureSwapChainDesc depthDesc = {};
					depthDesc.Type = ovrTexture_2D;
					depthDesc.ArraySize = 1;
					switch (vDepthFormat)
					{
					case DXGI_FORMAT_D16_UNORM:             depthDesc.Format = OVR_FORMAT_D16_UNORM;            break;
					case DXGI_FORMAT_D24_UNORM_S8_UINT:     depthDesc.Format = OVR_FORMAT_D24_UNORM_S8_UINT;    break;
					case DXGI_FORMAT_D32_FLOAT:             depthDesc.Format = OVR_FORMAT_D32_FLOAT;            break;
					case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:  depthDesc.Format = OVR_FORMAT_D32_FLOAT_S8X24_UINT; break;
					default: throw new std::exception("Matthew5Graphics Oculus error."); break;
					}
					depthDesc.Width = sizeW;
					depthDesc.Height = sizeH;
					depthDesc.MipLevels = 1;
					depthDesc.SampleCount = vEyeMsaaRate;
					depthDesc.MiscFlags = ovrTextureMisc_DX_Typeless;
					depthDesc.StaticImage = ovrFalse;
					depthDesc.BindFlags = ovrTextureBind_DX_DepthStencil;

					result = ovr_CreateTextureSwapChainDX(session, &vrID3D12CommandQueue, &depthDesc, &mOvrDepthTextureChain);
					if (!OVR_SUCCESS(result))
						return false;

					mDepthTex.resize(mPerEyeSwapChainBufferCount);
					mDepthTexDsv.resize(mPerEyeSwapChainBufferCount);
					for (int i = 0; i < mPerEyeSwapChainBufferCount; i++)
					{
						mDepthTexDsv[i] = vrDsvDescriptorHeapManager.GetNextUsingCounter();
						result = ovr_GetTextureSwapChainBufferDX(mOvrSession, mOvrDepthTextureChain, i, IID_PPV_ARGS(&mDepthTex[i]));
						if (!OVR_SUCCESS(result))
							return false;
						mDepthTex[i]->SetName(L"EyeDepthRes");

						D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
						dsvDesc.Format = vDepthFormat;
						dsvDesc.ViewDimension = vEyeMsaaRate > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DMS
							: D3D12_DSV_DIMENSION_TEXTURE2D;
						dsvDesc.Texture2D.MipSlice = 0;
						vrID3D12Device.CreateDepthStencilView(mDepthTex[i], &dsvDesc, mDepthTexDsv[i]);
					}
				}

				return true;
			}

			~OculusEyeTexture()
			{
				if (mOvrTextureChain)
				{
					for (size_t i = 0; i < mTexResource.size(); i++)
					{
						if (mTexResource[i])
							mTexResource[i]->Release();
					}

					ovr_DestroyTextureSwapChain(mOvrSession, mOvrTextureChain);
				}

				if (mOvrDepthTextureChain)
				{
					for (size_t i = 0; i < mDepthTex.size(); i++)
					{
						if (mDepthTex[i])
							mDepthTex[i]->Release();
					}

					ovr_DestroyTextureSwapChain(mOvrSession, mOvrDepthTextureChain);
				}
			}

			D3D12_CPU_DESCRIPTOR_HANDLE GetRtv()
			{
				int index = 0;
				ovr_GetTextureSwapChainCurrentIndex(mOvrSession, mOvrTextureChain, &index);
				return mTexRtv[index];
			}

			D3D12_CPU_DESCRIPTOR_HANDLE GetDsv()
			{
				int index = 0;
				if (mOvrDepthTextureChain)
				{
					ovr_GetTextureSwapChainCurrentIndex(mOvrSession, mOvrDepthTextureChain, &index);
				}
				else
				{
					ovr_GetTextureSwapChainCurrentIndex(mOvrSession, mOvrTextureChain, &index);
				}
				return mDepthTexDsv[index];
			}

			ID3D12Resource* GetD3DColorResource()
			{
				int index = 0;
				ovr_GetTextureSwapChainCurrentIndex(mOvrSession, mOvrTextureChain, &index);
				return mTexResource[index];
			}

			ID3D12Resource* GetD3DDepthResource()
			{
				if (mDepthTex.size() > 0)
				{
					int index = 0;
					ovr_GetTextureSwapChainCurrentIndex(mOvrSession, mOvrTextureChain, &index);
					return mDepthTex[index];
				}
				else
				{
					return nullptr;
				}
			}

			// Commit changes
			void Commit()
			{
				ovr_CommitTextureSwapChain(mOvrSession, mOvrTextureChain);

				if (mOvrDepthTextureChain)
				{
					ovr_CommitTextureSwapChain(mOvrSession, mOvrDepthTextureChain);
				}
			}
		};

	}
}