#pragma once

#include <d3d11.h>
//#include <d3dx11.h>
#include <Windows.h>
class IObject
{
public:
	virtual ~IObject() = 0 ;
	virtual void Tick(DWORD dwTimes) = 0;
};
inline IObject::~IObject()
{

}

