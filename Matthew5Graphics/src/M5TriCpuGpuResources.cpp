#include "M5TriCpuGpuResources.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		D3D12_VERTEX_BUFFER_VIEW M5TriCpuGpuResources::GetGpuVertexBufferView() const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = mpM5TriGpuResources->mpVertexArray->GetID3D12Resource().GetGPUVirtualAddress();
			vbv.StrideInBytes = sizeof(M5TriVertex);
			vbv.SizeInBytes = (UINT)mpM5TriGpuResources->mpVertexArray->GetBufferSize();

			return vbv;
		}
		D3D12_INDEX_BUFFER_VIEW M5TriCpuGpuResources::GetGpuIndexBufferView() const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = mpM5TriGpuResources->mpIndexArray->GetID3D12Resource().GetGPUVirtualAddress();
			ibv.Format = DXGI_FORMAT_R16_UINT;
			ibv.SizeInBytes = (UINT)mpM5TriGpuResources->mpIndexArray->GetBufferSize();

			return ibv;
		}
	}
}