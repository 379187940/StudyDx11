#pragma once
//#include "unit.h"
#include "IObject.h"
//透视投影相机
class CPerspectiveProjectionCamera:public IObject
{
public:
	CPerspectiveProjectionCamera();
	~CPerspectiveProjectionCamera();
public:
	void Tick(DWORD dwTimes);
	void SetPostionAndDir(float3 pos,float3 dir);
	void SetProjectionForLH(float fov, float aspect, float znear, float zfar);
private:
	float3 m_CameraPos;
	float3 m_ViewMatrix;
	float3 m_ProjectionMatrix;
	bool m_ViewDirty;
	bool m_ProjectionDirty;
};