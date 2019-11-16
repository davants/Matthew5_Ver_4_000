

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbCamera : register(b1)
{
	float4x4 gCameraViewProj;
};

struct VertexIn
{
	float3 PosL    : POSITION;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;
	   	 
    // Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gCameraViewProj);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{

	float4 litColor;

	litColor.x = 0.5f;  // Dark red of some kind 

    return litColor;
}


