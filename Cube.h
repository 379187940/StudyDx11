#pragma once
#include "IRenderObject.h"
#include "BaseRenderObject.h"
class CCube :
	public CBaseRenderObject,public CCommonRenderData
{

	struct SimpleVertex
	{
		float3 postion;
		float4 color;
	};
	struct TestConstantBuffer
	{
		float4x4 mWorld;
	};
	struct TestConstantBuffer1
	{
		float4x4 mView;
	};
	struct ConstantBuffer
	{
		float4x4 mProjection;
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

