#include "unit.h"
#include "Clinder.h"



CClinder::CClinder(float3 start, float3 end, uint subdivide, float radius, float4 color)
{
	m_start = start;
	m_end = end;
	m_radius = radius;
	m_color = color;
	m_subdivide = subdivide;
	BuildGeometry();
}


CClinder::~CClinder()
{

}
void CClinder::BuildGeometry()
{
	m_vertexs.clear();
	m_index.clear();
	float3 dir = m_end - m_start;
	dir = normalize(dir);
	float3 up(dir.z, dir.x, dir.y);
	float3 right = cross(up, dir);
	up = cross(dir, right);
	float subAngle = 2 * PI / m_subdivide;
	//顶点
	for (int i = 0; i < m_subdivide; i++)
	{
		float3 pos1 = m_start + (up*cos(subAngle*i) + right*sin(subAngle*i)) * m_radius;
		float3 pos2 = m_end + (up*cos(subAngle*i) + right*sin(subAngle*i)) * m_radius;
		m_vertexs.push_back(pos1);
		m_vertexs.push_back(pos2);
	}
	//侧面索引
	for (int i = 0; i < m_subdivide; i++)
	{
		m_index.push_back(i*2);
		m_index.push_back(i*2+1);
		m_index.push_back((i*2+2)%(2*m_subdivide-1) );
		m_index.push_back((i * 2 + 2) % (2 * m_subdivide - 1));
		m_index.push_back(i*2 + 1);
		m_index.push_back((i*2 + 3) % (2 * m_subdivide - 1));
	}
	m_vertexs.push_back(m_start);
	uint16_t startIndex = m_vertexs.size() - 1;
	m_vertexs.push_back(m_end);
	uint16_t endIndex = m_vertexs.size() - 1;
	//地面索引   上底 下底
	for (int i = 0; i < m_subdivide; i++)
	{
		m_index.push_back(startIndex);
		m_index.push_back(i*2);
		m_index.push_back((i * 2 +2)% (2 * m_subdivide - 1));
		m_index.push_back(endIndex);
		m_index.push_back((i * 2 + 3) % (2 * m_subdivide - 1));
		m_index.push_back(i * 2+1);
	}
}
