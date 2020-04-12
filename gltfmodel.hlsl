//
//cbuffer globalmatrix
//{
//	matrix world;
//	matrix view;
//	matrix proj;
//	matrix matviewproj;
//	float3 camerapos;
//};
//cbuffer lightinfo
//{
//	float3 lightDir;
//	float3 lightcolor;
//};
//cbuffer material
//{
//	float3 ambient;
//	float3 diffuse;
//	float3 specular;
//	int    shininess;
//	float  alpha;
//	bool   bSpecular;
//};
//struct vs_input
//{
//	float4 postion:POSITION;
//	float3 normal:NORMAL;
//	float2 texcoord:TEXCOORD0;
//	float2 texcoord1:TEXCOORD1;
//	float4 joint0:NORMAL1;
//	float4 weight0:NORMAL2;
//};
//
//struct ps_input
//{
//	float4 postion:POSITION;
//	float3 normal:NORMAL;
//	float4 color:COLOR;
//	float2 texcoord:TEXCOORD;
//};
//ps_input vs_main(vs_input input)
//{
//	ps_input output;
//	float4 vPosWorld = mul(input.postion, world);
//	output.postion = mul(vPosWorld, matviewproj);
//	output.normal =  mul(input.normal,(float3x3)world);
//	output.texcoord = input.texcoord;
//
//	output.color.rgb = lightcolor * ambient;
//	output.color.rgb += lightcolor * diffuse * saturate(dot(lightDir, output.normal));
//
//	// If enabled, calculate the specular term
//	
//	float3 vCamera = normalize(vPosWorld.xyz - camerapos);
//	float3 vReflection = reflect(lightDir, output.normal);
//	float  fPhoneValue = saturate(dot(vReflection, vCamera));
//
//	output.color.rgb += specular * pow(fPhoneValue, shininess);
//	
//
//	output.color.a = alpha;
//	return output;
//}
//texture2D diffuseTex;
//SamplerState samLiner;
//float4 ps_main(ps_input psInput ):SV_Target
//{
//	float4 colorResult = psInput.color;
//    colorResult.rgb *= diffuseTex.Sample(samLiner , psInput.texcoord , 0);
//	return colorResult;
//} 
#ifdef USE_TEX
texture2D diffuseTex;
SamplerState samLinear : register(s0);
#endif
struct vs_input
{
	float3 postion:POSITION;
	float3 normal:NORMAL;
	float2 texcoord:TEXCOORD0;
	float2 texcoord1:TEXCOORD1;
	float4 joint0:NORMAL1;
	float4 weight0:NORMAL2;
};
struct vs_out
{
	float4 postion:SV_POSITION;
	float4 color:COLOR0;
	float2 texcoord:TEXCOORD0;
};
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};
cbuffer lightinfo
{
	float3 lightDir;
	float4 lightcolor;
};
cbuffer material
{
	float3 ambient;
	float3 diffuse;
	float3 specular;
	int    shininess;
};
vs_out vs_main(vs_input input)
{
	vs_out temp;
	
	temp.postion = mul(float4(input.postion,1.0f), World);
	temp.postion = mul(temp.postion, View);
	temp.postion = mul(temp.postion, Projection);
	temp.texcoord = input.texcoord;
	return temp;
}

float4 ps_main(vs_out vsout) : SV_Target
{
#ifdef USE_TEX
	return  diffuseTex.Sample(samLinear, vsout.texcoord);
#else
	return  float4(1,0,0,1);
#endif
}