#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
class CTriangle :
	public CBaseRenderObject,public CCommonRenderData
{
	struct SimpleVertex
	{
		XMFLOAT3 pos;
		//float r, g, b;
	};
public:
	CTriangle(wstring strName);
	virtual ~CTriangle();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams) { return true; }
	bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

};

