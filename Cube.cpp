#include "Cube.h"
#include "assert.h"
#include <tchar.h>

CCube::CCube(wstring strName):
	CBaseRenderObject(strName)
{
	m_pConstBuffer = NULL;
}


CCube::~CCube()  
{
	
	if (m_pConstBuffer)
		m_pConstBuffer->Release();
	if (m_pConstBufferTest)
		m_pConstBufferTest->Release();
	if (m_pConstBufferTest1)
		m_pConstBufferTest1->Release();
}

bool CCube::Render(DWORD dwTimes)
{
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->VSSetShader(m_pVertexShader,NULL , 0);
	m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0,1,&m_pVertexBuffer , &stride , &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pConstBufferTest);
	m_pContext->VSSetConstantBuffers(2, 1, &m_pConstBufferTest1);
	m_pContext->VSSetConstantBuffers(1, 1, &m_pConstBuffer);
	//m_pContext->vsset
	m_pContext->DrawIndexed(36, 0, 0);
	return false;
}

bool CCube::Init(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pContext)
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	HRESULT hr;
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};
	
	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0 , sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA subVertexData;
	memset(&subVertexData, 0 , sizeof(D3D11_SUBRESOURCE_DATA));
	subVertexData.pSysMem = vertices;
	hr = m_pd3dDevice->CreateBuffer(&vertexBufferDesc, &subVertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(hr));
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};
	D3D11_BUFFER_DESC indexBufferDesc;
	memset(&indexBufferDesc, 0 , sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = ARRAYSIZE(indices) * 2;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA subIndexData;
	memset(&subIndexData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	subIndexData.pSysMem = indices;
	hr = m_pd3dDevice->CreateBuffer(&indexBufferDesc, &subIndexData, &m_pIndexBuffer);
	assert(SUCCEEDED(hr));


	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA ,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA ,0 },
	};
	ID3D10Blob* pVertexShader = NULL;
	hr = CompileShaderFromFile(_T("cube.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShader);
	assert(SUCCEEDED(hr));
	hr = m_pd3dDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	assert(SUCCEEDED(hr));
	//vertex shader
	hr = m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pVertexShader);
	assert(SUCCEEDED(hr));
	
	ID3D10Blob* pPixelShader = NULL;
	hr = CompileShaderFromFile(_T("cube.hlsl"), NULL, NULL, "ps_main", "ps_4_0", 0, 0, NULL, &pPixelShader);
	assert(SUCCEEDED(hr));
	//pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_pPixelShader);
	assert(SUCCEEDED(hr));

	m_pConstBuffer = CreateBuffer(m_pd3dDevice, sizeof(ConstantBuffer), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0);
	m_pConstBufferTest = CreateBuffer(m_pd3dDevice, sizeof(TestConstantBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0);
	m_pConstBufferTest1 = CreateBuffer(m_pd3dDevice, sizeof(TestConstantBuffer1), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0);
	assert(SUCCEEDED(hr));


	return false;
}
bool CCube::UpdateRenderParams(const RenderParams& renderParams)
{
	ConstantBuffer cb;
	TestConstantBuffer cb1;
	TestConstantBuffer1 cb2;
	cb1.mWorld = XMMatrixTranspose(renderParams.m_worldMatrix);
	cb2.mView = XMMatrixTranspose(renderParams.m_viewMatrix);
	cb.mProjection = XMMatrixTranspose(renderParams.m_projMatrix);
	m_pContext->UpdateSubresource(m_pConstBuffer, 0, NULL, &cb, 0, 0);
	D3D11_MAPPED_SUBRESOURCE mapSubResource;
	m_pContext->Map(m_pConstBufferTest, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubResource);
	memcpy(mapSubResource.pData, &cb1, sizeof(TestConstantBuffer));
	m_pContext->Unmap(m_pConstBufferTest , 0 ) ;
	m_pContext->UpdateSubresource(m_pConstBufferTest1, 0, nullptr, &cb2, 0, 0);
	return true;
}
void CCube::Tick(DWORD dwTimes)
{

}