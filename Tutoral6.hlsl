
struct vs_out
{
	float4 postion:SV_POSITION;
	float4 color:COLOR0;
};
cbuffer ConstantBuffer:register(b0)
{
	matrix world				 :packoffset(c0);
	matrix view					 :packoffset(c4);;
	matrix Projection            :packoffset(c8);
	struct LightDataStruct
	{
		float4 lightDir;
		float4 lightColor;
	}lightinfo[2]                :packoffset(c9);
};
vs_out vs_main(float4 in_postion:POSITION , float4 in_normal:NORAML0 )
{
	vs_out temp;
	temp.postion = mul(in_postion,world);
	temp.postion = mul(temp.postion, view);
	temp.postion = mul(temp.postion, Projection);
	temp.color = 0;
	for (int i = 0; i < 2; i++)
	{
		temp.color += saturate(dot(in_normal, lightinfo[i].lightDir) * lightinfo[i].color);
	}
	return temp;
}

float4 ps_main(vs_out vsout) : SV_Target
{
	return vsout.color;
}