#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>

class IObject
{
public:
	//IObject() = 0;
	virtual void Tick(DWORD dwTimes) = 0;
};

