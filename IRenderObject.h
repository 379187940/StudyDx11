#pragma once
#include "IObject.h"
struct RenderParams
{
	float4x4 m_worldMatrix;
	float4x4 m_viewMatrix;
	float4x4 m_projMatrix;
	
	RenderParams()
	{
		
	}
};
class IRenderObject :
	public IObject
{
public:
	//virtual IRenderObject() = 0;
	virtual bool Render(DWORD dwTimes) = 0;
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) = 0 ;
	virtual bool UpdateRenderParams(const RenderParams& renderParams) = 0;
	virtual bool IsVisible() = 0;
	
};
class CCommonRenderData
{
protected:
	
	CCommonRenderData()
	{
		m_pLayoutInput = NULL ;
		m_pVertexBuffer =  NULL;
		m_pVertexShader = NULL ;
		m_pPixelShader = NULL ;
		m_pIndexBuffer = NULL;
	}
	virtual ~CCommonRenderData()
	{
		if (m_pLayoutInput)
			m_pLayoutInput->Release();
		if (m_pVertexBuffer)
			m_pVertexBuffer->Release();
		if (m_pVertexShader)
			m_pVertexShader->Release();
		if (m_pPixelShader)
			m_pPixelShader->Release();
		if (m_pIndexBuffer)
			m_pIndexBuffer->Release();
	}
protected:
	ID3D11DeviceContext* m_pContext;
	ID3D11Device*        m_pd3dDevice;
	ID3D11InputLayout*	 m_pLayoutInput;
	ID3D11Buffer*        m_pVertexBuffer;
	ID3D11Buffer*        m_pIndexBuffer;
	ID3D11VertexShader*  m_pVertexShader;
	ID3D11PixelShader*   m_pPixelShader;
	RenderParams         m_renderParams;
};

