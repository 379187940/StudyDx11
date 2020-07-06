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
#include "tiny_gltf.h"
#include "GLTF.h"
#include "terrain.h"
#include "shadermanagerclass.h"
CScene g_Scene;
CScene::CScene()
{
	m_bDrawDepth = false;
	m_pShaderManagerClass = new ShaderManagerClass();
}


CScene::~CScene()
{
	delete m_pCmaera;
	delete m_pMainLight;
	if (m_pShaderManagerClass )
		m_pShaderManagerClass->Shutdown();
	delete m_pShaderManagerClass;
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
void CScene::CreateOfenUseState()
{
	HRESULT hr;
	//raster state
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthBias = TRUE;
	hr = m_pD3d11Device->CreateRasterizerState(&desc, &m_pFillFrameState);
	assert(SUCCEEDED(hr));
	desc.FillMode = D3D11_FILL_SOLID;
	hr = m_pD3d11Device->CreateRasterizerState(&desc, &m_pFillSolidState);
	assert(SUCCEEDED(hr));

	//depthstenci state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = m_pD3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	hr = m_pD3d11Device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	assert(SUCCEEDED(hr));

	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.AlphaToCoverageEnable = FALSE;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = m_pD3d11Device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	assert(SUCCEEDED(hr));

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	blendStateDescription.AlphaToCoverageEnable = false;

	// Create the blend state using the description.
	hr = m_pD3d11Device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	assert(SUCCEEDED(hr));


}
bool CScene::LoadDafultScene(ID3D11Device* pd3d11Device, ID3D11DeviceContext* pContext)
{
	m_pD3d11Device = pd3d11Device;
	m_pD3d11Context = pContext;
	m_pShaderManagerClass->Initialize(pd3d11Device, AfxGetMainWnd());
	m_font.Initialize(m_pD3d11Device, m_pD3d11Context, "font01.txt", "font01.tga", 32, 1);
	int2 windowSize;
	windowSize = AfxGetWindowSize();
	m_fpsString.Initialize(m_pD3d11Device, m_pD3d11Context, windowSize.x, windowSize.y , 20 , false,&m_font ,"fps: NULL"  ,  10 ,10 ,  1.0f , 0 , 0);

	//创建经常使用的raster state和depthstencil state
	CreateOfenUseState();
	
	CTriangle* pNewTrianle = new CTriangle(_T("Triangle"));
	pNewTrianle->Init(pd3d11Device, pContext);
	CCube* pNewCube = new CCube(_T("Cube"));
	pNewCube->Init(pd3d11Device, pContext);
	CCubeLight* pNewCubeLight = new CCubeLight(_T("CubeLight"));
	pNewCubeLight->Init(pd3d11Device, pContext);
	m_quardDepth = new CQuard(_T("CQuard"));
	m_quardDepth->Init(pd3d11Device, pContext);
	
	CGLTF* pNewGltf = new CGLTF(_T("GltfLoad"));
	pNewGltf->Init(pd3d11Device, pContext);
	pNewGltf->LoadFromObjFile(_T("cup2uv.gltf"));
	//RegisterObject(pNewTrianle);
	//RegisterObject(pNewCube);
	//RegisterObject(pNewCubeLight);
	//RegisterObject(pNewGltf);
	
	//CTerrain* pTerrain = new CTerrain(_T("Terrain"));
	//pTerrain->Init(pd3d11Device, pContext);
	//RegisterObject(pTerrain);
	
	//CGLTF* pNewTerrain = new CGLTF(_T("terrain"));
	//pNewTerrain->Init(pd3d11Device, pContext);
	//pNewTerrain->LoadFromObjFile(_T("terrain.gltf"));
	////RegisterObject(pNewTrianle);
	////RegisterObject(pNewCube);
	////RegisterObject(pNewCubeLight);
	//RegisterObject(pNewTerrain);


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
	UpdateCamera(float3(0, 0, 0),
		float3(0.0f, 0.0f, 0.0f),
		0.1f,
		100000.0f
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
void CScene::RenderFps(DWORD dwTimes)
{
	static DWORD dwTimeTick = 0;
	static DWORD  renderTime = 0;
	static DWORD dOldTick = 0;
	if (dOldTick == 0)
	{
		dOldTick = dwTimes;
		return;
	}
	dwTimeTick += dwTimes - dOldTick ;
	dOldTick = dwTimes;
	renderTime += 1;////////
	if (dwTimeTick > 1000)
	{
		char a[20];
		sprintf(a, "fps: %d", renderTime);
		m_fpsString.UpdateSentence(m_pD3d11Context, &m_font, a, 10, 10, 1.0f, 0.0f, 0.0f);
		dwTimeTick = 0;
		renderTime = 0;
	}
	else
	{
	
	}
	m_fpsString.Render(m_pD3d11Context, m_pShaderManagerClass, float4x4::Identity(), float4x4::Identity(), float4x4::Ortho(AfxGetWindowSize().x, AfxGetWindowSize().y, 0.0f, 1.0f, false) , m_font.GetTexture());
}
bool CScene::Render(DWORD dwTimes)
{
	EnableAlphaBlending();
	m_pD3d11Context->OMSetDepthStencilState(m_depthStencilState, 1);
	if (m_bWireFrame)
		m_pD3d11Context->RSSetState(m_pFillFrameState);
	else
		m_pD3d11Context->RSSetState(m_pFillSolidState);
	for (map<IRenderObject*, int>::iterator it = m_allObject.begin(); it != m_allObject.end(); it++)
	{
		if ( it->first->IsVisible() )
			it->first->Render(dwTimes);
	}
	if (IsDrawDepth())
	{
		m_quardDepth->Render(dwTimes);
	}
	if (m_bWireFrame)
		m_pD3d11Context->RSSetState(m_pFillSolidState);
	m_pD3d11Context->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	
	RenderFps( dwTimes);
	m_pD3d11Context->OMSetDepthStencilState(m_depthStencilState, 1);
	return true;
}
void CScene::EnableAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_pD3d11Context->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}


void CScene::DisableAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	m_pD3d11Context->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
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
	TwAddVarRW(debugPanel, "line", TW_TYPE_BOOLCPP, &m_bWireFrame, "group=RenderParams");
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
	
	TwAddVarRO(debugPanel, "Camera", TW_TYPE_QUAT4F, &m_rotation, "axisz = -z");
}