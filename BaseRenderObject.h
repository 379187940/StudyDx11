#pragma once
#include "IRenderObject.h"
#include "unit.h"
#include "Common.h"
using std::wstring;
class CBaseRenderObject :public IRenderObject
{
public:
	CBaseRenderObject(wstring strName);
	virtual ~CBaseRenderObject();
public:
	wstring GetName();
	void SetVisible(bool isVisible) { m_isVisible = isVisible; }
	bool IsVisible() { return m_isVisible; }
private:
	wstring m_Name;//²»Î¨Ò» 
	bool m_isVisible;
};

