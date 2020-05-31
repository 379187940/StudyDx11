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
	vs_out output;
	output.position = mul(float4(input.position, 1.0f), viewProj);
	output.color = input.color;
	return output;
}

float4 ps_main( vs_output input):SV_Target
{
	return float4(input.color,1.0f);
}