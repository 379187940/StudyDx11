#pragma once
#include "IRenderObject.h"
#include "unit.h"
using std::wstring;
class CBaseRenderObject:public IRenderObject
{
public:
	CBaseRenderObject(wstring strName);
	virtual ~CBaseRenderObject();
public:
	wstring GetName();
private:
	wstring m_Name;//²»Î¨Ò» 
};

