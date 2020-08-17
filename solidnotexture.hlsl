cbuffer CameraMatrix
{
	matrix viewproj;
};
cbuffer PixelSolidColor
{
	float4 solidcolor;
};

struct vs_input
{
	float3 postion:POSITION;
};

float4 vs_main(vs_input input) :SV_POSITION
{
	return mul(float4(input.positon, 1.0f), viewproj));
}
float4 ps_main(float4 pos:SV_POSITION) : SV_TARGET
{
	return solidcolor;
}