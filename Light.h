#pragma once
class CDirLight
{
public:
	CDirLight(float3 dir , float4 color );
	~CDirLight();
	float3 GetDir() const{ return m_dir; }
	float4 GetColor() const { return m_color; }
private:
	float3 m_dir;
	float4 m_color;
};

