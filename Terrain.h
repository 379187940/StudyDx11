#pragma once
#include "BaseRenderObject.h"
class CTerrain :
	public CBaseRenderObject, public CCommonRenderData
{
public:
	CTerrain();
	~CTerrain();
public:
	bool Init(char* name);
	bool LoadHeightMap(char* name);
private:
	
};

