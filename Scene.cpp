#include "Scene.h"
#include "assert.h"
#include <Windows.h>
#include "IRenderObject.h"
#include "Cube.h"
#include "Triangle.h"
#include "CubeLight.h"
#include "unit.h"
#include <tchar.h>
CScene g_Scene;
CScene::CScene()
{
}


CScene::~CScene()
{
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		delete it->first;
	}
}
bool CScene::LoadDafultScene(ID3D11Device* pd3d11Device, ID3D11DeviceContext* pContext)
{
	if (!TwInit(TW_DIRECT3D11, pd3d11Device))
	{
		MessageBoxA( NULL, TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);
		return 0;
	}
	
	CTriangle* pNewTrianle = new CTriangle(_T("Triangle"));
	pNewTrianle->Init(pd3d11Device, pContext);
	CCube* pNewCube = new CCube(_T("Cube"));
	pNewCube->Init(pd3d11Device, pContext);
	CCubeLight* pNewCubeLight = new CCubeLight(_T("CubeLight"));
	pNewCubeLight->Init(pd3d11Device, pContext);

	RegisterObject(pNewTrianle);
	RegisterObject(pNewCube);
	RegisterObject(pNewCubeLight);

	BuildSelectRenderUi();
	return true;
}
bool CScene::RegisterObject(IRenderObject* pRenderObject)
{
	if (m_allObject.find(pRenderObject) != m_allObject.end())
		assert(0);
	m_allObject[pRenderObject] = m_allObject.size()-1;
	return true;
}
void CScene::Tick(DWORD dwTimes)
{
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		it->first->Tick(dwTimes);
	}
}
bool CScene::Render(DWORD dwTimes)
{
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		it->first->Render(dwTimes);
	}
	return true;
}
bool CScene::UpdateRenderParams(const RenderParams& renderParams)
{
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		it->first->UpdateRenderParams(renderParams);
	}
	return true;
}
void TW_CALL CScene::SetSpongeAOCB(const void *value, void * /*clientData*/)
{
	
}


// Callback function called by AntTweakBar to get the sponge recursion level
void TW_CALL CScene::GetSpongeAOCB(void *value, void * /*clientData*/)
{
	/**static_cast<int *>(value) = g_SpongeLevel;*/
}
void CScene::BuildSelectRenderUi()
{
	TwBar *bar = TwNewBar("Show Object");
	int barSize[2] = { 224, 320 };
	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	static char temp[MAX_PATH];
	static char tempKey[MAX_PATH];
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		CBaseRenderObject* pRenderObject = dynamic_cast<CBaseRenderObject*>(it->first);
		if (!pRenderObject)
			continue;
		wcstombs(temp, pRenderObject->GetName().data(), MAX_PATH);
		sprintf_s(tempKey, MAX_PATH, "group=Sponge key=%d", it->second);
		TwAddVarCB(bar, temp, TW_TYPE_BOOLCPP, SetSpongeAOCB, GetSpongeAOCB, NULL, tempKey);
	}
}