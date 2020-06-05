#include "common.hlsli"
struct vs_input
{
	float3 position: POSITION;
	float3 color: COLOR;
};
struct vs_output
{
	float4 position:POSITION;
	float3 color:COLOR;
};
vs_output vs_main( vs_input input)
{
	vs_output output;
	output.position = mul(float4(input.position, 1.0f), viewproj);
	output.color = input.color;
	return output;
}

void ps_main( vs_output input , out float4 color: SV_Target,out float fDepth: SV_Depth)
{
	color = float4(input.color,1.0f);
	fDepth = 1.0f;
}