#include "Cube.h"
#include "assert.h"
#include <tchar.h>

CCube::CCube()
{
}


CCube::~CCube()
{
}

bool CCube::Render(DWORD dwTimes)
{

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
		{ "POSTION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA ,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA ,0 },
	};
	ID3D10Blob* pVertexShader = NULL;
	D3DX11CompileFromFile(_T("Tutoral4.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShader, NULL, &hr);
	assert(SUCCEEDED(hr));
	hr = m_pd3dDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	assert(SUCCEEDED(hr));
	//pixel shader
	hr = m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pPixelShader);
	assert(SUCCEEDED(hr));

	return false;
}
