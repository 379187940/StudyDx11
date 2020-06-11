#include <random>
#include "unit.h"
#include "Terrain.h"
#include "Common.h"
#include "Material.h"
CTerrain::CTerrain(wstring strName) :
	CBaseRenderObject(strName)
{

}


CTerrain::~CTerrain()
{
}
bool CTerrain::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	//LoadHeightMap(name);
	
	return true;
}
bool CTerrain::LoadHeightMap(char* name)
{
	return true;
}
bool CTerrain::InitGeometry()
{
	int row = m_HeightData.row;
	int col = m_HeightData.col;
	std::default_random_engine e;
	std::uniform_int_distribution<unsigned> u(0, 255); //生成随机颜色
	for ( int i = 0 ; i < row ; i++ )
		for (int j = 0; j < col; j++)
		{
			float3 pos;
			pos.x = j*m_tileSize;
			pos.y = m_HeightData.pheightData[i*col + j];
			pos.z = (row - i - 1)*m_tileSize;
			m_VertexBuffer.push_back(pos);
			float3 color;
			color.r = float(u(e))/255;
			color.g = float(u(e)) / 255;
			color.b = float(u(e)) / 255;
			m_VertexColorBuffer.push_back(color);
		}
	/*
		3|-----------|4
		 |  \        |
		 |    \      |
		 |      \    |
		 |         \ |
		1|-----------|2
		two triangle:
		(1,3,2) （2,3,4）
	*/
	//construct indexbuffer
	for (int i = 0; i < row-1; i++)
		for (int j = 0; j < col-1; j++)
		{
			int index1 = i*col + j;
			int index2 = i*col + j + 1;
			int index3 = (i + 1)*col + j;
			int index4 = (i + 1)*col + j + 1;
			m_indexBuffer.push_back(index1);
			m_indexBuffer.push_back(index3);
			m_indexBuffer.push_back(index2);
			m_indexBuffer.push_back(index2);
			m_indexBuffer.push_back(index3);
			m_indexBuffer.push_back(index4);
		}
	m_pCameraAttBuffer = CreateBuffer(m_pd3dDevice, sizeof(CameraAtrribute), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pCameraAttBuffer);
	m_pVertexBuffer = CreateBuffer(m_pd3dDevice, m_VertexBuffer.size() * sizeof(float3), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pVertexBuffer);
	m_pVertexColorBuffer = CreateBuffer(m_pd3dDevice, m_VertexColorBuffer.size() * sizeof(float3), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pVertexColorBuffer);
	m_pIndexBuffer = CreateBuffer(m_pd3dDevice, m_indexBuffer.size() * sizeof(int), D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pIndexBuffer);

	ID3D10Blob* pVertexShaderBlob = NULL;
	assert(SUCCEEDED(CompileShaderFromFile("terrain.hlsl", NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShaderBlob)));
	vector<D3D11_INPUT_ELEMENT_DESC> allDesc;
	allDesc.push_back({"POSITION",0,DXGI_FORMAT_R8G8B8A8_UINT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0});
	allDesc.push_back({"COLOR",0,DXGI_FORMAT_R8G8B8A8_UINT,1,24,D3D11_INPUT_PER_VERTEX_DATA,0 });
	m_pLayoutInput = CreateInputLayout(m_pd3dDevice, allDesc, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize());
	m_pd3dDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);
	pVertexShaderBlob->Release();

	ID3D10Blob* pPixelShaderBlob = NULL;
	assert(SUCCEEDED(CompileShaderFromFile("terrain.hlsl", NULL, NULL, "ps_main", "ps_4_0", 0, 0, NULL, &pPixelShaderBlob)));
	m_pd3dDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
	pPixelShaderBlob->Release();
 
	return true;
}	

void CTerrain::Tick(DWORD dwTimes)
{
}

bool CTerrain::Render(DWORD dwTimes)
{
	UINT stride = sizeof(float3);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pContext->IASetVertexBuffers(1, 1, &m_pVertexColorBuffer, &stride, &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT,0);
	m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pCameraAttBuffer);
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->DrawIndexed(m_indexBuffer.size(), 0, 0);
	return false;
}
bool CTerrain::UpdateRenderParams(const RenderParams& renderParams)
{
	
	CameraAtrribute cameraAttribute;
	cameraAttribute.worldviewproj = renderParams.m_worldMatrix * renderParams.m_viewMatrix * renderParams.m_projMatrix;
	UpdateBufferData(m_pContext, m_pCameraAttBuffer, &cameraAttribute, sizeof(cameraAttribute));
	UpdateBufferData(m_pContext, m_pVertexBuffer, m_VertexBuffer.data(), m_VertexBuffer.size() * sizeof(float3));
	UpdateBufferData(m_pContext, m_pVertexColorBuffer, m_VertexColorBuffer.data(), m_VertexBuffer.size() * sizeof(float3));
	UpdateBufferData(m_pContext, m_pIndexBuffer, m_indexBuffer.data(), m_indexBuffer.size() * sizeof(int3));
	return true;
}