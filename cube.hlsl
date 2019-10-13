
struct vs_out
{
	float4 postion:SV_POSITION;
	float4 color:COLOR0;
};
cbuffer ConstantBuffer:register(b1)
{
	matrix world;
	matrix view;
	matrix Projection;
};
vs_out vs_main(float4 in_postion:POSITION , float4 in_color:COLOR )
{
	vs_out temp;
	temp.postion = mul(in_postion,world);
	temp.postion = mul(temp.postion, view);
	temp.postion = mul(temp.postion, Projection);
	temp.color = in_color;
	return temp;
}

float4 ps_main(vs_out vsout) : SV_Target
{
	return vsout.color;
}