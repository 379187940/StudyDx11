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
	m_allObject[pRenderObject] = 1;
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
void CScene::BuildSelectRenderUi()
{
	/*TwBar *bar = TwNewBar("TweakBar");
	int barSize[2] = { 224, 320 };
	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		TwAddVarCB(bar, "Ambient Occlusion", TW_TYPE_BOOLCPP, SetSpongeAOCB, GetSpongeAOCB, NULL, "group=Sponge key=o");
	}*/
}