#include "unit.h"
#include "GLTFLoader.hpp"
using namespace Diligent::GLTF;
#include "light.h"
#include "Scene.h"
#include "assert.h"
#include <Windows.h>
#include "IRenderObject.h"
#include "Cube.h"
#include "Triangle.h"
#include "CubeLight.h"
#include "Quard.h"
#include <tchar.h>
#include "ObjModel.h"
#include "tiny_gltf.h"
#include "GLTF.h"

CScene g_Scene;
CScene::CScene()
{
	m_bDrawDepth = false;
}


CScene::~CScene()
{
	delete m_pCmaera;
	delete m_pMainLight;
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
	/*CObjModel* ObjMode = new CObjModel(_T("fromobjload"));
	ObjMode->Init(pd3d11Device, pContext);
	ObjMode->LoadFromObjFile(_T("cup.obj"));*/
	CGLTF* pNewGltf = new CGLTF(_T("GltfLoad"));
	pNewGltf->Init(pd3d11Device, pContext);
	pNewGltf->LoadFromObjFile(_T("cup2uv.gltf"));
	//RegisterObject(pNewTrianle);
	//RegisterObject(pNewCube);
	//RegisterObject(pNewCubeLight);
	RegisterObject(pNewGltf);
	
	HRESULT hr = S_FALSE ;
	ID3D11RenderTargetView* pRenderTargetView = NULL;
	ID3D11DepthStencilView* pDepthView = NULL;
	m_pD3d11Context->OMGetRenderTargets(1, &pRenderTargetView, &pDepthView);
	pRenderTargetView->Release();
	ID3D11Resource* pResource = NULL;
	pDepthView->GetResource(&pResource);
	pDepthView->Release();
	D3D11_SHADER_RESOURCE_VIEW_DESC SRDesc;
	SRDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRDesc.Texture2D.MostDetailedMip = 0;
	SRDesc.Texture2D.MipLevels = 1;
	hr = m_pD3d11Device->CreateShaderResourceView(pResource, &SRDesc, &m_pDepthTextureSRV );
	
	pResource->Release();
	m_quardDepth->SetDepthTexture(m_pDepthTextureSRV);

	m_pCmaera = new FirstPersonCamera();
	UpdateCamera(float3(0, 0, -10.0f),
		float3(0.0f, 0.0f, 0.0f),
		0.1f,
		1000.0f
	);
	m_pCmaera->SetMoveSpeed(5.0f);
	BuildUi();
	m_pMainLight = new CDirLight(float3(1, -1, 1), float3(1.0f, 1.0f, 1.0f));
	return true;
}
void CScene::UpdateInput(InputController& Controller, float ElapsedTime)
{
	m_pCmaera->Update(Controller, ElapsedTime);
}
void CScene::UpdateCamera(float3 cameraPos, float3 lookAt, float nearClipPlane, float farClipPlane)
{
	m_pCmaera->SetPos(cameraPos);
	m_pCmaera->SetLookAt(lookAt);
	static UINT numPort = 1;
	D3D11_VIEWPORT viewPort;
	m_pD3d11Context->RSGetViewports(&numPort, &viewPort);
	m_pCmaera->SetProjAttribs(0.1f, 1000.0f, viewPort.Width / viewPort.Height, PI_F / 4.0f, false);
	
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
bool CScene::UpdateRenderParams()
{
	RenderParams renderParams;
	renderParams.m_worldMatrix = Matrix4x4<float>::Identity();
	renderParams.m_viewMatrix = m_pCmaera->GetViewMatrix();
	renderParams.m_projMatrix = m_pCmaera->GetProjMatrix();
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		it->first->UpdateRenderParams(renderParams);
	}
	m_rotation = g_Scene.m_pCmaera->GetRotation();
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
		sprintf_s(tempKey, MAX_PATH, "group=Sponge", it->second);
		TwAddVarCB(showObjectBar, temp, TW_TYPE_BOOLCPP, 
			SetObjectVisible, 
			GetObjectVisible, 
			it->first, tempKey);
	}
	TwBar *debugPanel = TwNewBar("Debug Panel");
	//TwAddVarCB(debugPanel, "RenderDepth", TW_TYPE_BOOLCPP, SetRenderDepth, GetRenderDepth, this, "");
	TwAddVarRW(debugPanel, "RenderDepth", TW_TYPE_BOOLCPP, &m_bDrawDepth, "group=RenderParams");
	TwAddVarCB(debugPanel, "MoveSpeed", TW_TYPE_FLOAT, 
		[](const void *value, void * clientData) {
		float fSpeed = *static_cast<const float *>(value);
		g_Scene.GetCamera()->SetMoveSpeed(fSpeed);
	}
		,
		[](void *value, void * clientData) {
		*static_cast<float *>(value) = g_Scene.GetCamera()->GetMoveSpeed();
	}, 
		this, "");
	//Quaternion
	
	TwAddVarRO(debugPanel, "Camera", TW_TYPE_QUAT4F, &m_rotation, "");
}