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
	for ( int i = 0 ; i < m_HeightData.row ; i++ )
		for (int j = 0; j < m_HeightData.col; j++)
		{
			TerrainVer ver;
			ver.pos.x = 
		}
}