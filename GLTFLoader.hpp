/*
 *  Copyright 2019-2020 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

#pragma once

//#include <vector>
//#include <memory>
#include <cfloat>

typedef ID3D11Device IRenderDevice;
typedef ID3D11DeviceContext IDeviceContext;
typedef ID3D11Buffer        IBuffer;
typedef ID3D11Texture2D		ITexture;
typedef ID3D11SamplerState ISampler;
namespace tinygltf
{

class Node;
class Model;

} // namespace tinygltf

namespace Diligent
{

namespace GLTF
{

struct Material
{
    Material() noexcept {}

    enum ALPHA_MODE
    {
        ALPHAMODE_OPAQUE,
        ALPHAMODE_MASK,
        ALPHAMODE_BLEND
    };
    ALPHA_MODE AlphaMode = ALPHAMODE_OPAQUE;

    float  AlphaCutoff     = 1.0f;
    float  MetallicFactor  = 1.0f;
    float  RoughnessFactor = 1.0f;
    float4 BaseColorFactor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 EmissiveFactor  = float4(1.0f, 1.0f, 1.0f, 1.0f);

	CComPtr<ID3D11Texture2D> pBaseColorTexture;
	CComPtr<ID3D11Texture2D> pMetallicRoughnessTexture;
	CComPtr<ID3D11Texture2D> pNormalTexture;
	CComPtr<ID3D11Texture2D> pOcclusionTexture;
	CComPtr<ID3D11Texture2D> pEmissiveTexture;

    struct TextureCoordinateSets
    {
        unsigned char BaseColor          = 0;
        unsigned char MetallicRoughness  = 0;
        unsigned char SpecularGlossiness = 0;
        unsigned char Normal             = 0;
        unsigned char Occlusion          = 0;
        unsigned char Emissive           = 0;
    };
    TextureCoordinateSets TexCoordSets;

    struct Extension
    {
		CComPtr<ID3D11Texture2D> pSpecularGlossinessTexture;
		CComPtr<ID3D11Texture2D> pDiffuseTexture;
        float4                  DiffuseFactor  = float4(1.0f, 1.0f, 1.0f, 1.0f);
        float3                  SpecularFactor = float3(1.0f, 1.0f, 1.0f);
    };
    Extension extension;

    enum class PbrWorkflow
    {
        MetallicRoughness,
        SpecularGlossiness
    };
    PbrWorkflow workflow = PbrWorkflow::MetallicRoughness;
};

struct BoundBox
{
	float3 Min;
	float3 Max;
};
struct Primitive
{
    UINT32    FirstIndex  = 0;
    UINT32    IndexCount  = 0;
    UINT32    VertexCount = 0;
    Material& material;
    bool      hasIndices;

    BoundBox BB;
    bool     IsValidBB = false;

    Primitive(UINT32    _FirstIndex,
              UINT32    _IndexCount,
              UINT32    _VertexCount,
              Material& _material) :
        FirstIndex{_FirstIndex},
        IndexCount{_IndexCount},
        VertexCount{_VertexCount},
        material{_material},
        hasIndices{_IndexCount > 0}
    {
    }

    void SetBoundingBox(const float3& min, const float3& max)
    {
        BB.Min    = min;
        BB.Max    = max;
        IsValidBB = true;
    }
};



struct Mesh
{
    std::vector<std::unique_ptr<Primitive>> Primitives;

    BoundBox BB;
    BoundBox AABB;
    bool     IsValidBB = false;

    struct TransformData
    {
        static constexpr UINT32 MaxNumJoints = 128u;

        float4x4 matrix;
        float4x4 jointMatrix[MaxNumJoints] = {};
        int      jointcount                = 0;
    };

    TransformData Transforms;

    Mesh(IRenderDevice* pDevice, const float4x4& matrix);
    void SetBoundingBox(const float3& min, const float3& max);
};


struct Node;
struct Skin
{
    std::string           Name;
    Node*                 pSkeletonRoot = nullptr;
    std::vector<float4x4> InverseBindMatrices;
    std::vector<Node*>    Joints;
};


struct Node
{
    std::string Name;
    Node*       Parent = nullptr;
    UINT32      Index;

    std::vector<std::unique_ptr<Node>> Children;

    float4x4              Matrix;
    std::unique_ptr<Mesh> _Mesh;
    Skin*                 _Skin     = nullptr;
    INT32                 SkinIndex = -1;
    float3                Translation;
    float3                Scale = float3(1.0f, 1.0f, 1.0f);
    Quaternion            Rotation;
    BoundBox              BVH;
    BoundBox              AABB;
    bool                  IsValidBVH = false;

    float4x4 LocalMatrix() const;
    float4x4 GetMatrix() const;
    void     Update();
};


struct AnimationChannel
{
    enum PATH_TYPE
    {
        TRANSLATION,
        ROTATION,
        SCALE
    };
    PATH_TYPE PathType;
    Node*     node         = nullptr;
    UINT32    SamplerIndex = static_cast<UINT32>(-1);
};


struct AnimationSampler
{
    enum INTERPOLATION_TYPE
    {
        LINEAR,
        STEP,
        CUBICSPLINE
    };
    INTERPOLATION_TYPE  Interpolation;
    std::vector<float>  Inputs;
    std::vector<float4> OutputsVec4;
};

struct Animation
{
    std::string                   Name;
    std::vector<AnimationSampler> Samplers;
    std::vector<AnimationChannel> Channels;

    float Start = (std::numeric_limits<float>::max)();
    float End   = (std::numeric_limits<float>::min)();
};


struct Model
{
    struct Vertex
    {
        float3 pos;
        float3 normal;
        float2 uv0;
        float2 uv1;
        float4 joint0;
        float4 weight0;
    };

    CComPtr<IBuffer> pVertexBuffer;
	CComPtr<IBuffer> pIndexBuffer;
    UINT32                 IndexCount = 0;

    float4x4 aabb;

    std::vector<std::unique_ptr<Node>> Nodes;
    std::vector<Node*>                 LinearNodes;

    std::vector<std::unique_ptr<Skin>> Skins;

    std::vector<CComPtr<ITexture>> Textures;
	std::vector<CComPtr<ID3D11ShaderResourceView>> ResoruceView;
    std::vector<CComPtr<ISampler>> TextureSamplers;
	std::vector<CComPtr<ISampler>> TextureSamplersIndex;
    std::vector<Material>                Materials;
    std::vector<Animation>               Animations;
    std::vector<std::string>             Extensions;

    struct Dimensions
    {
        float3 min = float3{+FLT_MAX, +FLT_MAX, +FLT_MAX};
        float3 max = float3{-FLT_MAX, -FLT_MAX, -FLT_MAX};
    } dimensions;

    Model(IRenderDevice* pDevice, IDeviceContext* pContext, const std::string& filename);

    void UpdateAnimation(UINT32 index, float time);
	CComPtr<ISampler> GetSampler(ITexture* pTexture);
	ID3D11ShaderResourceView* GetResourceView(ITexture* pTexture);
private:
    void LoadFromFile(IRenderDevice* pDevice, IDeviceContext* pContext, const std::string& filename);
    void LoadNode(IRenderDevice*         pDevice,
                  Node*                  parent,
                  const tinygltf::Node&  gltf_node,
                  uint32_t               nodeIndex,
                  const tinygltf::Model& gltf_model,
                  std::vector<uint32_t>& indexBuffer,
                  std::vector<Vertex>&   vertexBuffer);

    void LoadSkins(const tinygltf::Model& gltf_model);

    void LoadTextures(IRenderDevice*         pDevice,
                      IDeviceContext*        pCtx,
                      const tinygltf::Model& gltf_model);

    void  LoadTextureSamplers(IRenderDevice* pDevice, const tinygltf::Model& gltf_model);
    void  LoadMaterials(const tinygltf::Model& gltf_model);
    void  LoadAnimations(const tinygltf::Model& gltf_model);
    void  CalculateBoundingBox(Node* node, const Node* parent);
    void  GetSceneDimensions();
    Node* FindNode(Node* parent, UINT32 index);
    Node* NodeFromIndex(uint32_t index);
};

} // namespace GLTF

} // namespace Diligent
