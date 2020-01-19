#include "Scene.h"
#include "assert.h"
#include <Windows.h>
#include "IRenderObject.h"
#include "Cube.h"
#include "Triangle.h"
#include "CubeLight.h"
#include "Quard.h"
#include "unit.h"
#include <tchar.h>
CScene g_Scene;
CScene::CScene()
{
	m_bDrawDepth = false;
}


CScene::~CScene()
{
	
}
bool CScene::Release()
{
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		delete it->first;
	}
	delete m_quardDepth;
	m_pDepthTextureSRV->Release();
	return true;
}
bool CScene::LoadDafultScene(ID3D11Device* pd3d11Device, ID3D11DeviceContext* pContext)
{
	m_pD3d11Device = pd3d11Device;
	m_pD3d11Context = pContext;
	
	CTriangle* pNewTrianle = new CTriangle(_T("Triangle"));
	pNewTrianle->Init(pd3d11Device, pContext);
	CCube* pNewCube = new CCube(_T("Cube"));
	pNewCube->Init(pd3d11Device, pContext);
	CCubeLight* pNewCubeLight = new CCubeLight(_T("CubeLight"));
	pNewCubeLight->Init(pd3d11Device, pContext);
	m_quardDepth = new CQuard(_T("CQuard"));
	m_quardDepth->Init(pd3d11Device, pContext);

	RegisterObject(pNewTrianle);
	RegisterObject(pNewCube);
	RegisterObject(pNewCubeLight);
	//RegisterObject(pNewQuard);
	BuildUi();
	HRESULT hr = S_FALSE ;
	ID3D11RenderTargetView* pRenderTargetView = NULL;
	ID3D11DepthStencilView* pDepthView = NULL;
	m_pD3d11Context->OMGetRenderTargets(1, &pRenderTargetView, &pDepthView);
	pRenderTargetView->Release();
	ID3D11Resource* pResource = NULL;
	pDepthView->GetResource(&pResource);
	pDepthView->Release();
	D3D11_SHADER_RESOURCE_VIEW_DESC SRDesc;
	SRDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRDesc.Texture2D.MostDetailedMip = 0;
	SRDesc.Texture2D.MipLevels = 1;
	hr = m_pD3d11Device->CreateShaderResourceView(pResource, &SRDesc, &m_pDepthTextureSRV );
	
	pResource->Release();
	m_quardDepth->SetDepthTexture(m_pDepthTextureSRV);
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
		if ( it->first->IsVisible() )
			it->first->Render(dwTimes);
	}
	if (IsDrawDepth())
	{
		m_quardDepth->Render(dwTimes);
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
void TW_CALL CScene::SetRenderDepth(const void *value, void * clientData)
{
	bool bDraw = *static_cast<const bool *>(value);
	g_Scene.SetRenderDepth(bDraw);
}
void TW_CALL CScene::GetRenderDepth(void *value, void * clientData)
{
	*static_cast<bool *>(value) = g_Scene.IsDrawDepth();
}
void TW_CALL CScene::SetObjectVisible(const void *value, void * clientData)
{ 
	bool bShow = *static_cast<const bool *>(value);
	CBaseRenderObject* pRenderObject = static_cast<CBaseRenderObject*>(clientData);
	pRenderObject->SetVisible(bShow);
}
void TW_CALL CScene::GetObjectVisible(void *value, void * clientData)
{
	CBaseRenderObject* pRenderObject = static_cast<CBaseRenderObject*>(clientData);
	*static_cast<bool *>(value) = pRenderObject->IsVisible();
	
}
void CScene::BuildUi()
{
	TwBar *showObjectBar = TwNewBar("Show Object");
	int barSize[2] = { 224, 320 };
	TwSetParam(showObjectBar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	static char temp[MAX_PATH];
	static char tempKey[MAX_PATH];
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		CBaseRenderObject* pRenderObject = dynamic_cast<CBaseRenderObject*>(it->first);
		if (!pRenderObject)
			continue;
		wcstombs(temp, pRenderObject->GetName().data(), MAX_PATH);
		sprintf_s(tempKey, MAX_PATH, "group=Sponge key=%d", it->second);
		TwAddVarCB(showObjectBar, temp, TW_TYPE_BOOLCPP, SetObjectVisible, GetObjectVisible, it->first, tempKey);
	}
	TwBar *debugPanel = TwNewBar("Debug Panel");
	TwAddVarCB(debugPanel, "RenderDepth", TW_TYPE_BOOLCPP, SetRenderDepth, GetRenderDepth, this, "");
}