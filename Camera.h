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
	void SetPostionAndDir(XMFLOAT3 pos,XMFLOAT3 dir);
	void SetProjectionForLH(float fov, float aspect, float znear, float zfar);
private:
	XMFLOAT3 m_CameraPos;
	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjectionMatrix;
	bool m_ViewDirty;
	bool m_ProjectionDirty;
};