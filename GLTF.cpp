#include "unit.h"
#include "light.h"
#include "GLTFLoader.hpp"
using namespace Diligent::GLTF;
#include "gltf.h"
#include <assert.h>
#include<tchar.h>
#include "tiny_gltf.h"

#include "Scene.h"


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
	
	m_TransMatrixBuffer = CComPtr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(globalmatrix), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0) ) ;
	m_LightInfoBuffer = CComPtr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(lightinfo), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0) );
	m_MaterialBuffer = CComPtr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(material), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0));
	ID3DBlob* pVertexShader = NULL;
	HRESULT hr = CompileShaderFromFile(_T("gltfmodel.hlsl"), NULL, NULL, "vs_main", "vs_4_0", 0, 0, &pVertexShader);
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

	ID3DBlob* pPixelShader = NULL;
	hr = CompileShaderFromFile(_T("gltfmodel.hlsl"), NULL, NULL, "ps_main", "ps_4_0", 0, 0, &pPixelShader);
	assert(SUCCEEDED(hr));
	//pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_pPixelShader);
	assert(SUCCEEDED(hr));
	pPixelShader->Release();
	pPixelShader = NULL;

	D3D_SHADER_MACRO macro[] = {
		{"USE_TEX" , "1"},
		{nullptr,nullptr}
	};
	hr = CompileShaderFromFile(_T("gltfmodel.hlsl"), macro, NULL, "ps_main", "ps_4_0", 0, 0, &pPixelShader);
	assert(SUCCEEDED(hr));
	//pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_PixelShaderUseTex);
	assert(SUCCEEDED(hr));

	return true;
}
bool CGLTF::Render(DWORD dwTimes)
{
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->VSSetShader(m_pVertexShader, NULL, 0);
	
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Update
	UINT stride = sizeof(Model::Vertex);
	UINT offset = 0;
	ID3D11Buffer* pTempBuffer = m_pGLTFModel->pVertexBuffer;
	m_pContext->IASetVertexBuffers(0, 1,&pTempBuffer , &stride, &offset);//這個dg引擎是怎么传递这个指针的
	m_pContext->IASetIndexBuffer(m_pGLTFModel->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	for (auto& Node : m_pGLTFModel->Nodes)
	{
		for (auto& primitive : Node->_Mesh->Primitives)
		{
			DrawPrimitive(primitive);
		}
	}
	return false;
}
bool CGLTF::DrawPrimitive(std::unique_ptr<Primitive>& primitive) const
{
	material materialDst;
	materialDst.ambient = float4(0.2f, 0.2f, 0.2f,1.0f);
	materialDst.diffuse = float4(0.80f, 0.80f, 0.80f,1.0f);
	materialDst.specular = float4(1.0f, 1.0f, 1.0f,1.0f);
	materialDst.shininess =2.0f;
	m_pContext->UpdateSubresource(m_MaterialBuffer, 0, nullptr, &materialDst, 0, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_TransMatrixBuffer.p);
	m_pContext->VSSetConstantBuffers(1, 1, &m_LightInfoBuffer.p);
	m_pContext->VSSetConstantBuffers(2, 1, &m_MaterialBuffer.p);
	if (primitive->material.pBaseColorTexture == NULL)
	{
		m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
		const Diligent::GLTF::Material& materialSrc = primitive->material;
		
	}
	else
	{
		m_pContext->PSSetShader(m_PixelShaderUseTex, NULL, 0);
		ID3D11ShaderResourceView* pTextureResourceView = m_pGLTFModel->GetResourceView(primitive->material.pBaseColorTexture);
		m_pContext->PSSetShaderResources(0, 1, &pTextureResourceView);
		ID3D11SamplerState* pSamplerState = m_pGLTFModel->GetSampler(primitive->material.pBaseColorTexture);
		m_pContext->PSSetSamplers(0, 1, &pSamplerState);
	}
	m_pContext->DrawIndexed(primitive->IndexCount, primitive->FirstIndex, 0);
	return true;
}
bool CGLTF::UpdateRenderParams(const RenderParams& renderParams)
{
	//update transform
	globalmatrix glbMatrix;
	glbMatrix.world = renderParams.m_worldMatrix.Transpose();
	glbMatrix.world = glbMatrix.world * Matrix4x4<float>::RotationY(-PI / 2);
	glbMatrix.view = renderParams.m_viewMatrix.Transpose();
	glbMatrix.proj = renderParams.m_projMatrix.Transpose();
	glbMatrix.cameraPos = g_Scene.GetCamera()->GetPos();
	m_pContext->UpdateSubresource(m_TransMatrixBuffer, 0, nullptr, &glbMatrix, 0, 0);
	//update light
	lightinfo light;
	light.lightDir = float4(g_Scene.GetMainLight()->GetDir(),1.0f);
	light.lightcolor = float4(g_Scene.GetMainLight()->GetColor(),1.0f);
	m_pContext->UpdateSubresource(m_LightInfoBuffer, 0, nullptr, &light, 0, 0);


	return true;
}