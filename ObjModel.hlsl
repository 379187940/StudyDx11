
cbuffer globalmatrix
{
	matrix world;
	matrix view;
	matrix proj;
	matrix matviewproj;
};
cbuffer material
{
	float3 ambient;
	float3 diffuse;
	float3 specular;
	int    shininess;
	float  alpha;
	bool   bSpecular;
};
struct vs_input
{
	float4 postion:SV_POSTION;
	float3 normal:NORMAL;
	float2 texcoord; TEXCOORD;
}

struct vs_output
{
	float4 postion:SV_POSITION;
	float3 normal:NORMAL;
	float2 texcoord:TEXCOORD;
};
vs_output vs_main(vs_input input)
{
	vs_output;
	output.postion = mul(input.positon, matviewproj);
	output.normal = normal;
	output.texcoord = input.texcoord;
}
float4 ps_main( vs_output psInput ):SV_Target
{
	float colorResult = 
}