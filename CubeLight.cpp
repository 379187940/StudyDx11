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
	m_pContext->IASetVertexBuffers(0,1,&m_pVertexBuffer.p , &stride , &offset);
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
		{ float3(-1.0f, 1.0f, -1.0f), float3(0.0f, 1.0f, 0.0f) },
		{ float3(1.0f, 1.0f, -1.0f), float3(0.0f, 1.0f, 0.0f) },
		{ float3(1.0f, 1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f) },
		{ float3(-1.0f, 1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f) },

		{ float3(-1.0f, -1.0f, -1.0f), float3(0.0f, -1.0f, 0.0f) },
		{ float3(1.0f, -1.0f, -1.0f), float3(0.0f, -1.0f, 0.0f) },
		{ float3(1.0f, -1.0f, 1.0f), float3(0.0f, -1.0f, 0.0f) },
		{ float3(-1.0f, -1.0f, 1.0f), float3(0.0f, -1.0f, 0.0f) },

		{ float3(-1.0f, -1.0f, 1.0f), float3(-1.0f, 0.0f, 0.0f) },
		{ float3(-1.0f, -1.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f) },
		{ float3(-1.0f, 1.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f) },
		{ float3(-1.0f, 1.0f, 1.0f), float3(-1.0f, 0.0f, 0.0f) },

		{ float3(1.0f, -1.0f, 1.0f), float3(1.0f, 0.0f, 0.0f) },
		{ float3(1.0f, -1.0f, -1.0f), float3(1.0f, 0.0f, 0.0f) },
		{ float3(1.0f, 1.0f, -1.0f), float3(1.0f, 0.0f, 0.0f) },
		{ float3(1.0f, 1.0f, 1.0f), float3(1.0f, 0.0f, 0.0f) },

		{ float3(-1.0f, -1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f) },
		{ float3(1.0f, -1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f) },
		{ float3(1.0f, 1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f) },
		{ float3(-1.0f, 1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f) },

		{ float3(-1.0f, -1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f) },
		{ float3(1.0f, -1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f) },
		{ float3(1.0f, 1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f) },
		{ float3(-1.0f, 1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f) },
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
	ID3DBlob* pVertexShader = NULL;
	//pErrorMsg->GetBufferPointer();
	hr = CompileShaderFromFile(_T("cubelight.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, &pVertexShader);
		
	assert(SUCCEEDED(hr));

	hr = m_pd3dDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	assert(SUCCEEDED(hr));
	//vertex shader
	hr = m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pVertexShader);
	assert(SUCCEEDED(hr));
	
	ID3DBlob* pPixelShader = NULL;
	hr = CompileShaderFromFile(_T("cubelight.hlsl"), NULL, NULL, "ps_main", "ps_4_0", 0, 0, &pPixelShader);
	assert(SUCCEEDED(hr));
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
	float4x4 temp = Matrix4x4<float>::Translation(-2.5f, 0.0f, 0.0f) *Matrix4x4<float>::Scale(0.5f, 0.5f, 0.5f);
	cb.mWorld = renderParams.m_worldMatrix*temp.Transpose();
	cb.mView = renderParams.m_viewMatrix.Transpose();
	cb.mProjection = renderParams.m_projMatrix.Transpose();
	//cb.mProjection = renderParams.m_projMatrix;
	float4 vLightDirs[2] =
	{
		float4(-0.577f, 0.577f, -0.577f, 1.0f),
		float4(0.0f, 0.0f, -1.0f, 1.0f),
	};
	float4 vLightColors[2] =
	{
		float4(0.5f, 0.5f, 0.5f, 1.0f),
		float4(0.5f, 0.0f, 0.0f, 1.0f)
	};
	float4x4 mRotate = Matrix4x4<float>::RotationY(step/3600.f*2*XM_PI);
	float4 vLightDir = float4(vLightDirs[1],0.0);
	
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