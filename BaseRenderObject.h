#pragma once
#include "IRenderObject.h"

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

