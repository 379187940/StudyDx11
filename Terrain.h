#pragma once
#include "BaseRenderObject.h"
struct HeightData
{
	float* pheightData = NULL;
	float height = 0;
	float width =  0;
};
struct TerrainVer
{
	float3 pos;
	float3 normal;
	float2 maskUv;
};
class CTerrain :
	public CBaseRenderObject, public CCommonRenderData
{
public:
	CTerrain();
	~CTerrain();
public:
	bool Init(char* name);
	bool LoadHeightMap(char* name);
	bool InitGeometry();
private:
	HeightData m_HeightData;
	std::vector<TerrainVer> m_verSrc;
};

