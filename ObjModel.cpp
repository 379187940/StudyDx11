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
	m_ObjData.Create(m_pd3dDevice, fileName);
	WCHAR workDir[MAX_PATH] = L"";
	if ( wcsrchr(fileName, L'\\') == nullptr &&
		wcsrchr(fileName, L'/') == nullptr )
		_tgetcwd(workDir, MAX_PATH - 1);
	if (
		workDir[lstrlenW(workDir) - 1] != L'\\' &&
		workDir[lstrlenW(workDir) - 1] != L'/'
		)
	{
		workDir[lstrlenW(workDir)] = L'\\';
		workDir[lstrlenW(workDir)] = L'\0';
	}
	WCHAR* fullPathName = lstrcatW(workDir, fileName);
	LoadFromObjFile(fullPathName);
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
