#include "unit.h"
#include "gltf.h"
#include <assert.h>
#include<tchar.h>
#include "tiny_gltf.h"
#include "GLTFLoader.hpp"
using namespace Diligent::GLTF;
//using namespace 
CGLTF::CGLTF(wstring strName):
	CBaseRenderObject(strName)
{

}


CGLTF::~CGLTF()
{
	
}

void CGLTF::Tick(DWORD dwTimes)
{
}
bool CGLTF::LoadFromObjFile(WCHAR* fileName)
{
	CString fullPath;
	GetFullPath(fileName, fullPath);
	char* filePath = CustomW2A(fullPath);
	m_pGLTFModel = std::make_shared<Model>(m_pd3dDevice, m_pContext, filePath);
	//m_pGLTFModel.reset(new Model(m_pd3dDevice, m_pContext,filePath));
	delete[]filePath;
	return true;
}
bool CGLTF::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext )
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext	 = pContext;
	
	m_TransMatrixBuffer = std::shared_ptr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(globalmatrix), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0) , DeleteComPtr ) ;
	m_LightInfoBuffer = std::shared_ptr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(lightinfo), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0) , DeleteComPtr );
	m_MaterialBuffer = std::shared_ptr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(material), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0), DeleteComPtr);
	ID3D10Blob* pVertexShader = NULL;
	HRESULT hr = CompileShaderFromFile(_T("gltfmodel.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShader);
	assert(SUCCEEDED(hr));
	D3D11_INPUT_ELEMENT_DESC elements[]=
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",1,DXGI_FORMAT_R32G32_FLOAT,0,32,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",1,DXGI_FORMAT_R32G32B32A32_FLOAT,0,40,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",2,DXGI_FORMAT_R32G32B32A32_FLOAT,0,56,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	pd3dDevice->CreateInputLayout(elements, ARRAYSIZE(elements), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	//vertex shader
	hr = m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pVertexShader);
	assert(SUCCEEDED(hr));

	ID3D10Blob* pPixelShader = NULL;
	hr = CompileShaderFromFile(_T("gltfmodel.hlsl"), NULL, NULL, "ps_main", "ps_4_0", 0, 0, NULL, &pPixelShader);
	assert(SUCCEEDED(hr));
	//pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_pPixelShader);
	assert(SUCCEEDED(hr));

	return true;
}
bool CGLTF::Render(DWORD dwTimes)
{
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
	/*UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;*/
	/*m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);*/

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//m_pContext->DrawIndexed(36, 0, 0);
	//Update
	UINT stride = sizeof(Model::Vertex);
	UINT offset = 0;
	ID3D11Buffer* pTempBuffer = m_pGLTFModel->pVertexBuffer.get();
	m_pContext->IASetVertexBuffers(0, 1,&pTempBuffer , &stride, &offset);//這個dg引擎是怎么传递这个指针的
	pTempBuffer = m_pGLTFModel->pIndexBuffer.get();
	m_pContext->IASetIndexBuffer(pTempBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pContext->DrawIndexed(30, 0, 0);
	/*for (auto& Node : m_pGLTFModel->Nodes)
	{
		
	}*/
	return false;
}
