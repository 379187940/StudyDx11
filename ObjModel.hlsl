
cbuffer globalmatrix
{
	matrix world;
	matrix view;
	matrix proj;
	matrix matviewproj;
};
cbuffer lightinfo
{
	float4 lightPos;
	float4 lightcolor;
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

struct ps_input
{
	float4 postion:SV_POSITION;
	float3 normal:NORMAL;
	float3 color:COLOR;
	float2 texcoord:TEXCOORD;
};
ps_input vs_main(vs_input input)
{
	ps_input output;
	output.postion = mul(input.positon, world);
	output.postion = mul(output.postion, matviewproj);
	output.normal =  mul(normal,(float3x3)world);
	output.texcoord = input.texcoord;

	output.vColor.rgb = g_vLightColor * g_vMaterialAmbient;
	output.vColor.rgb += g_vLightColor * g_vMaterialDiffuse * saturate(dot(vLight, vNormalWorld));

	// If enabled, calculate the specular term
	if (bSpecular)
	{
		float3 vCamera = normalize(vPosWorld.xyz - g_vCameraPosition);
		float3 vReflection = reflect(vLight, vNormalWorld);
		float  fPhoneValue = saturate(dot(vReflection, vCamera));

		output.vColor.rgb += g_vMaterialSpecular * pow(fPhoneValue, g_nMaterialShininess);
	}

	output.vColor.a = g_fMaterialAlpha;
}
float4 ps_main( vs_output psInput ):SV_Target
{
	float colorResult = 
}