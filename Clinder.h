#pragma once
class CClinder
{
public:
	CClinder(float3 start ,float3 end , uint subdivide , float radius ,float4 color);
	~CClinder();
public:
	void BuildGeometry();
	
private:
	float3 m_start;
	float3 m_end;
	float m_radius;
	float4 m_color;
	uint m_subdivide;
	vector<float3> m_vertexs;
	vector <uint16_t> m_index;
};

