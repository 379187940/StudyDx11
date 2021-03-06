#include "unit.h"
#include "Triangle.h"
#include <assert.h>
#include<tchar.h>
CTriangle::CTriangle(wstring strName):
	CBaseRenderObject(strName)
{

}


CTriangle::~CTriangle()
{
	
}

void CTriangle::Tick(DWORD dwTimes)
{
}
bool CTriangle::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext	 = pContext;
	ID3DBlob* pVertexShader = NULL;
	HRESULT hr;
	hr = CompileShaderFromFile(_T("triangle.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, &pVertexShader);
	if (FAILED(hr))
		assert(false);
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	m_pLayoutInput = NULL;
	m_pd3dDevice->CreateInputLayout(desc, ARRAYSIZE(desc), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	assert(m_pLayoutInput);
	SimpleVertex vertices[] = {
		float3(0.0f , 0.5f , 0.999f) ,
		float3(0.5f , -0.5f , 0.999f) ,
		float3(-0.5f,-0.5f,0.999f)
	};
	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	memset(&initData, 0, sizeof(initData));
	initData.pSysMem = vertices;
	if (FAILED(m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer)))
	{
		assert(false);
		return false;
	}
	if (FAILED(m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		pVertexShader->Release();
		assert(false); return false;
	}
	ID3DBlob* pPixelShader = NULL;

	hr = CompileShaderFromFile(_T("triangle.hlsl"), NULL, NULL, "ps_main", "ps_4_0", 0, 0, &pPixelShader);
	if (FAILED(m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		assert(false);
		return false;
	}
	return true;
}
bool CTriangle::Render(DWORD dwTimes)
{
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride , &offset);
	m_pContext->Draw(3, 0);
	return false;
}
