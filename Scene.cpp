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
	textureDesc.BindFlags =0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.Format = DXGI_FORMAT_R32_UINT;;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc = { 1,0 };
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.MiscFlags = 0;
	HRESULT hr = pd3d11Device->CreateTexture2D(&textureDesc, NULL, &m_pDepth);
	CTriangle* pNewTrianle = new CTriangle(_T("Triangle"));
	pNewTrianle->Init(pd3d11Device, pContext);
	CCube* pNewCube = new CCube(_T("Cube"));
	pNewCube->Init(pd3d11Device, pContext);
	CCubeLight* pNewCubeLight = new CCubeLight(_T("CubeLight"));
	pNewCubeLight->Init(pd3d11Device, pContext);
	CQuard* pNewQuard = new CQuard(_T("CQuard"));
	pNewQuard->Init(pd3d11Device, pContext);

	RegisterObject(pNewTrianle);
	RegisterObject(pNewCube);
	RegisterObject(pNewCubeLight);
	RegisterObject(pNewQuard);
	BuildSelectRenderUi();

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
	pNewQuard->SetDepthTexture(m_pDepthTextureSRV);
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
	/*HRESULT hr;
	ID3D11Resource* pResource = NULL;
	m_pDepthTextureSRV->GetResource(&pResource);*/
	/*ID3D10Texture2D* pTemp;
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D10_TEXTURE2D_DESC));
	pResource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&pTemp);
	pTemp->GetDesc(&desc);
	D3D10_MAPPED_TEXTURE2D mappedTex2D;
	pTemp->Map(0, D3D10_MAP_READ, 0, &mappedTex2D);
	pTemp->Unmap(0);
	pResource->Release();*/
	//CreateShaderResourceView
	////pDepthView->Release();
	//D3D11_RESOURCE_DIMENSION resroure_dimesion;
	//pResource->GetType(&resroure_dimesion);
	//if (resroure_dimesion == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
	//{
	//	const UINT nSubResource = D3D11CalcSubresource(0, 0, 1);
	//	//m_pD3d11Context->CopySubresourceRegion(m_pDepth, nSubResource, 0, 0, 0, pResource, nSubResource, NULL);
	//	m_pD3d11Context->CopyResource(m_pDepth, pResource);
	//	//D3DX11SaveTextureToFileA(m_pD3d11Context, m_pDepth, D3DX11_IFF_JPG, "33332222.jpg");
	//	//m_pDepth->
	//	D3D11_MAPPED_SUBRESOURCE mappedTexture;
	//	HRESULT hr = m_pD3d11Context->Map(m_pDepth, nSubResource, D3D11_MAP_READ, 0, &mappedTexture);
	//	
	//	m_pD3d11Context->Unmap(m_pDepth, 0);
	//}
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