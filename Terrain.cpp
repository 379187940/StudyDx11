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
			m_VertexBuffer.push_back(pos);
			float3 color;
			color.r = u(e);
			color.g = u(e);
			color.b = u(e);
			m_VertexColorBuffer.push_back(color);
		}
	/*
		3|-----------|4
		 |  \        |
		 |    \      |
		 |      \    |
		 |         \ |
		1|-----------|2
		two triangle:
		(1,3,2) （2,3,4）
	*/
	//construct indexbuffer
	for (int i = 0; i < row-1; i++)
		for (int j = 0; j < col-1; j++)
		{
			int index1 = i*col + j;
			int index2 = i*col + j + 1;
			int index3 = (i + 1)*col + j;
			int index4 = (i + 1)*col + j + 1;
			m_indexBuffer.push_back(int3(index1, index3, index2));
			m_indexBuffer.push_back(int3(index2, index3, index4));
		}
	m_pd3dDevice->CreateInputLayout()
}