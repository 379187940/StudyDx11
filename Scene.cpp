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
	m_bDrawDepth = false;
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
	m_pD3d11Device = pd3d11Device;
	m_pD3d11Context = pContext;
	static UINT viewportNum = 1;
	D3D11_VIEWPORT viewPort;
	pContext->RSGetViewports(&viewportNum, &viewPort);
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = viewPort.Width;
	textureDesc.Height = viewPort.Height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc = { 1,0 };
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MiscFlags = 0;
	pd3d11Device->CreateTexture2D(&textureDesc, NULL, &m_pDepth);
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
	ID3D11RenderTargetView* pRenderTargetView = NULL;
	ID3D11DepthStencilView* pDepthView = NULL;
	m_pD3d11Context->OMGetRenderTargets(1, &pRenderTargetView, &pDepthView);
	pRenderTargetView->Release();
	ID3D11Resource* pResource = NULL;
	pDepthView->GetResource(&pResource);
	pDepthView->
	D3D11_RESOURCE_DIMENSION resroure_dimesion;
	pResource->GetType(&resroure_dimesion);

	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		if ( it->first->IsVisible() )
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
void TW_CALL CScene::SetSpongeAOCB(const void *value, void * clientData)
{ 
	bool bShow = *static_cast<const bool *>(value);
	CBaseRenderObject* pRenderObject = static_cast<CBaseRenderObject*>(clientData);
	pRenderObject->SetVisible(bShow);
}


// Callback function called by AntTweakBar to get the sponge recursion level
void TW_CALL CScene::GetSpongeAOCB(void *value, void * clientData)
{
	CBaseRenderObject* pRenderObject = static_cast<CBaseRenderObject*>(clientData);
	*static_cast<bool *>(value) = pRenderObject->IsVisible();
	
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
		TwAddVarCB(bar, temp, TW_TYPE_BOOLCPP, SetSpongeAOCB, GetSpongeAOCB, it->first, tempKey);
	}
}