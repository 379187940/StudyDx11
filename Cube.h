#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
class CCube :
	public CBaseRenderObject,public CCommonRenderData
{

	struct SimpleVertex
	{
		XMFLOAT3 postion;
		XMFLOAT4 color;
	};
	struct TestConstantBuffer
	{
		XMMATRIX mWorld;
	};
	struct TestConstantBuffer1
	{
		XMMATRIX mView;
	};
	struct ConstantBuffer
	{
		XMMATRIX mProjection;
	};
public:
	CCube(wstring strName);
	virtual ~CCube();
	virtual bool Render(DWORD dwTimes);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
	virtual void Tick(DWORD dwTimes);
private:
	ID3D11Buffer* m_pConstBuffer;
	//没有必要写两个constbuffer，为了学习ConstBuffer 把矩阵拆分为两部分
	ID3D11Buffer* m_pConstBufferTest;
	ID3D11Buffer* m_pConstBufferTest1;
};

