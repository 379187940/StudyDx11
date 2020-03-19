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
	D3D11_INPUT_ELEMENT_DESC elements[]=
	{
		"POSITION",
	};
	pd3dDevice->CreateInputLayout()
	return true;
}
bool CGLTF::Render(DWORD dwTimes)
{
	//Update
	for (auto& Node : m_pGLTFModel->Nodes)
	{

	}
	return false;
}
