#pragma once
#include "IRenderObject.h"
#include "BaseRenderObject.h"
#include "solidnotexture.h"
class CClinder:
	public CBaseRenderObject, public CCommonRenderData
{
public:
	CClinder(wstring strName);
	~CClinder();
public:
	void UpdateProperty(float3 start, float3 end, uint subdivide, float radius, float4 color);
	void BuildGeometry();
	virtual bool Render(DWORD dwTimes);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
	virtual void Tick(DWORD dwTimes);
private:
	float3 m_start;
	float3 m_end;
	float m_radius;
	float4 m_color;
	uint m_subdivide;
	vector<float3> m_vertexs;
	vector <uint16_t> m_index;
	solidnotexture* m_pMaterailSolidColor;
	float4x4 m_ViewProj;
};

