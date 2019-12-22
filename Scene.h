#pragma once
#include <map>
#include "IRenderObject.h"
#include <Windows.h>
#include "AntTweakBar.h"
struct RenderParam;
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
	static void TW_CALL SetSpongeAOCB(const void *value, void * /*clientData*/);


	// Callback function called by AntTweakBar to get the sponge recursion level
	static void TW_CALL GetSpongeAOCB(void *value, void * /*clientData*/);
	
protected:
	void BuildSelectRenderUi();
private:
	map<IRenderObject*, int> m_allObject;
	bool m_bDrawDepth;
	ID3D11Texture2D* m_pDepth = NULL;
	ID3D11Device*    m_pD3d11Device = NULL;
	ID3D11DeviceContext* m_pD3d11Context = NULL;
};
extern CScene g_Scene;

