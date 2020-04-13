#pragma once
class CDirLight
{
public:
	CDirLight(float3 dir , float3 color );
	~CDirLight();
	float3 GetDir() const{ return m_dir; }
	float3 GetColor() const { return m_color; }
private:
	float3 m_dir;
	float3 m_color;
};

