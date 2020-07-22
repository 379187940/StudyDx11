#include "unit.h"
#include "cScene.h"

//#include <glm\gtc\matrix_transform.hpp>

cScene::cMesh::cMesh() :
	meshName( "" ), numberOfIndices( 0 ), numberOfVertices( 0 ),
	mNumBones( 0 ),
	mNumRootBones( 0 )
{
}

cScene::cMesh::~cMesh()
{
}


//cScene::cScene()
//{
//}

cScene::~cScene()
{
	for ( size_t index = 0; index < this->meshes.size(); index++ )
	{
		delete this->meshes[ index ];
		this->meshes[ index ] = 0;
	}
}

void cScene::getBoneAnimationData( float timeInSeconds, std::string& animationName,
	std::vector<float4x4>& finalTransforms, std::vector<float4x4>& globals,
	std::vector<float4x4>& offsets )
{
	// Based VERY heavily on cSimpleAssimpSkinnedMesh by Mike Feeney.
	// Currently only handles single mesh. the first mesh.

	cAnimation* pAnimation = this->mapAnimations[ animationName ];
	cMesh* pMesh = this->meshes[ 0 ]; // this is the "only handles first" part

	float4x4 identity = float4x4::Identity();

	float timeInTicks = timeInSeconds * pAnimation->ticksPerSecond;
	float animationTime = fmod( timeInTicks, pAnimation->durationInTicks );

	this->mReadNodeHeirarchy( animationTime, pAnimation, this->mRootNode, identity );

	finalTransforms.resize( pMesh->mNumBones );
	globals.resize( pMesh->mNumBones );
	offsets.resize( pMesh->mNumBones );

	for ( unsigned int boneIndex = 0; boneIndex < pMesh->mNumBones; boneIndex++ )
	{
		finalTransforms[ boneIndex ] = pMesh->mBones[ boneIndex ]->finalTransformation;
		globals[ boneIndex ] = pMesh->mBones[ boneIndex ]->objectBoneTransformation;
		offsets[ boneIndex ] = pMesh->mBones[ boneIndex ]->transform;
	}
}

void cScene::getBoneAnimationData( float timeInSeconds, std::string& animationName, std::vector<float4x4>& finalTransforms )
{	
	// Based VERY heavily on cSimpleAssimpSkinnedMesh by Mike Feeney.
	// Currently only handles single mesh. the first mesh.

	cAnimation* pAnimation = this->mapAnimations[ animationName ];
	cMesh* pMesh = this->meshes[ 0 ]; // this is the "only handles first" part

	float4x4 identity = float4x4::Identity();

	float timeInTicks = timeInSeconds * pAnimation->ticksPerSecond;
	float animationTime = fmod( timeInTicks, pAnimation->durationInTicks );

	this->mReadNodeHeirarchy( animationTime, pAnimation, this->mRootNode, identity );

	finalTransforms.resize( pMesh->mNumBones );

	for ( unsigned int boneIndex = 0; boneIndex < pMesh->mNumBones; boneIndex++ )
	{
		finalTransforms[ boneIndex ] = pMesh->mBones[ boneIndex ]->finalTransformation;
	}
}

void cScene::getBlendedAnimationData( float time1, std::string& name1,
	float time2, std::string& name2, float factor, std::vector<float4x4>& finalTransforms )
{
	std::vector<float4x4> bones1;
	std::vector<float4x4> bones2;

	this->getBoneAnimationData( time1, name1, bones1 );
	this->getBoneAnimationData( time2, name2, bones2 );

	finalTransforms.resize( bones1.size() );

	if ( factor < 0.0f ) factor = 0.0f;
	if ( factor > 1.0f ) factor = 1.0f;
	
	for ( unsigned int boneIndex = 0; boneIndex < finalTransforms.size(); boneIndex++ )
	{
		/*finalTransforms[ boneIndex ] =
			(1.0f - factor)*( bones1[ boneIndex ])
			+ ( bones2[ boneIndex ] * factor );*/
		float4x4 bone1Temp = bones1[boneIndex];
		float4x4 bone2Temp = bones2[boneIndex];
		bone1Temp *= (1.0f - factor);
		bone2Temp *= factor;
		finalTransforms[boneIndex] = bone1Temp + bone2Temp;
	}

}

void cScene::mReadNodeHeirarchy( float animationTime, cAnimation* pAnimation, 
	cNode* pNode, float4x4& parentTransform )
{
	cMesh* pMesh = this->meshes[ 0 ]; // this is the "only handles first" part

	float4x4 nodeTransform = pNode->transform;

	cAnimation::cBoneAnimation* pBoneAnim = pAnimation->findBoneAnimationByName( pNode->name );

	if ( pBoneAnim )
	{
		// get interpolated stuff
		Quaternion rotation;
		this->mInterpolateRotation( animationTime, pBoneAnim, rotation );
		float4x4 rotationMatrix = rotation.ToMatrix();

		float3 position;
		this->mInterpolatePosition( animationTime, pBoneAnim, position );

		//if ( pNode->name == "B_Pelvis" )
		//	position.x = 0.0f;

		float4x4 positionMatrix = float4x4::Translation ( position );

		float3 scale;
		this->mInterpolateScale( animationTime, pBoneAnim, scale );
		float4x4 scaleMatrix = float4x4::Scale(scale );

		// combo time
		nodeTransform = positionMatrix * rotationMatrix * scaleMatrix;
	}

	float4x4 objectBoneTransform = parentTransform * nodeTransform;

	std::map<std::string, cMesh::cBone*>::iterator itBone = pMesh->mNameToBone.find( pNode->name );
	if ( itBone != pMesh->mNameToBone.end() )
	{
		itBone->second->objectBoneTransformation = objectBoneTransform;
		itBone->second->finalTransformation =
			this->mGlobalInverseTransform
			* objectBoneTransform
			* itBone->second->transform;
	}

	for ( size_t childIndex = 0; childIndex < pNode->numChildren; childIndex++ )
	{
		this->mReadNodeHeirarchy( animationTime, pAnimation, pNode->children[ childIndex ], objectBoneTransform );
	}

}

void cScene::mInterpolateRotation( float animationTime, 
	cAnimation::cBoneAnimation* pNode, Quaternion& rotationOut )
{
	if ( pNode->numRotations == 1 )
	{
		rotationOut.q.x = pNode->rotationKeys[ 0 ]->data.q.x;
		rotationOut.q.y = pNode->rotationKeys[ 0 ]->data.q.y;
		rotationOut.q.z = pNode->rotationKeys[ 0 ]->data.q.z;
		rotationOut.q.w = pNode->rotationKeys[ 0 ]->data.q.w;
		return;
	}

	unsigned int rotationIndex = this->mFindRotation( animationTime, pNode );
	unsigned int nextRotationIndex = rotationIndex + 1;

	float dt = pNode->rotationKeys[ nextRotationIndex ]->time - pNode->rotationKeys[ rotationIndex ]->time;
	float factor = ( animationTime - pNode->rotationKeys[ rotationIndex ]->time ) / dt;
	if ( factor < 0.0f ) factor = 0.0f;
	if ( factor > 1.0f ) factor = 1.0f;

	Quaternion rotationStart = pNode->rotationKeys[ rotationIndex ]->data;
	Quaternion rotationEnd = pNode->rotationKeys[ nextRotationIndex ]->data;

	rotationOut = slerp( rotationStart, rotationEnd, factor );
	rotationOut = normalize( rotationOut );

}

void cScene::mInterpolatePosition( float animationTime,
	cAnimation::cBoneAnimation* pNode, float3& positionOut )
{
	if ( pNode->numPositions == 1 )
	{
		positionOut.x = pNode->positionKeys[ 0 ]->data.x;
		positionOut.y = pNode->positionKeys[ 0 ]->data.y;
		positionOut.z = pNode->positionKeys[ 0 ]->data.z;
		return;
	}

	unsigned int positionIndex = this->mFindPosition( animationTime, pNode );
	unsigned int nextPositionIndex = positionIndex + 1;

	float dt = pNode->positionKeys[ nextPositionIndex ]->time - pNode->positionKeys[ positionIndex ]->time;
	float factor = ( animationTime - pNode->positionKeys[ positionIndex ]->time ) / dt;
	if ( factor < 0.0f ) factor = 0.0f;
	if ( factor > 1.0f ) factor = 1.0f;

	float3 start = pNode->positionKeys[ positionIndex ]->data;
	float3 end = pNode->positionKeys[ nextPositionIndex ]->data;

	positionOut = ( end - start ) * factor + start;

}

void cScene::mInterpolateScale( float animationTime,
	cAnimation::cBoneAnimation* pNode, float3& scaleOut )
{
	if ( pNode->numScales == 1 )
	{
		scaleOut.x = pNode->scalingKeys[ 0 ]->data.x;
		scaleOut.y = pNode->scalingKeys[ 0 ]->data.y;
		scaleOut.z = pNode->scalingKeys[ 0 ]->data.z;
		return;
	}

	unsigned int scaleIndex = this->mFindScaling( animationTime, pNode );
	unsigned int nextScaleIndex = scaleIndex + 1;

	float dt = pNode->scalingKeys[ nextScaleIndex ]->time - pNode->scalingKeys[ scaleIndex ]->time;
	float factor = ( animationTime - pNode->scalingKeys[ scaleIndex ]->time ) / dt;
	if ( factor < 0.0f ) factor = 0.0f;
	if ( factor > 1.0f ) factor = 1.0f;

	float3 start = pNode->scalingKeys[ scaleIndex ]->data;
	float3 end = pNode->scalingKeys[ nextScaleIndex ]->data;

	scaleOut = ( end - start ) * factor + start;

}


unsigned int cScene::mFindRotation( float animationTime, cAnimation::cBoneAnimation* pNode )
{
	for ( unsigned int index = 0; index < pNode->numRotations - 1; index++ )
	{
		if ( animationTime < pNode->rotationKeys[ index + 1 ]->time )
		{
			return index;
		}
	}

	return 0;
}

unsigned int cScene::mFindPosition( float animationTime, cAnimation::cBoneAnimation* pNode )
{
	for ( unsigned int index = 0; index < pNode->numPositions - 1; index++ )
	{
		if ( animationTime < pNode->positionKeys[ index + 1 ]->time )
		{
			return index;
		}
	}

	return 0;
}

unsigned int cScene::mFindScaling( float animationTime, cAnimation::cBoneAnimation* pNode )
{
	for ( unsigned int index = 0; index < pNode->numScales - 1; index++ )
	{
		if ( animationTime < pNode->scalingKeys[ index + 1 ]->time )
		{
			return index;
		}
	}

	return 0;
}


// ** cAnimation **
cScene::cAnimation::cBoneAnimation* cScene::cAnimation::findBoneAnimationByName( std::string& boneName )
{
	std::map<std::string, cBoneAnimation*>::iterator itBoneAnim = this->mapBoneAnimations.find( boneName );
	if ( itBoneAnim == this->mapBoneAnimations.end() )
		return NULL;
	return itBoneAnim->second;
}



//bool LoadCharacter()
//{
//	Assimp::Importer* mImporter = new Assimp::Importer;
//	std::string fileAndPath = "media/models/RPG-Character.fbx";
//
//	const aiScene* pAiScene = mImporter->ReadFile(fileAndPath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
//	std::string assimpError = mImporter->GetErrorString();
//	assert(pAiScene);
//	cScene* pScene = new cScene();
//	pScene->name = "RPG-Character";
//
//	// ***********
//	// ** Nodes **
//	// ***********
//	{
//		//aiNode* pRootAiNode = pAiScene->mRootNode;
//		//cScene::cNode* pRootNode = new cScene::cNode();
//		//pScene->mRootNode = pRootNode;
//
//		//pRootNode->name = pRootAiNode->mName.C_Str();
//		//pRootNode->numChildren = pRootAiNode->mNumChildren;
//		//pRootNode->transform = pRootAiNode->mTransformation;
//		////pRootNode->boneID = -1; // denotes root
//
//		//pScene->mGlobalInverseTransform = glm::inverse(pRootNode->transform);
//
//		//pScene->mNodes.push_back(pRootNode);
//		//pScene->mNameToNode[pRootNode->name] = pRootNode;
//
//		////pScene->mBones.push_back
//		//mProcessChildNodes(pScene, pRootNode, pRootAiNode);
//	}
//	// ***************
//	// ** End Nodes **
//	// ***************
//
//	// ************
//	// ** Meshes **
//	// ************
//	for (size_t idxMesh = 0; idxMesh < pAiScene->mNumMeshes; idxMesh++)
//	{
//		cScene::cMesh* pMesh = new cScene::cMesh();
//		pMesh->meshName = pAiScene->mMeshes[idxMesh]->mName.C_Str();
//
//		// **************
//		// ** Vertices **
//		// **************
//		bool hasTexCoords = pAiScene->mMeshes[idxMesh]->HasTextureCoords(0);
//		for (size_t idxVertex = 0; idxVertex < pAiScene->mMeshes[idxMesh]->mNumVertices; idxVertex++)
//		{
//			// need to zero out the bone ids and weights.
//			pMesh->boneIDs.push_back(uint4(0,0,0,0)); // though this will actually refer to the root bone when not assgined later, it won't batter because the weight will be 0.0f
//			pMesh->boneWeights.push_back(float4(0, 0, 0, 0));
//
//			pMesh->vertices.push_back(float4(
//				pAiScene->mMeshes[idxMesh]->mVertices[idxVertex].x,
//				pAiScene->mMeshes[idxMesh]->mVertices[idxVertex].y,
//				pAiScene->mMeshes[idxMesh]->mVertices[idxVertex].z,
//				1.0f));
//			pMesh->normals.push_back(float3(
//				pAiScene->mMeshes[idxMesh]->mNormals[idxVertex].x,
//				pAiScene->mMeshes[idxMesh]->mNormals[idxVertex].y,
//				pAiScene->mMeshes[idxMesh]->mNormals[idxVertex].z));
//			if (hasTexCoords)
//			{
//				pMesh->texCoords.push_back(float3(
//					pAiScene->mMeshes[idxMesh]->mTextureCoords[0][idxVertex].x,
//					pAiScene->mMeshes[idxMesh]->mTextureCoords[0][idxVertex].y,
//					pAiScene->mMeshes[idxMesh]->mTextureCoords[0][idxVertex].z));
//			}
//		} // idxVertex
//
//		pMesh->numberOfVertices = pAiScene->mMeshes[idxMesh]->mNumVertices;
//
//		// ******************
//		// ** End Vertices **
//		// ******************
//
//
//		// *************
//		// ** Indices **
//		// *************
//		for (size_t idxIndex = 0; idxIndex < pAiScene->mMeshes[idxMesh]->mNumFaces; idxIndex++)
//		{
//			if (pAiScene->mMeshes[idxMesh]->mFaces->mNumIndices != 3)
//			{
//				// TODO: LastError pattern
//				return false;
//			}
//			pMesh->indices.push_back(uint3(
//				pAiScene->mMeshes[idxMesh]->mFaces[idxIndex].mIndices[0],
//				pAiScene->mMeshes[idxMesh]->mFaces[idxIndex].mIndices[1],
//				pAiScene->mMeshes[idxMesh]->mFaces[idxIndex].mIndices[2]
//			));
//
//		} // idxIndex
//
//		pMesh->numberOfIndices = (pAiScene->mMeshes[idxMesh]->mNumFaces * 3);
//		// *****************
//		// ** End Indices **
//		// *****************
//
//		// ***********
//		// ** Bones **
//		// ***********
//		{
//			pMesh->mNumBones = pAiScene->mMeshes[idxMesh]->mNumBones;
//			// add bones and weights to relevant collections
//			for (size_t idxBone = 0; idxBone < pMesh->mNumBones; idxBone++)
//			{
//				aiBone* pAiBone = pAiScene->mMeshes[idxMesh]->mBones[idxBone];
//				cScene::cMesh::cBone* pBone = new cScene::cMesh::cBone();
//
//				pBone->name = pAiBone->mName.C_Str();
//				pBone->transform = aiMatrix4x4to4x4(pAiBone->mOffsetMatrix);
//
//				pBone->numVertexBoneWeights = pAiBone->mNumWeights;
//				for (size_t idxVertBone = 0; idxVertBone < pAiBone->mNumWeights; idxVertBone++)
//				{
//					cScene::cMesh::cVertexBoneWeight* pVertBone = new cScene::cMesh::cVertexBoneWeight();
//					pVertBone->boneID = idxBone;
//					pVertBone->vertexID = pAiBone->mWeights[idxVertBone].mVertexId;
//					pVertBone->weight = pAiBone->mWeights[idxVertBone].mWeight;
//					if (pVertBone->weight < 0.005)
//						continue;
//					pBone->vertexBoneWeights.push_back(pVertBone);
//					pMesh->vertIDToBoneWeights[pVertBone->vertexID].push_back(pVertBone);
//				}
//
//				pMesh->mBones.push_back(pBone);
//				pMesh->mNameToBone[pBone->name] = pBone;
//			}
//
//
//			// check for child bones
//			for (size_t idxBone = 0; idxBone < pMesh->mNumBones; idxBone++)
//			{
//				cScene::cMesh::cBone* pParentBone = pMesh->mBones[idxBone];
//				std::map<std::string, cScene::cNode*>::iterator itParentNode = pScene->mNameToNode.find(pParentBone->name);
//				if (itParentNode == pScene->mNameToNode.end())
//					return false;
//
//				cScene::cNode* pParentNode = itParentNode->second;
//
//				pParentBone->numChildren = pParentNode->numChildren;
//				for (size_t idxChild = 0; idxChild < pParentBone->numChildren; idxChild++)
//				{
//					cScene::cNode* pChildNode = pParentNode->children[idxChild];
//					std::map<std::string, cScene::cMesh::cBone*>::iterator itChildBone = pMesh->mNameToBone.find(pChildNode->name);
//					if (itChildBone == pMesh->mNameToBone.end())
//						return false;
//
//					itChildBone->second->parent = pParentBone;
//					pParentBone->children.push_back(itChildBone->second);
//				}
//			}
//
//			for (size_t idxBone = 0; idxBone < pMesh->mNumBones; idxBone++)
//			{
//				if (pMesh->mBones[idxBone]->parent == NULL)
//				{
//					pMesh->mRootBones.push_back(pMesh->mBones[idxBone]);
//				}
//			}
//
//			// Handle the bone weighting per-vertex.
//			//	This isn't the classiest way to do it, but it's gonna work for now
//			for (size_t idxVert = 0; idxVert < pMesh->numberOfVertices; idxVert++)
//			{
//
//				std::vector<cScene::cMesh::cVertexBoneWeight*> vVertBones = pMesh->vertIDToBoneWeights[idxVert];
//				for (size_t idxVertBone = 0; idxVertBone < vVertBones.size(); idxVertBone++)
//				{
//					pMesh->boneIDs[idxVert][idxVertBone] = vVertBones[idxVertBone]->boneID;
//					pMesh->boneWeights[idxVert][idxVertBone] = vVertBones[idxVertBone]->weight;
//				}
//			}
//
//		}
//		// ***************
//		// ** End Bones **
//		// ***************
//
//		pScene->meshes.push_back(pMesh);
//
//	}
//	// ****************
//	// ** End Meshes **
//	// ****************
//
//	// ****************
//	// ** Animations **
//	// ****************
//		std::string animFileName = "media\\models\\rpg-character\\RPG-Character@Unarmed-Idle.fbx";
//
//		Assimp::Importer animImp;
//		const aiScene* pAiAnimScene = animImp.ReadFile(animFileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
//
//		cScene::cAnimation* pAnim = new cScene::cAnimation();
//
//		pAnim->name = "idle_animation";
//		pAnim->durationInTicks = 50;
//		pAnim->numBoneChannels = 55;
//		pAnim->ticksPerSecond = 30;
//
//		for (int channelIndex = 0; channelIndex < pAnim->numBoneChannels; channelIndex++)
//		{
//			aiNodeAnim* aiAnimChannel = pAiAnimScene->mAnimations[0]->mChannels[channelIndex];
//			cScene::cAnimation::cBoneAnimation* pBoneAnim = new cScene::cAnimation::cBoneAnimation();
//
//			pBoneAnim->nodeName = aiAnimChannel->mNodeName.C_Str();
//			pBoneAnim->numPositions = aiAnimChannel->mNumPositionKeys;
//			pBoneAnim->numScales = aiAnimChannel->mNumScalingKeys;
//			pBoneAnim->numRotations = aiAnimChannel->mNumRotationKeys;
//			// ** Position keys **
//			for (size_t posIndex = 0; posIndex < aiAnimChannel->mNumPositionKeys; posIndex++)
//			{
//				aiVectorKey aiPosData = aiAnimChannel->mPositionKeys[posIndex];
//				cScene::cAnimation::cBoneAnimation::sV3Data* posData = new cScene::cAnimation::cBoneAnimation::sV3Data();
//				posData->time = aiPosData.mTime;
//				posData->data.x = (false && pBoneAnim->nodeName == "B_Pelvis" ? 0.0f : aiPosData.mValue.x);
//				posData->data.y = aiPosData.mValue.y;
//				posData->data.z = aiPosData.mValue.z;
//
//				pBoneAnim->positionKeys.push_back(posData);
//			}
//
//			// ** Scaling keys **
//			for (size_t scaleIndex = 0; scaleIndex < aiAnimChannel->mNumScalingKeys; scaleIndex++)
//			{
//				aiVectorKey aiScaleData = aiAnimChannel->mScalingKeys[scaleIndex];
//				cScene::cAnimation::cBoneAnimation::sV3Data* scaleData = new cScene::cAnimation::cBoneAnimation::sV3Data();
//				scaleData->time = aiScaleData.mTime;
//				scaleData->data.x = aiScaleData.mValue.x;
//				scaleData->data.y = aiScaleData.mValue.y;
//				scaleData->data.z = aiScaleData.mValue.z;
//
//				pBoneAnim->scalingKeys.push_back(scaleData);
//			}
//
//			// ** Rotation keys **
//			for (size_t rotIndex = 0; rotIndex < aiAnimChannel->mNumRotationKeys; rotIndex++)
//			{
//				aiQuatKey aiRotData = aiAnimChannel->mRotationKeys[rotIndex];
//				cScene::cAnimation::cBoneAnimation::sQData* rotData = new cScene::cAnimation::cBoneAnimation::sQData();
//				rotData->time = aiRotData.mTime;
//				rotData->data.q.x = aiRotData.mValue.x;
//				rotData->data.q.y = aiRotData.mValue.y;
//				rotData->data.q.z = aiRotData.mValue.z;
//				rotData->data.q.w = aiRotData.mValue.w;
//
//				pBoneAnim->rotationKeys.push_back(rotData);
//			}
//			pAnim->boneAnimations.push_back(pBoneAnim);
//			pAnim->mapBoneAnimations[pBoneAnim->nodeName] = pBoneAnim;
//		}
//		
//}