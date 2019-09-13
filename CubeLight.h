#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
class CCubeLight :
	public IRenderObject,public CCommonRenderData
{

	struct SimpleVertex
	{
		XMFLOAT3 postion;
		XMFLOAT3 Normal;
	};
	struct ConstantBufferMatrix
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
	};
	struct ConstantBufferLightDir
	{
		XMFLOAT3 light1Dir;
		XMFLOAT3 light1Color;
		XMFLOAT3 light2Dir;
		XMFLOAT3 light2Color;
	};
public:
	CCubeLight();
	virtual ~CCubeLight();
	virtual bool Render(DWORD dwTimes);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
	virtual void Tick(DWORD dwTimes);
private:
	ID3D11Buffer* m_pConstBuffer;

};

