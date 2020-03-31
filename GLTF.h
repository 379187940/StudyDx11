#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
#include "MeshLoader11.h"
namespace Diligent
{
	namespace GLTF {
		struct Model; 
	}
}
class CGLTF :
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
		//XMMATRIX matviewproj;
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
	CGLTF(wstring strName);
	virtual ~CGLTF();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool LoadFromObjFile(WCHAR* fileName);
private:
	std::shared_ptr<Diligent::GLTF::Model> m_pGLTFModel;
	//CMeshLoader11 m_ObjData;
	CComPtr<ID3D11Buffer> m_TransMatrixBuffer;
	CComPtr<ID3D11Buffer> m_LightInfoBuffer;
	CComPtr<ID3D11Buffer> m_MaterialBuffer;
	CComPtr<ID3D11PixelShader> m_PixelShaderUseTex;
};

