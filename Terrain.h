#pragma once
#include "BaseRenderObject.h"
struct HeightData
{
	float* pheightData = NULL;
	UINT32 row = 0;
	UINT32 col =  0;
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
	CTerrain(wstring strName);
	~CTerrain();
public:
	bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool LoadHeightMap(char* name);
	bool InitGeometry();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams) { return true; }
private:
	HeightData m_HeightData;
	std::vector<float3> m_VertexBuffer;
	std::vector<float2> m_TerrainUv;
	std::vector<float3> m_VertexColorBuffer;
	std::vector<int3>    m_indexBuffer;
	float m_tileSize = 10.0f;
};

