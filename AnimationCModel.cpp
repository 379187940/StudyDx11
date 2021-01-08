#include "AnimationCModel.h"
#include "unit.h"
#include "scene.h"
using namespace Diligent;
// ** cAnimation **
CAnimationCModel::cAnimation::cBoneAnimation* CAnimationCModel::cAnimation::findBoneAnimationByName(std::string& boneName)
{
	std::map<std::string, cBoneAnimation*>::iterator itBoneAnim = this->mapBoneAnimations.find(boneName);
	if (itBoneAnim == this->mapBoneAnimations.end())
		return NULL;
	
	return itBoneAnim->second;
}
CAnimationCModel::cMesh::cMesh() :
	meshName(""), numberOfIndices(0), numberOfVertices(0),
	mNumBones(0),
	mNumRootBones(0)
{
}

CAnimationCModel::cMesh::~cMesh()
{
}

CAnimationCModel::CAnimationCModel()
{
}


CAnimationCModel::~CAnimationCModel()
{
	std::map < cMesh::cBone*, CClinder* >::iterator itClinder;
	for (itClinder = m_Bone_Clinder.begin(); itClinder != m_Bone_Clinder.end(); itClinder++)
	{
		CClinder*& pClinder = itClinder->second;
		AfxGetScene()->UnRegisterObject(pClinder);
		delete pClinder;
		pClinder = NULL;
	}
}
static float4x4 aiMatrix4x4to4x4(aiMatrix4x4 matrix4x4)
{
	return float4x4(
		matrix4x4.a1, matrix4x4.b1, matrix4x4.c1, matrix4x4.d1,
		matrix4x4.a2, matrix4x4.b2, matrix4x4.c2, matrix4x4.d2,
		matrix4x4.a3, matrix4x4.b3, matrix4x4.c3, matrix4x4.d3,
		matrix4x4.a4, matrix4x4.b4, matrix4x4.c4, matrix4x4.d4
	);
}
static bool mProcessChildNodes(CAnimationCModel* pModel, CAnimationCModel::cNode* parentnode, aiNode* parentainode)
{
	for (size_t index = 0; index < parentainode->mNumChildren; index++)
	{
		aiNode* childainode = parentainode->mChildren[index];
		CAnimationCModel::cNode* childnode = new CAnimationCModel::cNode();

		parentnode->children.push_back(childnode);

		childnode->parent = parentnode;
		childnode->name = childainode->mName.C_Str();
		childnode->transform = aiMatrix4x4to4x4(childainode->mTransformation);
		childnode->numChildren = childainode->mNumChildren;

		pModel->m_Nodes.push_back(childnode);
		pModel->m_NameToNode[childnode->name] = childnode;

		if (childnode->numChildren > 0)
		{
			mProcessChildNodes(pModel, childnode, childainode);
		}

	}
	return true;
}
bool CAnimationCModel::LoadCharacter(std::string strSkin, vector<std::string>& action)
{
	Assimp::Importer* mImporter = new Assimp::Importer;

	const aiScene* pAiScene = mImporter->ReadFile(strSkin.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	std::string assimpError = mImporter->GetErrorString();
	assert(pAiScene);

	// ***********
	// ** Nodes **
	// ***********
	{
		aiNode* pRootAiNode = pAiScene->mRootNode;
		cNode* pRootNode = new cNode();
		m_RootNode = pRootNode;

		pRootNode->name = pRootAiNode->mName.C_Str();
		pRootNode->numChildren = pRootAiNode->mNumChildren;
		pRootNode->transform = aiMatrix4x4to4x4(pRootAiNode->mTransformation);
		//pRootNode->boneID = -1; // denotes root

		m_GlobalInverseTransform = pRootNode->transform.Inverse();

		m_Nodes.push_back(pRootNode);
		m_NameToNode[pRootNode->name] = pRootNode;

		//pScene->mBones.push_back
		mProcessChildNodes(this, pRootNode, pRootAiNode);
	}
	// ***************
	// ** End Nodes **
	// ***************

	// ************
	// ** Meshes **
	// ************
	for (size_t idxMesh = 0; idxMesh < pAiScene->mNumMeshes; idxMesh++)
	{
		cMesh* pMesh = new cMesh();
		pMesh->meshName = pAiScene->mMeshes[idxMesh]->mName.C_Str();

		// **************
		// ** Vertices **
		// **************
		bool hasTexCoords = pAiScene->mMeshes[idxMesh]->HasTextureCoords(0);
		for (size_t idxVertex = 0; idxVertex < pAiScene->mMeshes[idxMesh]->mNumVertices; idxVertex++)
		{
			// need to zero out the bone ids and weights.
			pMesh->boneIDs.push_back(uint4(0, 0, 0, 0)); // though this will actually refer to the root bone when not assgined later, it won't batter because the weight will be 0.0f
			pMesh->boneWeights.push_back(float4(0, 0, 0, 0));

			pMesh->vertices.push_back(float4(
				pAiScene->mMeshes[idxMesh]->mVertices[idxVertex].x,
				pAiScene->mMeshes[idxMesh]->mVertices[idxVertex].y,
				pAiScene->mMeshes[idxMesh]->mVertices[idxVertex].z,
				1.0f));
			pMesh->normals.push_back(float3(
				pAiScene->mMeshes[idxMesh]->mNormals[idxVertex].x,
				pAiScene->mMeshes[idxMesh]->mNormals[idxVertex].y,
				pAiScene->mMeshes[idxMesh]->mNormals[idxVertex].z));
			if (hasTexCoords)
			{
				pMesh->texCoords.push_back(float3(
					pAiScene->mMeshes[idxMesh]->mTextureCoords[0][idxVertex].x,
					pAiScene->mMeshes[idxMesh]->mTextureCoords[0][idxVertex].y,
					pAiScene->mMeshes[idxMesh]->mTextureCoords[0][idxVertex].z));
			}
		} // idxVertex

		pMesh->numberOfVertices = pAiScene->mMeshes[idxMesh]->mNumVertices;

		// ******************
		// ** End Vertices **
		// ******************


		// *************
		// ** Indices **
		// *************
		for (size_t idxIndex = 0; idxIndex < pAiScene->mMeshes[idxMesh]->mNumFaces; idxIndex++)
		{
			if (pAiScene->mMeshes[idxMesh]->mFaces->mNumIndices != 3)
			{
				// TODO: LastError pattern
				return false;
			}
			pMesh->indices.push_back(uint3(
				pAiScene->mMeshes[idxMesh]->mFaces[idxIndex].mIndices[0],
				pAiScene->mMeshes[idxMesh]->mFaces[idxIndex].mIndices[1],
				pAiScene->mMeshes[idxMesh]->mFaces[idxIndex].mIndices[2]
			));

		} // idxIndex

		pMesh->numberOfIndices = (pAiScene->mMeshes[idxMesh]->mNumFaces * 3);
		// *****************
		// ** End Indices **
		// *****************

		// ***********
		// ** Bones **
		// ***********
		{
			pMesh->mNumBones = pAiScene->mMeshes[idxMesh]->mNumBones;
			// add bones and weights to relevant collections
			for (size_t idxBone = 0; idxBone < pMesh->mNumBones; idxBone++)
			{
				aiBone* pAiBone = pAiScene->mMeshes[idxMesh]->mBones[idxBone];
				cMesh::cBone* pBone = new cMesh::cBone();

				pBone->name = pAiBone->mName.C_Str();
				pBone->transform = aiMatrix4x4to4x4(pAiBone->mOffsetMatrix);

				pBone->numVertexBoneWeights = pAiBone->mNumWeights;
				for (size_t idxVertBone = 0; idxVertBone < pAiBone->mNumWeights; idxVertBone++)
				{
					cMesh::cVertexBoneWeight* pVertBone = new cMesh::cVertexBoneWeight();
					pVertBone->boneID = idxBone;
					pVertBone->vertexID = pAiBone->mWeights[idxVertBone].mVertexId;
					pVertBone->weight = pAiBone->mWeights[idxVertBone].mWeight;
					if (pVertBone->weight < 0.005)
						continue;
					pBone->vertexBoneWeights.push_back(pVertBone);
					pMesh->vertIDToBoneWeights[pVertBone->vertexID].push_back(pVertBone);
				}

				pMesh->mBones.push_back(pBone);
				pMesh->mNameToBone[pBone->name] = pBone;
			}


			// check for child bones
			for (size_t idxBone = 0; idxBone < pMesh->mNumBones; idxBone++)
			{
				cMesh::cBone* pParentBone = pMesh->mBones[idxBone];
				std::map<std::string, cNode*>::iterator itParentNode = m_NameToNode.find(pParentBone->name);
				if (itParentNode == m_NameToNode.end())
					return false;

				cNode* pParentNode = itParentNode->second;

				pParentBone->numChildren = pParentNode->numChildren;
				for (size_t idxChild = 0; idxChild < pParentBone->numChildren; idxChild++)
				{
					cNode* pChildNode = pParentNode->children[idxChild];
					std::map<std::string, cMesh::cBone*>::iterator itChildBone = pMesh->mNameToBone.find(pChildNode->name);
					if (itChildBone == pMesh->mNameToBone.end())
						return false;

					itChildBone->second->parent = pParentBone;
					pParentBone->children.push_back(itChildBone->second);
				}
			}

			for (size_t idxBone = 0; idxBone < pMesh->mNumBones; idxBone++)
			{
				if (pMesh->mBones[idxBone]->parent == NULL)
				{
					pMesh->mRootBones.push_back(pMesh->mBones[idxBone]);
				}
			}

			// Handle the bone weighting per-vertex.
			//	This isn't the classiest way to do it, but it's gonna work for now
			for (size_t idxVert = 0; idxVert < pMesh->numberOfVertices; idxVert++)
			{

				std::vector<cMesh::cVertexBoneWeight*> vVertBones = pMesh->vertIDToBoneWeights[idxVert];
				for (size_t idxVertBone = 0; idxVertBone < vVertBones.size(); idxVertBone++)
				{
					pMesh->boneIDs[idxVert][idxVertBone] = vVertBones[idxVertBone]->boneID;
					pMesh->boneWeights[idxVert][idxVertBone] = vVertBones[idxVertBone]->weight;
				}
			}

		}
		// ***************
		// ** End Bones **
		// ***************

		m_Skin.push_back(pMesh);

	}
	// ****************
	// ** End Meshes **
	// ****************

	// ****************
	// ** Animations **
	// ****************
	std::string animFileName = "media\\models\\rpg-character\\RPG-Character@Unarmed-Idle.fbx";

	
	for (int i = 0; i < action.size(); i++)
	{
		Assimp::Importer animImp;
		const aiScene* pAiAnimScene = animImp.ReadFile(action[i].c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		cAnimation* pAnim = new cAnimation();

		pAnim->name = "idle_animation";
		pAnim->durationInTicks = 50;
		pAnim->numBoneChannels = 55;
		pAnim->ticksPerSecond = 30;

		for (int channelIndex = 0; channelIndex < pAnim->numBoneChannels; channelIndex++)
		{
			aiNodeAnim* aiAnimChannel = pAiAnimScene->mAnimations[0]->mChannels[channelIndex];
			cAnimation::cBoneAnimation* pBoneAnim = new cAnimation::cBoneAnimation();

			pBoneAnim->nodeName = aiAnimChannel->mNodeName.C_Str();
			pBoneAnim->numPositions = aiAnimChannel->mNumPositionKeys;
			pBoneAnim->numScales = aiAnimChannel->mNumScalingKeys;
			pBoneAnim->numRotations = aiAnimChannel->mNumRotationKeys;
			// ** Position keys **
			for (size_t posIndex = 0; posIndex < aiAnimChannel->mNumPositionKeys; posIndex++)
			{
				aiVectorKey aiPosData = aiAnimChannel->mPositionKeys[posIndex];
				cAnimation::cBoneAnimation::sV3Data* posData = new cAnimation::cBoneAnimation::sV3Data();
				posData->time = aiPosData.mTime;
				posData->data.x = (false && pBoneAnim->nodeName == "B_Pelvis" ? 0.0f : aiPosData.mValue.x);
				posData->data.y = aiPosData.mValue.y;
				posData->data.z = aiPosData.mValue.z;

				pBoneAnim->positionKeys.push_back(posData);
			}

			// ** Scaling keys **
			for (size_t scaleIndex = 0; scaleIndex < aiAnimChannel->mNumScalingKeys; scaleIndex++)
			{
				aiVectorKey aiScaleData = aiAnimChannel->mScalingKeys[scaleIndex];
				cAnimation::cBoneAnimation::sV3Data* scaleData = new cAnimation::cBoneAnimation::sV3Data();
				scaleData->time = aiScaleData.mTime;
				scaleData->data.x = aiScaleData.mValue.x;
				scaleData->data.y = aiScaleData.mValue.y;
				scaleData->data.z = aiScaleData.mValue.z;

				pBoneAnim->scalingKeys.push_back(scaleData);
			}

			// ** Rotation keys **
			for (size_t rotIndex = 0; rotIndex < aiAnimChannel->mNumRotationKeys; rotIndex++)
			{
				aiQuatKey aiRotData = aiAnimChannel->mRotationKeys[rotIndex];
				cAnimation::cBoneAnimation::sQData* rotData = new cAnimation::cBoneAnimation::sQData();
				rotData->time = aiRotData.mTime;
				rotData->data.q.x = aiRotData.mValue.x;
				rotData->data.q.y = aiRotData.mValue.y;
				rotData->data.q.z = aiRotData.mValue.z;
				rotData->data.q.w = aiRotData.mValue.w;

				pBoneAnim->rotationKeys.push_back(rotData);
			}
			pAnim->boneAnimations.push_back(pBoneAnim);
			pAnim->mapBoneAnimations[pBoneAnim->nodeName] = pBoneAnim;
		}
		m_Actons.push_back(pAnim);
	}
	return true;
}
bool CAnimationCModel::UpdateBoneCClinder(cMesh::cBone* pParentBone)
{
	std::vector<cMesh::cBone*>& children = pParentBone->children;
	float4x4 boneToWorld = pParentBone->transform.Inverse();
	float3 parentPos = float3(boneToWorld.m30, boneToWorld.m31, boneToWorld.m32);
	float2 screenPos = WorldPosToScreenPos(parentPos);
	TextClass *pRenderBonename = NULL;
	if (m_Bone_RenderName.find(pParentBone) == m_Bone_RenderName.end())
	{
		pRenderBonename = new TextClass();
		pRenderBonename->Initialize(AfxGetDevice(), AfxGetDeviceContext(), AfxGetWindowSize().x,
			AfxGetWindowSize().y, 20, false, &AfxGetScene()->GetDefaultFont(), const_cast<char *>(pParentBone->name.c_str()), screenPos.x, screenPos.y, 0, 255, 0);
		m_Bone_RenderName[pParentBone] = pRenderBonename;
	}
	pRenderBonename = m_Bone_RenderName[pParentBone];
	assert(pRenderBonename);
	pRenderBonename->UpdateSentence(AfxGetDeviceContext(), &AfxGetScene()->GetDefaultFont(), const_cast<char *>(pParentBone->name.c_str()), screenPos.x, screenPos.y, 0, 1.0f, 0);
	pRenderBonename->Render(AfxGetDeviceContext(), AfxGetScene()->GetShaderManager(), float4x4::Identity(), float4x4::Identity(), float4x4::Ortho(AfxGetWindowSize().x, AfxGetWindowSize().y, 0.0f, 1.0f, false), AfxGetScene()->GetDefaultFont().GetTexture());
	for (int i = 0; i < children.size(); i++)
	{
		cMesh::cBone* child = children[i];
		boneToWorld = child->transform.Inverse();
		float3 childPos = float3(boneToWorld.m30, boneToWorld.m31, boneToWorld.m32);
		//if not exist create
		if (m_Bone_Clinder.find(child) == m_Bone_Clinder.end())
		{
			USES_CONVERSION;
			const WCHAR* result = A2W(child->name.c_str());
			CClinder* newClinder = new CClinder(result);
			newClinder->Init(AfxGetDevice(), AfxGetDeviceContext());
			m_Bone_Clinder[child] = newClinder;
			AfxGetScene()->RegisterObject(newClinder);
		}
		CClinder* clinder = m_Bone_Clinder[child];
		clinder->UpdateProperty(parentPos, childPos, 50.0, 0.50f, float4(1, 0, 0, 0));
		UpdateBoneCClinder(child);
	}
	return true;
}
void CAnimationCModel::SetPosAndDir(float3 pos, float3 dir)
{
	m_WorldTransform.Identity();
	m_pos = pos;
	m_dir = dir;
	Diligent::normalize(m_dir);
	m_WorldTransform.m31 = pos.x;
	m_WorldTransform.m32 = pos.y;
	m_WorldTransform.m33 = pos.z;
	static float3 unit_yAxis(0, 1, 0);
	float temp = abs(Diligent::dot(m_dir, unit_yAxis));
	//一般人物朝向都是直接
	assert(temp < cos(5.0f*PI / 180.0f));
	float3 right = Diligent::cross()
}
bool CAnimationCModel::RenderBone( )
{
	for (int i = 0; i < m_Skin.size(); i++)
	{
		std::vector<cMesh::cBone*>& rootBone = m_Skin[i]->mRootBones;
		for (int j = 0; j < rootBone.size(); j++)
		{
			UpdateBoneCClinder(rootBone[j]);
		}
	}
	return true;
}