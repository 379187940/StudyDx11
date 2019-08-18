#include "Cube.h"
#include "assert.h"


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
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSTION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA ,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA ,0},
	};
	ID3D10Blob* pVertexShader = NULL;
	D3DX11CompileFromFile(_T("test.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShader, NULL, &hr);
	if (FAILED(hr))
		assert(0);
	m_pd3dDevice->CreateInputLayout( inputDesc , ARRAYSIZE(inputDesc) ,  pVertexShader->GetBufferPointer() , )
	return false;
}
