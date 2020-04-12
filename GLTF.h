#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
//#include "MeshLoader11.h"
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
		float3 pos;
		//float r, g, b;
	};
	struct globalmatrix
	{
		float4x4 world;
		float4x4 view;
		float4x4 proj;
		//float4x4 matviewproj;
	};
	struct lightinfo
	{
		float3 lightDir;
		float4 lightcolor;
	};
	struct material
	{
		float3 ambient;
		float3 diffuse;
		float3 specular;
		int    shininess;
	};
public:
	CGLTF(wstring strName);
	virtual ~CGLTF();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams);
	virtual bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool LoadFromObjFile(WCHAR* fileName);
protected:
	bool DrawPrimitive(std::unique_ptr<Primitive>& primitive) const;
private:
	std::shared_ptr<Diligent::GLTF::Model> m_pGLTFModel;
	//CMeshLoader11 m_ObjData;
	CComPtr<ID3D11Buffer> m_TransMatrixBuffer;
	CComPtr<ID3D11Buffer> m_LightInfoBuffer;
	CComPtr<ID3D11Buffer> m_MaterialBuffer;
	CComPtr<ID3D11PixelShader> m_PixelShaderUseTex;
};

