#pragma once
#include <map>
#include "IRenderObject.h"
#include <Windows.h>
#include "AntTweakBar.h"
struct RenderParam;
class CQuard;
class FirstPersonCamera;
using namespace std;
class CScene
{
public:
	CScene();
	~CScene();
public:
	bool RegisterObject(IRenderObject* pRenderObject);
	void Tick(DWORD dwTimes);
	bool Render(DWORD dwTimes);
	bool UpdateRenderParams(const RenderParams& renderParams);
	//Tutoral Scene in general not use
	bool LoadDafultScene(ID3D11Device* pd3d11Device, ID3D11DeviceContext* pContext);
	bool Release();
	void UpdateInput(InputController& Controller, float ElapsedTime);
protected:
	static void TW_CALL SetObjectVisible(const void *value, void * /*clientData*/);
	static void TW_CALL GetObjectVisible(void *value, void * /*clientData*/);
	static void TW_CALL SetRenderDepth(const void *value, void * /*clientData*/);
	static void TW_CALL GetRenderDepth(void *value, void * /*clientData*/);


protected:
	void BuildUi();
	void UpdateCamera(float3 cameraPos, float3 lookAt, float nearClipPlane , float farClipPlane );
	void SetRenderDepth(bool bDrawDepth) { m_bDrawDepth = bDrawDepth; }
	bool IsDrawDepth() { return m_bDrawDepth; }
private:
	map<IRenderObject*, int> m_allObject;
	bool m_bDrawDepth = false ;
	ID3D11Device*    m_pD3d11Device = NULL;
	ID3D11DeviceContext* m_pD3d11Context = NULL;
	ID3D11ShaderResourceView*    m_pDepthTextureSRV = NULL;
	CQuard* m_quardDepth = NULL;
	FirstPersonCamera* m_pCmaera = NULL ;
};
extern CScene g_Scene;

