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
	for ( int i = 0 ; i < row ; i++ )
		for (int j = 0; j < col ; j++)
		{
			TerrainVer ver;
			ver.pos.x = j * m_tileSize;
			ver.pos.y = i * m_tileSize;
			ver.pos.z = m_tileSize * m_HeightData.pheightData[i*m_HeightData]
		}
}