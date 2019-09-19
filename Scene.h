#pragma once
#include <map>
class IRenderObject;
class RenderParam;
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
protected:
	map<IRenderObject*,int> m_allObject;

};

