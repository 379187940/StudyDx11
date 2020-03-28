#include "unit.h"
#include "ObjModel.h"
#include <assert.h>
#include<tchar.h>
#include "MeshLoader11.h"
CObjModel::CObjModel(wstring strName):
	CBaseRenderObject(strName)
{

}


CObjModel::~CObjModel()
{
	
}

void CObjModel::Tick(DWORD dwTimes)
{
}
bool CObjModel::LoadFromObjFile(WCHAR* fileName)
{
	CString fullPath;
	GetFullPath(fileName, fullPath);
	HRESULT hr = m_ObjData.Create(m_pd3dDevice, fullPath);
	m_ObjData.GetNumSubsets();
	return SUCCEEDED(hr) ? true : false;
}
bool CObjModel::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext )
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext	 = pContext;
	
	m_TransMatrixBuffer = CComPtr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(globalmatrix), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0) , DeleteComPtr ) ;
	m_LightInfoBuffer = CComPtr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(lightinfo), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0) , DeleteComPtr );
	m_MaterialBuffer = CComPtr<ID3D11Buffer>(CreateBuffer(m_pd3dDevice, sizeof(material), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0), DeleteComPtr);

	return true;
}
bool CObjModel::Render(DWORD dwTimes)
{
	//Update
	
	return false;
}
