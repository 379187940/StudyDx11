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
protected:
	map<IRenderObject*,int> m_allObject;
	void BuildSelectRenderUi();
};
extern CScene g_Scene;

