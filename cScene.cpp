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
