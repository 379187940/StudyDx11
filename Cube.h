#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
class CCube :
	public IRenderObject,public CCommonRenderData
{

	struct SimpleVertex
	{
		XMFLOAT3 postion;
		XMFLOAT4 color;
	};
	struct ConstantBuffer
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
	};
public:
	CCube();
	virtual ~CCube();
	virtual bool Render(DWORD dwTimes);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
private:
	ID3D11Buffer* m_pConstBuffer;

};

