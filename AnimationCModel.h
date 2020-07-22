#pragma once
#include "unit.h"
#include "cScene.h"
class CAnimationCModel
{
public:
	class cMesh
	{
	public:

		class cVertexBoneWeight
		{
		public:
			cVertexBoneWeight() :
				boneID(0),
				vertexID(0),
				weight(0.0f)
			{
			}

			unsigned int boneID;
			unsigned int vertexID;
			float weight;
		};

		class cBone
		{
		public:
			cBone() :
				name(""),
				parent(NULL),
				numChildren(0),
				transform(float4x4::Identity())
			{
			}
			// TODO: Write dtor to clean up bone weights

			std::string name;
			float4x4 transform;

			cBone* parent;
			unsigned int numChildren;
			std::vector<cBone*> children;

			unsigned int numVertexBoneWeights;
			std::vector<cVertexBoneWeight*> vertexBoneWeights;


			// From feeney's code
			//float4x4 boneOffset;
			float4x4 finalTransformation;
			float4x4 objectBoneTransformation;

		};

		cMesh();
		~cMesh();

		std::string meshName;

		// a vertex should have its own class, probably.
		std::vector<float4> vertices;
		std::vector<float3> normals;
		std::vector<float3> texCoords;
		std::vector<uint4> boneIDs;

		std::vector<float4> boneWeights;

		std::vector<uint3> indices;

		//std::vector<cVertexBoneWeight*> vertexBoneWeights;
		std::map<unsigned int /* Vertex ID */, std::vector<cVertexBoneWeight*>> vertIDToBoneWeights;

		std::map<unsigned int /* Shader Program Id */, unsigned int /* VAOID */> shadIdToVAOId;

		unsigned int numberOfVertices;
		unsigned int numberOfIndices;

	private:
		friend bool LoadCharacter();
		friend class cSceneManager;
		friend class cScene;
		friend class CAnimationCModel;
		// ** Bones **
		unsigned int mNumBones;
		std::vector<cBone*> mBones; // master list 
		unsigned int mNumRootBones;
		std::vector<cBone*> mRootBones;
		std::map<std::string /* Bone name */, cBone*> mNameToBone;
	};
	class cNode
	{
	public:
		cNode() :
			name(""),
			parent(NULL),
			numChildren(0),
			transform(float4x4::Identity())
		{
		}

		std::string name;
		float4x4 transform;

		cNode* parent;
		unsigned int numChildren;
		std::vector<cNode*> children;

	};
	class cAnimation
	{
	public:

		class cBoneAnimation
		{
		public:
			cBoneAnimation() {}
			~cBoneAnimation() {}

			struct sV3Data
			{
				float time;
				float3 data;
			};
			struct sQData
			{
				float time;
				Quaternion data;
			};
			std::vector<sV3Data*> positionKeys;
			std::vector<sV3Data*> scalingKeys;
			std::vector<sQData*> rotationKeys;

			std::string nodeName;

			unsigned int numPositions;
			unsigned int numRotations;
			unsigned int numScales;

		};

		cAnimation() {}
		~cAnimation() {}

		cBoneAnimation* findBoneAnimationByName(std::string& boneName);

		std::string name;
		unsigned int numBoneChannels;
		float durationInTicks;
		float ticksPerSecond;
		std::vector<cBoneAnimation*> boneAnimations;
		std::map<std::string /* name */, cBoneAnimation*> mapBoneAnimations;
	};
public:
	CAnimationCModel();
	~CAnimationCModel();
public:
	bool LoadCharacter(std::string strSkin , vector<std::string>& action);
private:
	friend bool mProcessChildNodes(CAnimationCModel* pModel, CAnimationCModel::cNode* parentnode, aiNode* parentainode);
	vector<cMesh*> m_Skin;
	vector<cAnimation> m_Actons;
	cNode* m_RootNode;
	unsigned int m_NumNodes;
	std::vector<cNode*> m_Nodes; // master list
	std::map<std::string /* Node name */, cNode*> m_NameToNode;
	float4x4 m_GlobalInverseTransform;
};

