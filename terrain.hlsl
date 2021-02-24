#include "common.hlsli"

Texture2D basecolor;
Texture2D colorTexture1;
Texture2D colorTexture2;
Texture2D colorTexture3;
Texture2D mask;
SamplerState lineSam;
struct vs_input
{
	float3 position: POSITION;
	float2 uv:TEXCOORD0;
};
struct vs_output
{
	float4 position:SV_POSITION;
	float2 uv:TEXCOORD0;
};
vs_output vs_main( vs_input input)
{
	vs_output output;
	output.position = mul(float4(input.position,1.0), viewproj);
	//output.position = mul(output.position, View);
	//output.position = mul(output.position, Projection);
	output.uv = input.uv;
	return output;
}

void ps_main( vs_output input , out float4 color: SV_Target)
{
	float3 maskColor = mask.Sample(lineSam, input.uv);
	color = basecolor.Sample(lineSam,input.uv);
	color = lerp(color, colorTexture1.Sample(lineSam, input.uv), maskColor.r);
	color = lerp(color, colorTexture2.Sample(lineSam, input.uv), maskColor.g);
	color = lerp(color, colorTexture3.Sample(lineSam, input.uv), maskColor.b);
}