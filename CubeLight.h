#pragma once
#include "IRenderObject.h"
#include "BaseRenderObject.h"
class CCubeLight :
	public CBaseRenderObject,public CCommonRenderData
{

	struct SimpleVertex
	{
		float3 postion;
		float3 Normal;
	};
	struct ConstantBuffer
	{
		float4x4 mWorld;
		float4x4 mView;
		float4x4 mProjection;
		float4 vLightDir[2];
		float4 vLightColor[2];
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

