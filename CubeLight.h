#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
class CCubeLight :
	public CBaseRenderObject,public CCommonRenderData
{

	struct SimpleVertex
	{
		XMFLOAT3 postion;
		XMFLOAT3 Normal;
	};
	struct ConstantBuffer
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
		XMFLOAT4 vLightDir[2];
		XMFLOAT4 vLightColor[2];
	};
	
public:
	CCubeLight(wstring strName);
	virtual ~CCubeLight();
	virtual bool Render(DWORD dwTimes);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
	virtual void Tick(DWORD dwTimes);
private:
	ID3D11Buffer* m_pConstBuffer;

};

