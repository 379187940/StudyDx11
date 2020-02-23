
cbuffer ConstantBuffer : register(b0)
{
	float g_fQ;                         // far / (far - near)
	float g_fQTimesZNear;               // Q * near
}

struct VS_RenderQuadInput
{
	float3 f3Position : POSITION;
	float2 f2TexCoord : TEXTURE0;
};
struct PS_RenderQuadInput
{
	float4 f4Position : SV_Position;
	float2 f2TexCoord : TEXTURE0;
};

struct PS_RenderOutput_10_0
{
	float4 f4Color	: SV_Target0;
	float4 f4Normal	: SV_Target1;
};
//SamplerState g_SamplePoint
//{
//	Filter = MIN_MAG_MIP_POINT;
//	AddressU = CLAMP;
//	AddressV = CLAMP;
//};
Texture2D g_txDepth: register(t0);
SamplerState samLinear : register(s0);
PS_RenderQuadInput VS_RenderQuad(VS_RenderQuadInput I)
{
	PS_RenderQuadInput O;

	O.f4Position.x = I.f3Position.x;
	O.f4Position.y = I.f3Position.y;
	O.f4Position.z = 0.01f;
	O.f4Position.w = 1.0f;

	O.f2TexCoord = I.f2TexCoord;

	return O;
}

float4 PS_RenderCameraZ(PS_RenderQuadInput I) : SV_Target0
{
	float fDepth;
	float4 f4CameraZ;

	fDepth = g_txDepth.SampleLevel(samLinear, I.f2TexCoord, 0).x;
	//f4CameraZ.x = -g_fQTimesZNear / (fDepth - g_fQ);

	return /*f4CameraZ.xxxx * 0.005f*/ float4( fDepth , fDepth , fDepth , 1.0f );
}



