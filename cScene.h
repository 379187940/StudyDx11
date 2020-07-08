#ifndef _sMesh_HG_
#define _sMesh_HG_

#include <map>
#include <vector>

//#include <glm\vec3.hpp>
//#include <glm\mat4x4.hpp>
//#include <glm\gtc\quaternion.hpp>

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

//#include "Geometry\cTriangle.h"

// TODO: Write a "MeshDef" for initializiation.
// TODO: Update this definition for multiple textures on a given mesh.



class cScene
{
public:

	class cMesh
	{
	public:

		class cVertexBoneWeight
		{
		public:
			cVertexBoneWeight() :
				boneID( 0 ),
				vertexID( 0 ),
				weight( 0.0f )
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
				name( "" ),
				parent( NULL ),
				numChildren( 0 ),
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
		friend class cSceneManager;
		friend class cScene;

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
			name( "" ),
			parent( NULL ),
			numChildren( 0 ),
			transform( float4x4::Identity() )
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

		cBoneAnimation* findBoneAnimationByName( std::string& boneName );

		std::string name;
		unsigned int numBoneChannels;
		float durationInTicks;
		float ticksPerSecond;
		std::vector<cBoneAnimation*> boneAnimations;
		std::map<std::string /* name */ , cBoneAnimation*> mapBoneAnimations; 
	};

	cScene() :
		mRootNode( NULL ),
		mNumNodes( 0 )
	{
	}

	~cScene();

	void getBoneAnimationData(
		float timeInSeconds,
		std::string& animationName,
		std::vector<float4x4>& finalTransforms,
		std::vector<float4x4>& globals,
		std::vector<float4x4>& offsets
	);

	void getBoneAnimationData(
		float timeInSeconds,
		std::string& animationName,
		std::vector<float4x4>& finalTransforms
	);
	
	void getBlendedAnimationData(
		float time1,
		std::string& name1,
		float time2,
		std::string& name2,
		float factor,
		std::vector<float4x4>& finalTransforms
	);

	std::string name;
	std::vector<cMesh*> meshes;

	std::vector<cAnimation*> animations;
	std::map<std::string, cAnimation*> mapAnimations;

private:
	friend class cSceneManager;

	void mReadNodeHeirarchy(
		float animationTime,
		cAnimation* pAnimation,
		cNode* pNode,
		float4x4& parentTransform
	);

	
	void mInterpolateRotation( 
		float animationTime, 
		cAnimation::cBoneAnimation* pNode,
		Quaternion& rotationOut 
	);
	unsigned int mFindRotation(
		float animationTime,
		cAnimation::cBoneAnimation* pNode
	);

	void mInterpolatePosition(
		float animationTime,
		cAnimation::cBoneAnimation* pNode,
		float3& positionOut
	);
	unsigned int mFindPosition(
		float animationTime,
		cAnimation::cBoneAnimation* pNode
	);

	void mInterpolateScale(
		float animationTime,
		cAnimation::cBoneAnimation* pNode,
		float3& scaleOut
	);
	unsigned int mFindScaling(
		float animationTime,
		cAnimation::cBoneAnimation* pNode
	);
	

	
	// ** Nodes **
	cNode* mRootNode;
	unsigned int mNumNodes;
	std::vector<cNode*> mNodes; // master list
	std::map<std::string /* Node name */, cNode*> mNameToNode;
	float4x4 mGlobalInverseTransform;




};

#endif
