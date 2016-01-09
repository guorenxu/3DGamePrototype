#include "Outlines.h"

#define SafeRelease(p)      { if(p) { (p)->Release(); (p)=NULL; } }

const DWORD MeshVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

Outlines::Outlines(ID3DXMesh* mesh, ID3DXBuffer* adj)
{
	vb = 0;
	ib = 0;
	vertexdecl = 0;
	VertexNumber = 0;
	FaceNumber = 0;

	GenerateOutlineVertices(mesh, adj);
	GenerateOutlineIndices(mesh);
	CreateVertexDeclaration();
}

Outlines::~Outlines()
{
	SafeRelease(vb);
	SafeRelease(ib);
	SafeRelease(vertexdecl);
}

bool Outlines::CreateVertexDeclaration()
{
	HRESULT hr = 0;

	D3DVERTEXELEMENT9 decl[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
		D3DDECL_END()
	};

	hr = d3ddev->CreateVertexDeclaration(decl, &vertexdecl);

	if(FAILED(hr))
	{
		MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
		return false;
	}

	return true;
}

void Outlines::GetFaceNormal(ID3DXMesh* mesh,
				DWORD faceindex,
				D3DXVECTOR3* facenormal)
{
	MeshVertex* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	WORD* in = 0;
	mesh->LockIndexBuffer(0, (void**)&in);

	WORD inA = in[faceindex * 3];
	WORD inB = in[faceindex * 3 + 1];
	WORD inC = in[faceindex * 3 + 2];

	D3DXVECTOR3 vA = v[inA].pos;
	D3DXVECTOR3 vB = v[inB].pos;
	D3DXVECTOR3 vC = v[inC].pos;

	D3DXVECTOR3 edgeA, edgeB;
	edgeA = vB - vA;
	edgeB = vC - vA;
	D3DXVec3Cross(facenormal, &edgeA, &edgeB);
	D3DXVec3Normalize(facenormal, facenormal);

	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void Outlines::GetFaceNormals(ID3DXMesh* mesh,
			ID3DXBuffer* adj,
			D3DXVECTOR3* currentfacenormal,
			D3DXVECTOR3 adjfacenormals[3],
			DWORD faceindex)
{
	MeshVertex* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	WORD* in = 0;
	mesh->LockIndexBuffer(0, (void**)&in);

	DWORD* a = (DWORD*)adj->GetBufferPointer();

	//
	// Get the face normal.
	GetFaceNormal(mesh, faceindex, currentfacenormal);

	//
	// Get adjacent face indices
	WORD faceindexA = WORD(a[faceindex * 3]);
	WORD faceindexB = WORD(a[faceindex * 3 + 1]);
	WORD faceindexC = WORD(a[faceindex * 3 + 2]);

	//
	// Get adjacent face normals, if there is no adjacent face,
	// then set the adjacent face normal to the opposite of the
	// "currentFaceNormal".  Recall we do this because edges that
	// don't have an adjacent triangle are automatically considered
	// silhouette edges.  And in order to make that happen, we need
	// the current face normal and adjacent face normal to point
	// in the opposite direction.  Also, recall that an entry
	// in the adjacency buffer equal to -1 denotes that the edge
	// doesn't have an adjacent triangle.

	D3DXVECTOR3 facenormalA, facenormalB, facenormalC;
    
	if( faceindexA != USHRT_MAX ) // is there an adjacent triangle?
	{
		WORD iA = in[faceindexA * 3];
		WORD iB = in[faceindexA * 3 + 1];
		WORD iC = in[faceindexA * 3 + 2];

		D3DXVECTOR3 vA = v[iA].pos;
		D3DXVECTOR3 vB = v[iB].pos;
		D3DXVECTOR3 vC = v[iC].pos;

		D3DXVECTOR3 edgeA = vB - vA;
		D3DXVECTOR3 edgeB = vC - vA;
		D3DXVec3Cross(&facenormalA, &edgeA, &edgeB);
		D3DXVec3Normalize(&facenormalA, &facenormalA);
	}
	else
	{
		facenormalA = -(*currentfacenormal);
	}

	if( faceindexB != USHRT_MAX ) // is there an adjacent triangle?
	{
		WORD iA = in[faceindexB * 3];
		WORD iB = in[faceindexB * 3 + 1];
		WORD iC = in[faceindexB * 3 + 2];

		D3DXVECTOR3 vA = v[iA].pos;
		D3DXVECTOR3 vB = v[iB].pos;
		D3DXVECTOR3 vC = v[iC].pos;

		D3DXVECTOR3 edgeA = vB - vA;
		D3DXVECTOR3 edgeB = vC - vA;
		D3DXVec3Cross(&facenormalB, &edgeA, &edgeB);
		D3DXVec3Normalize(&facenormalB, &facenormalB);
	}
	else
	{
		facenormalB = -(*currentfacenormal);
	}

	if( faceindexC != USHRT_MAX ) // is there an adjacent triangle?
	{
		WORD iA = in[faceindexC * 3];
		WORD iB = in[faceindexC * 3 + 1];
		WORD iC = in[faceindexC * 3 + 2];

		D3DXVECTOR3 vA = v[iA].pos;
		D3DXVECTOR3 vB = v[iB].pos;
		D3DXVECTOR3 vC = v[iC].pos;

		D3DXVECTOR3 edgeA = vB - vA;
		D3DXVECTOR3 edgeB = vC - vA;
		D3DXVec3Cross(&facenormalC, &edgeA, &edgeB);
		D3DXVec3Normalize(&facenormalC, &facenormalC);
	}
	else
	{
		facenormalC = -(*currentfacenormal);
	}

	// save adjacent face normals
	adjfacenormals[0] = facenormalA;
	adjfacenormals[1] = facenormalB;
	adjfacenormals[2] = facenormalC;

	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void Outlines::GenerateOutlineVertices(ID3DXMesh* mesh,
									  ID3DXBuffer* adj)
{
	// 3 edges per face and 4 vertices per edge
	VertexNumber = mesh->GetNumFaces() * 3 * 4;
	d3ddev->CreateVertexBuffer(
		VertexNumber * sizeof(OutlineVertex),
		D3DUSAGE_WRITEONLY,
		0, // using vertex declaration
		D3DPOOL_MANAGED,
		&vb,
		0);

	MeshVertex* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	WORD* in = 0;
	mesh->LockIndexBuffer(0, (void**)&in);

	OutlineVertex* OutlineVertexBuffer = 0;
	vb->Lock(0, 0, (void**)&OutlineVertexBuffer, 0);

	for(int i = 0; i < mesh->GetNumFaces(); i++)
	{
		D3DXVECTOR3 currentfacenormal;
		D3DXVECTOR3 adjfacenormals[3];

		GetFaceNormals(mesh, adj, &currentfacenormal, adjfacenormals, i);

		// get the indices for this face
		WORD indexA = in[i * 3];
		WORD indexB = in[i * 3 + 1];
		WORD indexC = in[i * 3 + 2];

		// get the vertices for this face
		MeshVertex vA = v[indexA];
		MeshVertex vB = v[indexB];
		MeshVertex vC = v[indexC];
		
		// A        B
		// *--------*
		// |  edge  |
		// *--------*
		// C        D
		// note, C and D are duplicates of A and B respectively, 
		// such that the quad is degenerate.  The vertex shader
		// will un-degenerate the quad if it is a silhouette edge.

		// compute edge0 vA->vB, note adjacent face
		// normal is faceNormal0
		OutlineVertex AA, BA, CA, DA;

		AA.pos = vA.pos;
		AA.norm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		AA.facenormA = currentfacenormal;
		AA.facenormB = adjfacenormals[0];
		BA.pos = vB.pos;
		BA.norm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		BA.facenormA = currentfacenormal;
		BA.facenormB = adjfacenormals[0];

		CA = AA;
		CA.norm = vA.norm;

		DA = BA;
		DA.norm = vB.norm;

		*OutlineVertexBuffer = AA; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = BA; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = CA; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = DA; ++OutlineVertexBuffer;	

		// compute edge0 vB->vC, note adjacent face
		// normal is faceNormal1
		OutlineVertex AB, BB, CB, DB;

		AB.pos = vB.pos;
		AB.norm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		AB.facenormA = currentfacenormal;
		AB.facenormB = adjfacenormals[1];

		BB.pos = vC.pos;
		BB.norm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		BB.facenormA = currentfacenormal;
		BB.facenormB = adjfacenormals[1];

		CB = AB;
		CB.norm = vB.norm;
		
		DB = BB;
		DB.norm = vC.norm;

		*OutlineVertexBuffer = AB; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = BB; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = CB; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = DB; ++OutlineVertexBuffer;	

		// compute edge0 vA->vC, note adjacent face
		// normal is faceNormal2
		OutlineVertex AC, BC, CC, DC;

		AC.pos = vA.pos;
		AC.norm = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		AC.facenormA = currentfacenormal;
		AC.facenormB = adjfacenormals[2];

		BC.pos    = vC.pos;
		BC.norm      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		BC.facenormA = currentfacenormal;
		BC.facenormB = adjfacenormals[2];

		CC = AC;
		CC.norm = vA.norm;
		
		DC = BC;
		DC.norm = vC.norm;

		*OutlineVertexBuffer = AC; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = BC; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = CC; ++OutlineVertexBuffer;
		*OutlineVertexBuffer = DC; ++OutlineVertexBuffer;	
	}

	vb->Unlock();
	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void Outlines::GenerateOutlineIndices(ID3DXMesh* mesh)
{
	DWORD OutlineNumber = mesh->GetNumFaces() * 3;

	FaceNumber = OutlineNumber * 2;
	
	d3ddev->CreateIndexBuffer(
		OutlineNumber * 6 * sizeof(WORD), // 2 triangles per edge
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&ib,
		0);

	WORD* in = 0;

	ib->Lock(0, 0, (void**)&in, 0);

	// 0        1
	// *--------*
	// |  edge  |
	// *--------*
	// 2        3

	for(UINT i = 0; i < OutlineNumber; i++)
	{
		// Six indices to define the triangles of the edge,
		// so every edge we skip six entries in the 
		// index buffer.  Four vertices to define the edge,
		// so every edge we skip four entries in the 
		// vertex buffer.
		in[i * 6]     = i * 4 + 0;
		in[i * 6 + 1] = i * 4 + 1;
		in[i * 6 + 2] = i * 4 + 2;
		in[i * 6 + 3] = i * 4 + 1;
		in[i * 6 + 4] = i * 4 + 3;
		in[i * 6 + 5] = i * 4 + 2;
	}

	ib->Unlock();
}

void Outlines::Draw()
{
	d3ddev->SetVertexDeclaration(vertexdecl);
	d3ddev->SetStreamSource(0, vb, 0, sizeof(OutlineVertex));
	d3ddev->SetIndices(ib);

	d3ddev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, VertexNumber, 0, FaceNumber);
}