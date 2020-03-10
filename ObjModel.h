#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
#include "MeshLoader11.h"
class CObjModel :
	public CBaseRenderObject,public CCommonRenderData
{
	struct SimpleVertex
	{
		XMFLOAT3 pos;
		//float r, g, b;
	};
	struct globalmatrix
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX matviewproj;
	};
	struct lightinfo
	{
		XMFLOAT4 lightPos;
		XMFLOAT4 lightcolor;
	};
	struct material
	{
		XMFLOAT3 ambient;
		XMFLOAT3 diffuse;
		XMFLOAT3 specular;
		int    shininess;
		float  alpha;
		bool   bSpecular;
	};
public:
	CObjModel(wstring strName);
	virtual ~CObjModel();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams) { return true; }
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool LoadFromObjFile(WCHAR* fileName);
	CMeshLoader11 m_ObjData;
	std::shared_ptr<ID3D11Buffer> m_TransMatrixBuffer;
	std::shared_ptr<ID3D11Buffer> m_LightInfoBuffer;
	std::shared_ptr<ID3D11Buffer> m_MaterialBuffer;
};

