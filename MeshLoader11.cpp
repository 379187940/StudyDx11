//--------------------------------------------------------------------------------------
// File: MeshLoader10.cpp
//
// Wrapper class for ID3DX10Mesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma warning(disable: 4995)
#include"unit.h"
#include "meshloader11.h"
#include <fstream>
#include "Common.h"
using namespace std;



// Define the input layout
const D3D11_INPUT_ELEMENT_DESC layout_CMeshLoader11[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
} ;
UINT numElements_layout_CMeshLoader11 = sizeof( layout_CMeshLoader11 ) / sizeof( layout_CMeshLoader11[0] );


//--------------------------------------------------------------------------------------
CMeshLoader11::CMeshLoader11()
{
    m_pd3dDevice = NULL;

    m_NumAttribTableEntries = 0;

    ZeroMemory( m_strMediaDir, sizeof( m_strMediaDir ) );
}


//--------------------------------------------------------------------------------------
CMeshLoader11::~CMeshLoader11()
{
    Destroy();
}


//--------------------------------------------------------------------------------------
void CMeshLoader11::Destroy()
{
    for ( int iMaterial = 0; iMaterial < m_Materials.size(); ++iMaterial )
    {
        Material *pMaterial = m_Materials[iMaterial];

        if ( pMaterial->pTextureRV10 && !IsErrorResource(pMaterial->pTextureRV10) )
        {
            ID3D11Resource* pRes = NULL;
            
            pMaterial->pTextureRV10->GetResource( &pRes );
            pRes->Release();
			pRes->Release();   // do this twice, because GetResource adds a ref

			pMaterial->pTextureRV10->Release();
        }
		delete pMaterial;
		pMaterial = NULL;
    }

    m_Materials.clear();
    m_Vertices.clear();
    m_Indices.clear();
    m_Attributes.clear();

    //SAFE_DELETE_ARRAY( m_pAttribTable );
    m_NumAttribTableEntries = 0;

    //SAFE_RELEASE( m_pMesh );
    m_pd3dDevice = NULL;
}


//--------------------------------------------------------------------------------------
HRESULT CMeshLoader11::Create( ID3D11Device* pd3dDevice, const WCHAR* strFilename )
{
    HRESULT hr;
    WCHAR str[ MAX_PATH ] = {0};

    // Start clean
    Destroy();

    // Store the device pointer
    m_pd3dDevice = pd3dDevice;

    // Load the vertex buffer, index buffer, and subset information from a file. In this case, 
    // an .obj file was chosen for simplicity, but it's meant to illustrate that ID3DXMesh objects
    // can be filled from any mesh file format once the necessary data is extracted from file.
    V_RETURN( LoadGeometryFromOBJ( strFilename ) );

    // Set the current directory based on where the mesh was found
    WCHAR wstrOldDir[MAX_PATH] = {0};
    GetCurrentDirectory( MAX_PATH, wstrOldDir );
    SetCurrentDirectory( m_strMediaDir );    

    //// Load material textures
    //for ( int iMaterial = 0; iMaterial < m_Materials.size(); ++iMaterial )
    //{
    //    Material *pMaterial = m_Materials[ iMaterial ];
    //    if ( pMaterial->strTexture[0] )
    //    {            
    //        pMaterial->pTextureRV10 = (ID3D10ShaderResourceView*)ERROR_RESOURCE_VALUE;

    //        if ( SUCCEEDED(DXUTFindDXSDKMediaFileCch( str, MAX_PATH, pMaterial->strTexture) ) )
    //        {
    //            DXUTGetGlobalResourceCache().CreateTextureFromFile( pd3dDevice, str, 
    //                &pMaterial->pTextureRV10, false ) ;
    //        }
    //    }
    //}

    //// Restore the original current directory
    //SetCurrentDirectory( wstrOldDir );

    //// Create the encapsulated mesh
    //ID3DX10Mesh *pMesh = NULL;

    //V_RETURN( D3DX10CreateMesh( pd3dDevice,
    //                            layout_CMeshLoader10,
    //                            numElements_layout_CMeshLoader10,
    //                            layout_CMeshLoader10[0].SemanticName,
    //                            m_Vertices.size(),
    //                            m_Indices.size() / 3,
    //                            D3DX10_MESH_32_BIT,
    //                            &pMesh ) );

    //// Set the vertex data
    //pMesh->SetVertexData( 0, (void*)m_Vertices.GetData() );
    //m_Vertices.clear();

    //// Set the index data
    //pMesh->SetIndexData( (void*)m_Indices.GetData(), m_Indices.size() );
    //m_Indices.clear();

    //// Set the attribute data
    //pMesh->SetAttributeData( (UINT*)m_Attributes.GetData() );
    //m_Attributes.clear();

    //// Reorder the vertices according to subset and optimize the mesh for this graphics 
    //// card's vertex cache. When rendering the mesh's triangle list the vertices will 
    //// cache hit more often so it won't have to re-execute the vertex shader.
    //V( pMesh->GenerateAdjacencyAndPointReps( 1e-6f ) );
    //V( pMesh->Optimize( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, NULL, NULL ) );

    //pMesh->GetAttributeTable( NULL, &m_NumAttribTableEntries );
    //m_pAttribTable = new D3DX10_ATTRIBUTE_RANGE[m_NumAttribTableEntries];
    //pMesh->GetAttributeTable( m_pAttribTable, &m_NumAttribTableEntries );

    //V( pMesh->CommitToDevice() );
    //
    //m_pMesh = pMesh;

    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMeshLoader11::LoadGeometryFromOBJ( const WCHAR* strFileName )
{
    WCHAR strMaterialFilename[MAX_PATH] = {0};

    // Create temporary storage for the input data. Once the data has been loaded into
    // a reasonable format we can create a D3DXMesh object and load it with the mesh data.
    vector<XMFLOAT3> Positions;
	vector<XMFLOAT2> TexCoords;
	vector<XMFLOAT3> Normals;

    // The first subset uses the default material
    Material* pMaterial = new Material();
    if( pMaterial == NULL )
        return E_OUTOFMEMORY;

    InitMaterial( pMaterial );
    wcscpy_s( pMaterial->strName, MAX_PATH - 1, L"default" );
    m_Materials.push_back( pMaterial );

    DWORD dwCurSubset = 0;

    // File input
    WCHAR strCommand[256] = {0};
    wifstream InFile(strFileName);
    if( !InFile )
        return S_FALSE;

    for(; ; )
    {
        InFile >> strCommand;
        if( !InFile )
            break;

        if( 0 == wcscmp( strCommand, L"#" ) )
        {
            // Comment
        }
        else if( 0 == wcscmp( strCommand, L"v" ) )
        {
            // Vertex Position
            float x, y, z;
            InFile >> x >> y >> z;
            Positions.push_back( XMFLOAT3( x, y, z ) );
        }
        else if( 0 == wcscmp( strCommand, L"vt" ) )
        {
            // Vertex TexCoord
            float u, v;
            InFile >> u >> v;
            TexCoords.push_back( XMFLOAT2( u, v ) );
        }
        else if( 0 == wcscmp( strCommand, L"vn" ) )
        {
            // Vertex Normal
            float x, y, z;
            InFile >> x >> y >> z;
            Normals.push_back( XMFLOAT3( x, y, z ) );
        }
        else if( 0 == wcscmp( strCommand, L"f" ) )
        {
            // Face
            UINT iPosition, iTexCoord, iNormal;
            VERTEX vertex;

            for( UINT iFace = 0; iFace < 3; iFace++ )
            {
                ZeroMemory( &vertex, sizeof( VERTEX ) );

                // OBJ format uses 1-based arrays
                InFile >> iPosition;
                vertex.position = Positions[ iPosition - 1 ];

                if( '/' == InFile.peek() )
                {
                    InFile.ignore();

                    if( '/' != InFile.peek() )
                    {
                        // Optional texture coordinate
                        InFile >> iTexCoord;
                        vertex.texcoord = TexCoords[ iTexCoord - 1 ];
                    }

                    if( '/' == InFile.peek() )
                    {
                        InFile.ignore();

                        // Optional vertex normal
                        InFile >> iNormal;
                        vertex.normal = Normals[ iNormal - 1 ];
                    }
                }

                // If a duplicate vertex doesn't exist, add this vertex to the Vertices
                // list. Store the index in the Indices array. The Vertices and Indices
                // lists will eventually become the Vertex Buffer and Index Buffer for
                // the mesh.
                DWORD index = AddVertex( iPosition, &vertex );
                if ( index == (DWORD)-1 )
                   return E_OUTOFMEMORY;

                m_Indices.push_back( index );
            }
            m_Attributes.push_back( dwCurSubset );
        }
        else if( 0 == wcscmp( strCommand, L"mtllib" ) )
        {
            // Material library
            InFile >> strMaterialFilename;
        }
        else if( 0 == wcscmp( strCommand, L"usemtl" ) )
        {
            // Material
            WCHAR strName[MAX_PATH] = {0};
            InFile >> strName;

            bool bFound = false;
            for( int iMaterial = 0; iMaterial < m_Materials.size(); iMaterial++ )
            {
                Material* pCurMaterial = m_Materials[ iMaterial ];
                if( 0 == wcscmp( pCurMaterial->strName, strName ) )
                {
                    bFound = true;
                    dwCurSubset = iMaterial;
                    break;
                }
            }

            if( !bFound )
            {
                pMaterial = new Material();
                if( pMaterial == NULL )
                    return E_OUTOFMEMORY;

                dwCurSubset = m_Materials.size();

                InitMaterial( pMaterial );
                wcscpy_s( pMaterial->strName, MAX_PATH - 1, strName );

                m_Materials.push_back( pMaterial );
            }
        }
        else
        {
            // Unimplemented or unrecognized command
        }

        InFile.ignore( 1000, '\n' );
    }

    // Cleanup
    InFile.close();
    DeleteCache();

    // If an associated material file was found, read that in as well.
    if( strMaterialFilename[0] )
    {
		CString fullPathMaterialFileName;
		GetFullPath(strMaterialFilename, fullPathMaterialFileName);
        V_RETURN( LoadMaterialsFromMTL( fullPathMaterialFileName) );
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
DWORD CMeshLoader11::AddVertex( UINT hash, VERTEX* pVertex )
{
    // If this vertex doesn't already exist in the Vertices list, create a new entry.
    // Add the index of the vertex to the Indices list.
    bool bFoundInList = false;
    DWORD index = 0;

    // Since it's very slow to check every element in the vertex list, a hashtable stores
    // vertex indices according to the vertex position's index as reported by the OBJ file
    if( ( UINT )m_VertexCache.size() > hash )
    {
        CacheEntry* pEntry = m_VertexCache[hash];
        while( pEntry != NULL )
        {
            VERTEX* pCacheVertex = &m_Vertices[pEntry->index];

            // If this vertex is identical to the vertex already in the list, simply
            // point the index buffer to the existing vertex
            if( 0 == memcmp( pVertex, pCacheVertex, sizeof( VERTEX ) ) )
            {
                bFoundInList = true;
                index = pEntry->index;
                break;
            }

            pEntry = pEntry->pNext;
        }
    }

    // Vertex was not found in the list. Create a new entry, both within the Vertices list
    // and also within the hashtable cache
    if( !bFoundInList )
    {
        // Add to the Vertices list
        index = m_Vertices.size();
        m_Vertices.push_back( *pVertex );

        // Add this to the hashtable
        CacheEntry* pNewEntry = new CacheEntry;
        if( pNewEntry == NULL )
            return (DWORD)-1;

        pNewEntry->index = index;
        pNewEntry->pNext = NULL;

        // Grow the cache if needed
        while( ( UINT )m_VertexCache.size() <= hash )
        {
            m_VertexCache.push_back( NULL );
        }

        // Add to the end of the linked list
        CacheEntry* pCurEntry = m_VertexCache[hash];
        if( pCurEntry == NULL )
        {
            // This is the head element
            m_VertexCache[hash] = pNewEntry ;
        }
        else
        {
            // Find the tail
            while( pCurEntry->pNext != NULL )
            {
                pCurEntry = pCurEntry->pNext;
            }

            pCurEntry->pNext = pNewEntry;
        }
    }

    return index;
}


//--------------------------------------------------------------------------------------
void CMeshLoader11::DeleteCache()
{
    // Iterate through all the elements in the cache and subsequent linked lists
    for( int i = 0; i < m_VertexCache.size(); i++ )
    {
        CacheEntry* pEntry = m_VertexCache[i];
        while( pEntry != NULL )
        {
            CacheEntry* pNext = pEntry->pNext;
            SAFE_DELETE( pEntry );
            pEntry = pNext;
        }
    }

    m_VertexCache.clear();
}


//--------------------------------------------------------------------------------------
HRESULT CMeshLoader11::LoadMaterialsFromMTL( const WCHAR* strFileName )
{
    HRESULT hr;
    // File input
    WCHAR strCommand[256] = {0};
    wifstream InFile( strFileName );
    if( !InFile )
        return S_FALSE;

    // Restore the original current directory
    //SetCurrentDirectory( wstrOldDir );

    Material* pMaterial = NULL;

    for(; ; )
    {
        InFile >> strCommand;
        if( !InFile )
            break;

        if( 0 == wcscmp( strCommand, L"newmtl" ) )
        {
            // Switching active materials
            WCHAR strName[MAX_PATH] = {0};
            InFile >> strName;

            pMaterial = NULL;
            for( int i = 0; i < m_Materials.size(); i++ )
            {
                Material* pCurMaterial = m_Materials[ i ];
                if( 0 == wcscmp( pCurMaterial->strName, strName ) )
                {
                    pMaterial = pCurMaterial;
                    break;
                }
            }
        }

        // The rest of the commands rely on an active material
        if( pMaterial == NULL )
            continue;

        if( 0 == wcscmp( strCommand, L"#" ) )
        {
            // Comment
        }
        else if( 0 == wcscmp( strCommand, L"Ka" ) )
        {
            // Ambient color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vAmbient = XMFLOAT3( r, g, b );
        }
        else if( 0 == wcscmp( strCommand, L"Kd" ) )
        {
            // Diffuse color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vDiffuse = XMFLOAT3( r, g, b );
        }
        else if( 0 == wcscmp( strCommand, L"Ks" ) )
        {
            // Specular color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vSpecular = XMFLOAT3( r, g, b );
        }
        else if( 0 == wcscmp( strCommand, L"d" ) ||
                 0 == wcscmp( strCommand, L"Tr" ) )
        {
            // Alpha
            InFile >> pMaterial->fAlpha;
        }
        else if( 0 == wcscmp( strCommand, L"Ns" ) )
        {
            // Shininess
            int nShininess;
            InFile >> nShininess;
            pMaterial->nShininess = nShininess;
        }
        else if( 0 == wcscmp( strCommand, L"illum" ) )
        {
            // Specular on/off
            int illumination;
            InFile >> illumination;
            pMaterial->bSpecular = ( illumination == 2 );
        }
        else if( 0 == wcscmp( strCommand, L"map_Kd" ) )
        {
            // Texture
            InFile >> pMaterial->strTexture;
        }

        else
        {
            // Unimplemented or unrecognized command
        }

        InFile.ignore( 1000, L'\n' );
    }

    InFile.close();

    return S_OK;
}


//--------------------------------------------------------------------------------------
void CMeshLoader11::InitMaterial( Material* pMaterial )
{
    ZeroMemory( pMaterial, sizeof( Material ) );

    pMaterial->vAmbient = XMFLOAT3( 0.2f, 0.2f, 0.2f );
    pMaterial->vDiffuse = XMFLOAT3( 0.8f, 0.8f, 0.8f );
    pMaterial->vSpecular = XMFLOAT3( 1.0f, 1.0f, 1.0f );
    pMaterial->nShininess = 0;
    pMaterial->fAlpha = 1.0f;
    pMaterial->bSpecular = false;
    pMaterial->pTextureRV10 = NULL;
}
