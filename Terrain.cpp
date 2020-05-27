#include <random>
#include "unit.h"
#include "Terrain.h"
CTerrain::CTerrain()
{
}


CTerrain::~CTerrain()
{
}
bool CTerrain::Init(char* name)
{
	LoadHeightMap(name);
}
bool CTerrain::LoadHeightMap(char* name)
{
	
}
bool CTerrain::InitGeometry()
{
	int row = m_HeightData.row;
	int col = m_HeightData.col;
	std::default_random_engine e;
	std::uniform_int_distribution<unsigned> u(0, 255); //生成随机颜色
	for ( int i = 0 ; i < row ; i++ )
		for (int j = 0; j < col; j++)
		{
			float3 pos;
			pos.x = j*m_tileSize;
			pos.y = m_HeightData.pheightData[i*col + j];
			pos.z = (row - i - 1)*m_tileSize;
			m_TerrainPos.push_back(pos);
			float3 color;
			color.r = u(e);
			color.g = u(e);
			color.b = u(e);
			m_VertexColor.push_back(color);
		}
}