#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
#include "MeshLoader11.h"
class CObjModel :
	public CBaseRenderObject,public CCommonRenderData
{
	struct SimpleVertex
	{
		XMFLOAT3 pos;
		//float r, g, b;
	};
public:
	CObjModel(wstring strName);
	virtual ~CObjModel();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams) { return true; }
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool LoadFromObjFile(WCHAR* fileName);
	CMeshLoader11 m_ObjData;
};

