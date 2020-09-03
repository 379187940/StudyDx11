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
	
	}
protected:
	ID3D11DeviceContext* m_pContext;
	ID3D11Device*        m_pd3dDevice;
	CComPtr<ID3D11InputLayout>	 m_pLayoutInput;
	CComPtr<ID3D11Buffer>        m_pVertexBuffer;
	CComPtr<ID3D11Buffer>        m_pIndexBuffer;
	CComPtr<ID3D11VertexShader>  m_pVertexShader;
	CComPtr<ID3D11PixelShader>   m_pPixelShader;
	RenderParams         m_renderParams;
};

