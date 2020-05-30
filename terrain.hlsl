cbuffer common_
struct vs_input
{
	float3 postion;
	float3 color;
};
float4 vs_main( float4 in_postion:POSITION ):SV_POSITION
{
	return in_postion;
}

float4 ps_main( float4 Pos:SV_POSITION ):SV_Target
{
	return float4(1.0f,1.0f,0.0f,1.0f);
}