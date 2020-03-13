#include "unit.h"
#include "CubeLight.h"
#include "assert.h"
#include <tchar.h>

CCubeLight ::CCubeLight (wstring strName):
	CBaseRenderObject(strName)
{
	m_pConstBuffer = NULL;
}


CCubeLight ::~CCubeLight ()
{
	if (m_pConstBuffer)
		m_pConstBuffer->Release();
}

bool CCubeLight ::Render(DWORD dwTimes)
{
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->VSSetShader(m_pVertexShader,NULL , 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pContext->PSSetConstantBuffers(0, 1, &m_pConstBuffer);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0,1,&m_pVertexBuffer , &stride , &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	
	
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->DrawIndexed(36, 0, 0);
	return false;
}

bool CCubeLight ::Init(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pContext)
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	HRESULT hr;
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0 , sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	//vertexBufferDesc.MiscFlags = 0;
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

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};
	D3D11_BUFFER_DESC indexBufferDesc;
	memset(&indexBufferDesc, 0 , sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = 36 * sizeof(WORD);
	indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA subIndexData;
	memset(&subIndexData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	subIndexData.pSysMem = indices;
	hr = m_pd3dDevice->CreateBuffer(&indexBufferDesc, &subIndexData, &m_pIndexBuffer);
	assert(SUCCEEDED(hr));


	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA ,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA ,0 },
	};
	ID3D10Blob* pVertexShader = NULL;
	ID3D10Blob* pErrorMsg = NULL;
	//pErrorMsg->GetBufferPointer();
	hr = CompileShaderFromFile(_T("cubelight.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShader);
	if (pErrorMsg)
	{
		OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
		pErrorMsg->Release();
	}
		
	assert(SUCCEEDED(hr));

	hr = m_pd3dDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	assert(SUCCEEDED(hr));
	//vertex shader
	hr = m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pVertexShader);
	assert(SUCCEEDED(hr));
	
	ID3D10Blob* pPixelShader = NULL;
	hr = CompileShaderFromFile(_T("cubelight.hlsl"), NULL, NULL, "ps_main", "ps_4_0", 0, 0, NULL, &pPixelShader);
	assert(SUCCEEDED(hr));
	if (pErrorMsg)
		pErrorMsg->Release();
	//pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_pPixelShader);
	assert(SUCCEEDED(hr));
	m_pConstBuffer = CreateBuffer(m_pd3dDevice, sizeof(ConstantBuffer), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0);
	return false;
}
bool CCubeLight ::UpdateRenderParams(const RenderParams& renderParams)
{
	static DWORD step = 0;
	step = (step + 1) % 3600;
	ConstantBuffer cb;
	XMMATRIX temp = XMMatrixTranslation(-2.5f, 0.0f, 0.0f) *XMMatrixScaling(0.5f, 0.5f, 0.5f);
	cb.mWorld = XMMatrixTranspose(renderParams.m_worldMatrix*temp);
	cb.mView = XMMatrixTranspose( renderParams.m_viewMatrix);
	cb.mProjection = XMMatrixTranspose(renderParams.m_projMatrix);
	//cb.mProjection = renderParams.m_projMatrix;
	XMFLOAT4 vLightDirs[2] =
	{
		XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
	};
	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)
	};
	XMMATRIX mRotate = XMMatrixRotationY(step/3600.f*2*XM_PI);
	XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat4(&vLightDirs[1], vLightDir);
	cb.vLightDir[0] = vLightDirs[0];
	cb.vLightDir[1] = vLightDirs[1];

	cb.vLightColor[0]  = vLightColors[0];
	cb.vLightColor[1] = vLightColors[1];
	m_pContext->UpdateSubresource(m_pConstBuffer, 0, NULL, &cb, 0, 0);
	return true;
}
void CCubeLight ::Tick(DWORD dwTimes)
{

}