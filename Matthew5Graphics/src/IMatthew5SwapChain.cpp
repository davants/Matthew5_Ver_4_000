#include "IMatthew5SwapChain.h"
namespace Matthew5
{
	namespace Matthew5Graphics
	{
		IMatthew5SwapChain::IMatthew5SwapChain(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, M5DxGpuCommand & vrM5DxGpuCommand)
			:
			mrM5DxGpuDeviceNFactory(vrM5DxGpuDeviceNFactory), mrM5DxGpuCommand( vrM5DxGpuCommand)
		{

		}

		IMatthew5SwapChain::~IMatthew5SwapChain()
		{
		}

		void IMatthew5SwapChain::CommandListSetViewport(ID3D12GraphicsCommandList & vrCmdList, ENUMM5Eye vENUMM5Eye)
		{
			vrCmdList.RSSetViewports(1, &mD3D12_VIEWPORTScreen[vENUMM5Eye]);
		}

		void IMatthew5SwapChain::CommandListSetRECTScissor(ID3D12GraphicsCommandList & vrCmdList, ENUMM5Eye vENUMM5Eye)
		{
			vrCmdList.RSSetScissorRects(1, &mD3D12_RECTScissor[vENUMM5Eye]);
		}


		bool IMatthew5SwapChain::Get4XMSAAIndicator(void)
		{
			return m4XMSAAIndicator;
		}

		int IMatthew5SwapChain::GetMSAACount(void)
		{
			if (Get4XMSAAIndicator())
			{
				return 4;
			}
			else
			{
				return 1;
			}
		}

		void IMatthew5SwapChain::SetViewPort(ENUMM5Eye vENUMM5Eye, const D3D12_VIEWPORT & vD3D12_VIEWPORT)
		{
			mD3D12_VIEWPORTScreen[vENUMM5Eye] = vD3D12_VIEWPORT;
		}

		void IMatthew5SwapChain::SetScissor(ENUMM5Eye vENUMM5Eye, const D3D12_RECT & vD3D12_RECT)
		{
			mD3D12_RECTScissor[vENUMM5Eye] = vD3D12_RECT;
		}
	}
}