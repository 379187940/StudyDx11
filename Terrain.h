#pragma once
#include "BaseRenderObject.h"
struct HeightData
{
	vector<int> heightData;
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

	struct ConstantBuffer
	{
		float4x4 mWorld;
		float4x4 mView;
		float4x4 mProjection;
	};
public:
	bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool LoadHeightMap(char* heightImageFilename);
	bool InitGeometry();
	bool Release();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
private:
	HeightData m_HeightData;
	std::vector<float3> m_VertexBuffer;
	std::vector<float2> m_TerrainUv;
	std::vector<float3> m_VertexColorBuffer;
	std::vector<int>    m_indexBuffer;
	float m_tileSize = 2.0f;
	float m_heightScale = 0.20f;
	CComPtr<ID3D11Buffer> m_pCameraAttBuffer;
	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pVertexColorBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;
};

