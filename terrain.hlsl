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
	output.Pos = mul(float4(input.Pos,1.0), World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.color = input.color;
	return output;
}

void ps_main( vs_output input , out float4 color: SV_Target)
{
	color = float4(input.color,1.0f);
}