#pragma once
#include <map>
#include "IRenderObject.h"
#include <Windows.h>
#include "AntTweakBar.h"
#include "AnimationCModel.h"
struct RenderParam;
class CQuard;
class FirstPersonCamera;
class ShaderManagerClass;
class CDirLight;
using namespace std;
class CScene
{
public:
	CScene();
	~CScene();
public:
	FontClass& GetDefaultFont() { return m_font; }

	bool RegisterObject(IRenderObject* pRenderObject);
	bool UnRegisterObject(IRenderObject* pRenderObject);
	void Tick(DWORD dwTimes);
	bool Render(DWORD dwTimes);
	bool UpdateRenderParams();
	//Tutoral Scene in general not use
	bool LoadDafultScene(ID3D11Device* pd3d11Device, ID3D11DeviceContext* pContext);
	bool Release();
	void UpdateInput(InputController& Controller, float ElapsedTime);
	FirstPersonCamera* GetCamera() { return m_pCmaera; }
	const CDirLight* GetMainLight() { return m_pMainLight; }
	ShaderManagerClass* GetShaderManager() { return m_pShaderManagerClass; }

protected:
	//ui
	static void TW_CALL SetObjectVisible(const void *value, void * /*clientData*/);
	static void TW_CALL GetObjectVisible(void *value, void * /*clientData*/);
	static void TW_CALL SetRenderDepth(const void *value, void * /*clientData*/);
	static void TW_CALL GetRenderDepth(void *value, void * /*clientData*/);


protected:
	void BuildUi();
	void UpdateCamera(float3 cameraPos, float3 lookAt, float nearClipPlane , float farClipPlane );
	void SetRenderDepth(bool bDrawDepth) { m_bDrawDepth = bDrawDepth; }
	bool IsDrawDepth() { return m_bDrawDepth; }
	void RenderFps(DWORD dwTimes);
	void CreateOfenUseState();
	void EnableAlphaBlending();
	void DisableAlphaBlending();
private:
	map<IRenderObject*, int> m_allObject;
	bool m_bDrawDepth = false ;
	bool m_bWireFrame = false;
	ID3D11Device*    m_pD3d11Device = NULL;
	ID3D11DeviceContext* m_pD3d11Context = NULL;
	ID3D11ShaderResourceView*    m_pDepthTextureSRV = NULL;
	CQuard* m_quardDepth = NULL;
	FirstPersonCamera* m_pCmaera = NULL ;
	Quaternion m_rotation;
	CDirLight* m_pMainLight;
	CComPtr<ID3D11RasterizerState> m_pFillSolidState;
	CComPtr<ID3D11RasterizerState> m_pFillFrameState;
	CComPtr<ID3D11DepthStencilState> m_depthDisabledStencilState;
	CComPtr<ID3D11DepthStencilState> m_depthStencilState;
	CComPtr<ID3D11BlendState> m_alphaEnableBlendingState;
	CComPtr<ID3D11BlendState> m_alphaDisableBlendingState;
	FontClass m_font;
	TextClass m_fpsString;
	ShaderManagerClass* m_pShaderManagerClass;
	CAnimationCModel* m_pHostPlayer = NULL ;
};
extern CScene g_Scene;

