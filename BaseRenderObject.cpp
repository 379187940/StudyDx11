#include "BaseRenderObject.h"



CBaseRenderObject::CBaseRenderObject(wstring strName)
{
	m_Name = strName;
}


CBaseRenderObject::~CBaseRenderObject()
{

}
wstring CBaseRenderObject::GetName()
{
	return m_Name;
}