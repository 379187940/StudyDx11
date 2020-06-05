#include <random>
#include "unit.h"
#include "Terrain.h"
#include "Common.h"
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
	ID3D10Blob* pPixelShaderBlob = NULL;
	assert(SUCCEEDED(CompileShaderFromFile("terrain.hlsl", NULL, NULL, "ps_main", "ps_4_0", 0, 0, NULL, &pPixelShaderBlob)));
	m_pd3dDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
	pPixelShaderBlob->Release();
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
			color.r = u(e);
			color.g = u(e);
			color.b = u(e);
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
			m_indexBuffer.push_back(int3(index1, index3, index2));
			m_indexBuffer.push_back(int3(index2, index3, index4));
		}
	
	m_pVertexBuffer = CreateBuffer(m_pd3dDevice, m_VertexBuffer.size() * 24, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0);
	assert(m_pVertexBuffer);
	m_pIndexBuffer  = CreateBuffer(m_pd3dDevice, m_indexBuffer.size() *  24, D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0);
	assert(m_pIndexBuffer);
	ID3D10Blob* pVertexShaderBlob = NULL;
	assert(SUCCEEDED(CompileShaderFromFile("terrain.hlsl", NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShaderBlob)));
	vector<D3D11_INPUT_ELEMENT_DESC> allDesc;
	allDesc.push_back({"POSITION",0,DXGI_FORMAT_R8G8B8A8_UINT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0});
	allDesc.push_back({"COLOR",0,DXGI_FORMAT_R8G8B8A8_UINT,1,24,D3D11_INPUT_PER_VERTEX_DATA,0 });
	CreateInputLayout(m_pd3dDevice, allDesc, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize());
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
	
	return false;
}