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
	return SUCCEEDED(hr) ? true : false;
}
bool CObjModel::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext )
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext	 = pContext;
	
	return true;
}
bool CObjModel::Render(DWORD dwTimes)
{
	
	return false;
}
