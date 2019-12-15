#include "BaseRenderObject.h"



CBaseRenderObject::CBaseRenderObject(wstring strName)
{
	m_Name = strName;
	m_isVisible = true;
}


CBaseRenderObject::~CBaseRenderObject()
{

}
wstring CBaseRenderObject::GetName()
{
	return m_Name;
}