#include <iostream>
#include <fstream>
#include <iomanip>
#include "Game.h"
#include "CString"
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

#define SafeRelease(p)      { if(p) { (p)->Release(); (p)=NULL; } }

extern D3DPRESENT_PARAMETERS d3dpp;

float ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

int BloomWidth = 512;
int BloomHeight = 256;

int MouseChangeX;
int MouseChangeY;
bool RightClick;
bool LeftClick;
int MouseWheelDelta;
bool DPress;
bool WPress;
bool APress;
bool SPress;

HWND ghwnd;

struct Simple_Vertex
{
	Simple_Vertex(){}

	Simple_Vertex(float X,
		float Y,
		float Z,
		float NX,
		float NY,
		float NZ,
		float U1, 
		float V1)
	{
		x = X;
		y = Y;
		z = Z;
		nx = NX;
		ny = NY;
		nz = NZ;
		u1 = U1;
		v1 = V1;
	}

	float x, y, z;
	float nx, ny, nz;
	float u1, v1;

	static const DWORD FVF;
};

struct Post_Vertex
{
	float x, y, z, w;
	float u, v;

	static const unsigned long FVF;

	Post_Vertex(float X, float Y, float U, float V)
	{
		x  = X;  y  = Y;  z  = 0.0f;  w  = 1.0f;
		u  = U;  v  = V;
	}
};

const DWORD Post_Vertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
const DWORD Simple_Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

float PI = 3.14159265f;

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

const D3DXCOLOR      White( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      Black( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        Red( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      Green( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       Blue( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     Yellow( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       Cyan( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    Magenta( D3DCOLOR_XRGB(255,   0, 255) );

const D3DMATERIAL9 WhiteMaterial  = InitMtrl(White, White, White, Black, 2.0f);
const D3DMATERIAL9 RedMaterial    = InitMtrl(Red, Red, Red, Black, 2.0f);
const D3DMATERIAL9 GreenMaterial  = InitMtrl(Green, Green, Green, Black, 2.0f);
const D3DMATERIAL9 BlueMaterial   = InitMtrl(Blue, Blue, Blue, Black, 2.0f);
const D3DMATERIAL9 YellowMaterial = InitMtrl(Yellow, Yellow, Yellow, Black, 2.0f);

long int Start = GetTickCount();
long int TimeDelta;
int ClickCooldown = 0;

float MoveFactor = 0.04f;

bool DeviceLost = false;

LPD3DXSPRITE SpHd;

Map* Demo = 0;
Efx* Effects = 0;
Outlines* MeshOutlines = 0;

int Test = 0;

LPDIRECT3DSURFACE9 MainSurface = NULL;

LPDIRECT3DSURFACE9 RenderSurface = NULL;
LPDIRECT3DSURFACE9 RenderSurface_Two = NULL;
LPDIRECT3DSURFACE9 RenderSurface_Three = NULL;
LPDIRECT3DTEXTURE9 RenderTexture = NULL;
LPDIRECT3DTEXTURE9 RenderTexture_Two = NULL;
LPDIRECT3DTEXTURE9 RenderTexture_Three = NULL;

LPDIRECT3DTEXTURE9 Cursor = NULL;
LPDIRECT3DTEXTURE9 CursorDrag = NULL;

LPDIRECT3DTEXTURE9 SkyBT = NULL;
LPDIRECT3DTEXTURE9 SkyT = NULL;
LPDIRECT3DTEXTURE9 TexT = NULL;
LPDIRECT3DTEXTURE9 Grass1T = NULL;
LPDIRECT3DTEXTURE9 Grass2T = NULL;
LPDIRECT3DTEXTURE9 Grass3T = NULL;
LPDIRECT3DTEXTURE9 CellT = NULL;
LPDIRECT3DTEXTURE9 Grass2OpacT = NULL;
LPDIRECT3DTEXTURE9 Grass3OpacT = NULL;
LPDIRECT3DTEXTURE9 Dirt1OpacT = NULL;

IDirect3DVertexBuffer9* Quad = 0;

ID3DXMesh* Objects[4] = {0, 0, 0, 0};
D3DMATERIAL9 GroundMaterial = WhiteMaterial;
D3DMATERIAL9 ObjectMaterial = WhiteMaterial;

float Cbx = 0.0f;
float Cby = 0.0f;
float Cbz = 0.0f;

float CameraRadius = 30.0f;
float CameraPhi = 100.0f;
float CameraTheta = 125.0f;

float CameraSavePhi = 0.0f;
float CameraSaveTheta = 0.0f;

float PX = 200.0f;
float PY = 0.0f;
float PZ = 200.0f;

float DestinationX;
float DestinationZ;

float LightX = 0.0f;
float LightY = 10.0f;
float LightZ = 10.0f;

D3DXVECTOR4 CameraPosition;
	
D3DXVECTOR4 LightDirectionFour(LightX, LightY, LightZ, 0.0f);
D3DXVECTOR3 LightDirectionThree(LightX, LightY, LightZ);

D3DXMATRIX Worlds[3];
D3DXMATRIX PillarWorlds[10];
D3DXMATRIX PillarTrans[10];
D3DXMATRIX PillarRotYM[10];
D3DXMATRIX RotationX[2];
D3DXMATRIX RotationY[2];
D3DXMATRIX RotationZ[2];
D3DXMATRIX SkyScale[1];
D3DXMATRIX SkyTrans[1];
D3DXMATRIX Translation[2];
float PillarRotY[10] = {0.1f, 0.6f, 0.8f, 1.3f, 1.7f, 2.7f, 3.1f, 2.2f, 1.6f, 3.4f};
float fRotationX[2] = {0, 0};
float fRotationY[2] = {0, 0};
float fRotationZ[2] = {0, 0};

D3DXMATRIX TestSmallScale;

D3DXMATRIX Proj;

D3DXMATRIX View;

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

void File_Parse(int Vertices,							//Parses a modified OBJ
				int Normals,
				int Texcoords,
				int Faces,
				ID3DXMesh* Mesh,
				char* Name)
{
	std::vector<float> vertices;						//Buffers to hold data
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<int> faces;

	std::vector<int> vlist;
	std::vector<int> nlist;
	std::vector<int> tclist;

	int counter = 0;									//Which number is it on?
	int texcoord_counter = 1;							//Cut off every third number
    float x;											//Infile float buffer

	std::ifstream inFile;
    
    inFile.open(Name);
    if (!inFile)
	{
		MessageBox(0, "File Instream Failed", 0, 0);
	}
    
    while (inFile >> x) 
	{
		if (counter < Vertices * 3)
		{
			vertices.push_back(x);
		}
		else if (counter < Vertices * 3 + Normals * 3)
		{
			normals.push_back(x);
		}
		else if (counter < Vertices * 3 + Normals * 3 + Texcoords * 3)
		{
			if (texcoord_counter % 3 != 0)
			{
				texcoords.push_back(x);
			}

			texcoord_counter++;
		}
		else//if (counter < Vertices * 3 + Normals * 3 + Texcoords * 3 + Faces * 9)
		{
			faces.push_back(int(x));
		}

		counter++;
    }
    
    inFile.close();

	for (int i = 0; i < Faces * 3; i++)					//REMEMBER THAT TC ARE 2ND, NOT 3RD
	{
		vlist.push_back(faces[i * 3] - 1);
		nlist.push_back(faces[i * 3 + 2] - 1);
		tclist.push_back(faces[i * 3 + 1] - 1);
	}

	IDirect3DVertexBuffer9* vertex_buffer = 0;
	Mesh->GetVertexBuffer(&vertex_buffer);

	IDirect3DIndexBuffer9* index_buffer = 0;
	Mesh->GetIndexBuffer(&index_buffer);

	Simple_Vertex* v;

	vertex_buffer->Lock(0, 0, (void**)&v, 0);
	
	for (int i = 0; i < Faces * 3; i++)						//Construct vertex lists
	{
		v[i] = Simple_Vertex(
			vertices[vlist[i] * 3],
			vertices[vlist[i] * 3 + 1],
			vertices[vlist[i] * 3 + 2],
			normals[nlist[i] * 3],
			normals[nlist[i] * 3 + 1],
			normals[nlist[i] * 3 + 2],
			texcoords[tclist[i] * 2],
			texcoords[tclist[i] * 2 + 1]);
	}

	vertex_buffer->Unlock();

	WORD* in = 0;

	index_buffer->Lock(0, 0, (void**)&in, 0);

	for (int i = 0; i < Faces * 3; i++)						//Construct index lists
	{
		in[i] = i;
	}

	index_buffer->Unlock();

	DWORD* a;

	Mesh->LockAttributeBuffer(0, &a);

	for (int w = 0; w < Faces; w++)							//Set Attributes to 0 #
	{
		a[w] = 0;
	}

	Mesh->UnlockAttributeBuffer();

	DWORD* adj = new DWORD[Faces * 3];

	Mesh->GenerateAdjacency(0.001f, adj);

	Mesh->OptimizeInplace(D3DXMESH_MANAGED |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_ATTRSORT | 
		D3DXMESHOPT_VERTEXCACHE,
		adj,
		adj,
		0,
		0);

	SafeRelease(index_buffer);
	SafeRelease(vertex_buffer);
}

void Setup_Mesh()
{
	ID3DXBuffer* adjbuf = 0;

	File_Parse(164, 165, 246, 326, Objects[0], "Stonehenge.txt");
	File_Parse(129, 129, 129, 224, Objects[1], "Skydome.txt");
	File_Parse(129, 129, 129, 224, Objects[2], "Skydome Bottom.txt");

	D3DXCreateTorus(d3ddev, 3, 6, 50, 50, &Objects[3], &adjbuf);

	MeshOutlines = new Outlines(Objects[3], adjbuf);
}

int Game_Init(HWND hwnd)
{
	ghwnd = hwnd;

	HRESULT result;

	//d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	//d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	result = D3DXCreateSprite(d3ddev, &SpHd);					//Create Sprite Handler

	Cursor = LoadTexture("Cursor.tga", White);
	CursorDrag = LoadTexture("Cursor Drag.tga", White);

	SkyT = LoadTexture("Skydome.bmp", NULL);
	SkyBT = LoadTexture("Skydome Bottom.bmp", NULL);

	TexT = LoadTexture("Stonehenge.bmp", NULL);
	Grass1T = LoadTexture("G1.bmp", NULL);
	Grass2T = LoadTexture("G2.bmp", NULL);
	Grass3T = LoadTexture("G3.bmp", NULL);
	CellT = LoadTexture("D1.bmp", NULL);
	Grass2OpacT = LoadTexture("G2Opac.bmp", NULL);
	Grass3OpacT = LoadTexture("G3Opac.bmp", NULL);
	Dirt1OpacT = LoadTexture("D1Opac.bmp", NULL);

	Demo = new Map("Heightmap129.raw",
		129,
		129,
		15,
		0.5f);

	Demo->Generate_Mesh();

	float ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	Proj = SetPerspective(1.0f, ratio, 1.0f, 10000.0f);			//REMEMBER FAR RANGE

	Init_PP();

	Effects = new Efx();
	
	if (!Init_Effects())			//Run Effects->Initialize
	{
		return 0;
	}

	Effects->GetTechniques();		//Get Technique names

	D3DXCreateMeshFVF(326,
		978,
		D3DXMESH_MANAGED,
		Simple_Vertex::FVF,
		d3ddev,
		&Objects[0]);

	D3DXCreateMeshFVF(224,
		672,
		D3DXMESH_MANAGED,
		Simple_Vertex::FVF,
		d3ddev,
		&Objects[1]);

	D3DXCreateMeshFVF(224,
		672,
		D3DXMESH_MANAGED,
		Simple_Vertex::FVF,
		d3ddev,
		&Objects[2]);

	Setup_Mesh();

	DestinationX = PX;
	DestinationZ = PZ;

	return 1;
}

void Init_PP()
{
	//WARNING

	d3ddev->CreateTexture(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_R5G6B5, 
		D3DPOOL_DEFAULT,
		&RenderTexture,
		NULL);

	d3ddev->CreateTexture(BloomWidth,
		BloomHeight,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_R5G6B5, 
		D3DPOOL_DEFAULT,
		&RenderTexture_Two, 
		NULL);

	d3ddev->CreateTexture(BloomWidth,
		BloomHeight, 
		1, 
		D3DUSAGE_RENDERTARGET,
		D3DFMT_R5G6B5, 
		D3DPOOL_DEFAULT, 
		&RenderTexture_Three,
		NULL);

	RenderTexture->GetSurfaceLevel(0, &RenderSurface);
	
	RenderTexture_Two->GetSurfaceLevel(0, &RenderSurface_Two);

	RenderTexture_Three->GetSurfaceLevel(0, &RenderSurface_Three);

	d3ddev->GetRenderTarget(0, &MainSurface);

	d3ddev->CreateVertexBuffer(
	4 * sizeof(Post_Vertex),
	0,
	Post_Vertex::FVF,
	D3DPOOL_MANAGED,
	&Quad,
	0);

	Post_Vertex* VPtr = 0;

	Quad->Lock(0, 0, (void**)&VPtr, 0);

	VPtr[0] = Post_Vertex(0.0f, 0.0f, 0.0f, 0.0f);
	VPtr[1] = Post_Vertex(0.0f, SCREEN_HEIGHT, 0.0f, 1.0f);
	VPtr[2] = Post_Vertex(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f);
	VPtr[3] = Post_Vertex(SCREEN_WIDTH, 0.0f, 1.0f, 0.0f);

	Quad->Unlock();
}

bool Init_Effects()
{
	return Effects->Initialize();
}

void Game_Run(HWND hwnd)
{	
	if (d3ddev == NULL)
	{
		return;
	}

	TimeDelta = GetTickCount() - Start;

	Start = GetTickCount();

	Game_Loop(hwnd);
}

void Game_Loop(HWND hwnd)
{
	if (Test == 0)
	{
		if (CheckLostDevices() == false)
		{
			return;
		}

		Set_Up();

		Game_Calc();

		Camera_Calc();

		Vertex_Draw();

		PP_Prep();

		PP_Effects();

		Pre_Present();

		Mouse();

		Interface();

		Present();
	}

	else if (Test == 1)
	{
		Secondary();
	}
}

void Set_Up()
{
	d3ddev->BeginScene();

	d3ddev->SetRenderTarget(0, RenderSurface);

	d3ddev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
}

void Game_Calc()
{
	D3DXVECTOR3 originW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dirW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 destination;

	GetPickingRay(originW, dirW);
	
	BOOL hit = 0;
	DWORD faceIndex = -1;
	float u = 0.0f;
	float v = 0.0f;
	float dist = 0.0f;
	ID3DXBuffer* allhits = 0;
	DWORD numHits = 0;

	if (LeftClick && ClickCooldown == 0)
	{
		ClickCooldown = 250;

		D3DXIntersect(Demo->mesh, &originW, &dirW, &hit,
				&faceIndex, &u, &v, &dist, &allhits, &numHits);

		//allhits->Release();
		//allhits = 0;

		if(hit)
		{
			float barycentricone;
			float barycentrictwo;

			D3DXVECTOR3 p1 = Demo->vertexdata[Demo->indexdata[faceIndex * 3]].p;
			D3DXVECTOR3 p2 = Demo->vertexdata[Demo->indexdata[faceIndex * 3 + 1]].p;
			D3DXVECTOR3 p3 = Demo->vertexdata[Demo->indexdata[faceIndex * 3 + 2]].p;

			D3DXIntersectTri(&p1,
				&p2,
				&p3,
				&originW, 
				&dirW,
				&barycentricone,
				&barycentrictwo,
				&dist);

			D3DXVec3BaryCentric(&destination, &p1, &p2, &p3, barycentricone, barycentrictwo);

			if (destination.x > float(Demo->length) - 100.0f)
			{
				destination.x = float(Demo->length) - 100.0f;
			}
			else if (destination.x < 100.0f)
			{
				destination.x = 100.0f;
			}

			if (destination.z > float(Demo->width) - 100.0f)
			{
				destination.z = float(Demo->width) - 100.0f;
			}
			else if (destination.z < 100.0f)
			{
				destination.z = 100.0f;
			}

			DestinationX = destination.x;
			DestinationZ = destination.z;
		}
	}
	else if (LeftClick && ClickCooldown != 0)
	{
		ClickCooldown = ClickCooldown - TimeDelta;

		if (ClickCooldown < 0)
		{
			ClickCooldown = 0;
		}
	}
	else if (!LeftClick)
	{
		ClickCooldown = 0;
	}

	D3DXVECTOR3 travel(DestinationX - PX, 0, DestinationZ - PZ);

	if (sqrt((travel.x * travel.x) + (travel.z * travel.z)) < (MoveFactor * TimeDelta))
	{
		PX = DestinationX;
		PZ = DestinationZ;
	}
	else
	{
		D3DXVec3Normalize(&travel, &travel);

		PX = PX + (travel.x * MoveFactor * TimeDelta);
		PZ = PZ + (travel.z * MoveFactor * TimeDelta);
	}
	
	fRotationY[1] = 0.001f * Start;

	PY = Demo->Get_Height(PX, PZ) + 12;

	D3DXMatrixTranslation(&Translation[0], 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&Translation[1], PX, PY, PZ);

	D3DXMatrixRotationY(&RotationY[0], fRotationY[0]);
	D3DXMatrixRotationY(&RotationY[1], fRotationY[1]);

	D3DXMATRIX scale;

	D3DXMatrixScaling(&scale, 0.7f, 0.7f, 0.7f);

	Worlds[0] = RotationY[0] * Translation[0];
	Worlds[1] = scale * RotationY[1] * Translation[1] ;

	D3DXMatrixTranslation(&PillarTrans[0], 800, Demo->Get_Height(800, 200) - 10, 200);
	D3DXMatrixTranslation(&PillarTrans[1], 200, Demo->Get_Height(200, 1000) - 10, 1000);
	D3DXMatrixTranslation(&PillarTrans[2], 1200, Demo->Get_Height(1200, 350) - 10, 350);
	D3DXMatrixTranslation(&PillarTrans[3], 1400, Demo->Get_Height(1400, 1500) - 10, 1500);
	D3DXMatrixTranslation(&PillarTrans[4], 300, Demo->Get_Height(300, 800) - 10, 800);
	D3DXMatrixTranslation(&PillarTrans[5], 1000, Demo->Get_Height(1000, 1650) - 10, 1650);
	D3DXMatrixTranslation(&PillarTrans[6], 1500, Demo->Get_Height(1500, 200) - 10, 200);
	D3DXMatrixTranslation(&PillarTrans[7], 500, Demo->Get_Height(500, 250) - 10, 250);
	D3DXMatrixTranslation(&PillarTrans[8], 1700, Demo->Get_Height(1700, 500) - 10, 500);
	D3DXMatrixTranslation(&PillarTrans[9], 900, Demo->Get_Height(900, 400) - 10, 400);

	D3DXMATRIX PillarScale[11];
	D3DXMatrixScaling(&PillarScale[0], 3.0f, 3.0f, 3.0f);
	D3DXMatrixScaling(&PillarScale[1], 4.0f, 3.0f, 4.0f);
	D3DXMatrixScaling(&PillarScale[2], 2.0f, 3.0f, 2.0f);
	D3DXMatrixScaling(&PillarScale[3], 4.0f, 4.0f, 4.0f);
	D3DXMatrixScaling(&PillarScale[4], 3.0f, 2.0f, 3.0f);
	D3DXMatrixScaling(&PillarScale[5], 3.0f, 3.0f, 3.0f);
	D3DXMatrixScaling(&PillarScale[6], 5.0f, 2.0f, 5.0f);
	D3DXMatrixScaling(&PillarScale[7], 2.0f, 3.0f, 2.0f);
	D3DXMatrixScaling(&PillarScale[8], 5.0f, 5.0f, 5.0f);
	D3DXMatrixScaling(&PillarScale[9], 7.0f, 3.0f, 7.0f);
	D3DXMatrixScaling(&PillarScale[10], 1.0f, 1.0f, 1.0f);


	for (int i = 0; i < 10; i++)
	{
		D3DXMatrixRotationY(&PillarRotYM[i], PillarRotY[i]);
	}

	for (int i = 0; i < 10; i++)
	{
		PillarWorlds[i] = PillarScale[i] * PillarRotYM[i] * PillarTrans[i];
	}

	D3DXMatrixScaling(&SkyScale[0], 1.0f, 1.0f, 1.0f);

	D3DXMatrixScaling(&TestSmallScale, 0.1f, 0.1f, 0.1f);
}

void Camera_Calc()
{
	float x = Cbx;
	float y = Cby;
	float z = Cbz;

	float camera_temp_theta = CameraTheta;
	float camera_temp_radius = CameraRadius;
	float camera_temp_phi = CameraPhi;

	if (RightClick)
	{
		CameraTheta = CameraTheta - (float(MouseChangeX) * 0.5f);
		CameraPhi = CameraPhi - (float(MouseChangeY) * 0.4f);
	}

	if (CameraPhi < 1)
	{
		CameraPhi = 1;
	}

	CameraRadius = CameraRadius + MouseWheelDelta * 0.09f;

	if (CameraRadius < 20)
	{
		CameraRadius = 20;
	}

	if (CameraRadius > 80)
	{
		CameraRadius = 80;
	}

	x = CameraRadius 
		* sin((CameraPhi) * PI / 180)
		* cos((CameraTheta) * PI / 180)
		+ PX;

	z = CameraRadius
		* sin((CameraPhi) * PI / 180)
		* sin((CameraTheta) * PI / 180)
		+ PZ;

	y = CameraRadius
		* cos((CameraPhi) * PI / 180)
		+ PY;

	if (y < Demo->Get_Height(x, z) + 5)
	{	
		CameraPhi = (180 / PI) * acos(
			((Demo->Get_Height(x, z) + 5) - PY)
			/ CameraRadius);

		if (CameraPhi > CameraSavePhi)	//Experimental save system
		{
			CameraSavePhi = CameraPhi;
		}
	}
	else	//Reserved for save system
	{
		if (CameraPhi < CameraSavePhi)
		{
		}
	}

	x = CameraRadius 
		* sin((CameraPhi) * PI / 180)
		* cos((CameraTheta) * PI / 180)
		+ PX;

	z = CameraRadius
		* sin((CameraPhi) * PI / 180)
		* sin((CameraTheta) * PI / 180)
		+ PZ;

	y = CameraRadius
		* cos((CameraPhi) * PI / 180)
		+ PY;

	Cbx = x;
	Cby = y;
	Cbz = z;

	D3DXMatrixTranslation(&SkyTrans[0], x, y - 100, z);
	Worlds[2] = SkyScale[0] * SkyTrans[0];

	CameraPosition.x = x;
	CameraPosition.y = y;
	CameraPosition.z = z;

	View = SetCamera(x, y, z, PX, PY, PZ);
}

void Vertex_Draw()
{
	LightDirectionFour = D3DXVECTOR4(LightX, LightY, LightZ, 0.0f);//WARNING

	D3DXVECTOR4 ambient = D3DXVECTOR4(ObjectMaterial.Ambient.r,
		ObjectMaterial.Ambient.g,
		ObjectMaterial.Ambient.b,
		ObjectMaterial.Ambient.a);

	D3DXVECTOR4 diffuse = D3DXVECTOR4(ObjectMaterial.Diffuse.r,
		ObjectMaterial.Diffuse.g,
		ObjectMaterial.Diffuse.b,
		ObjectMaterial.Diffuse.a);

	D3DXMATRIX ViewProjMatrix = View * Proj;
	Effects->LightingEffectSimple->SetMatrix(Effects->ViewProjSHandle, &ViewProjMatrix);

	Effects->LightingEffectSimple->SetVector(Effects->LightDirSHandle, &LightDirectionFour);
	Effects->LightingEffectSimple->SetVector(Effects->AmbientMtrlSHandle, &ambient);
	Effects->LightingEffectSimple->SetVector(Effects->DiffuseMtrlSHandle, &diffuse);		
	Effects->LightingEffectSimple->SetVector(Effects->CameraPositionSHandle, &CameraPosition);
	Effects->LightingEffectSimple->SetBool(Effects->FogOnSHandle, false);

	Effects->LightingEffectSimple->SetTechnique(Effects->LightTechSHandle);

	Effects->LightingEffectSimple->Begin(0, 0);
	Effects->LightingEffectSimple->BeginPass(1);

	D3DXMATRIX WorldView = Worlds[2] * View;
	D3DXMATRIX WorldViewProj = Worlds[2] * View * Proj;

	D3DXVECTOR4 amb = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXVECTOR4 dif = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

	Effects->LightingEffectSimple->SetMatrix(Effects->ViewMatrixSHandle, &Worlds[2]);
	Effects->LightingEffectSimple->SetMatrix(Effects->ViewProjMatrixSHandle, &WorldViewProj);
	Effects->LightingEffectSimple->SetVector(Effects->AmbientIntensitySHandle, &amb);
	Effects->LightingEffectSimple->SetVector(Effects->DiffuseIntensitySHandle, &dif);
	Effects->LightingEffectSimple->SetTexture(Effects->TexSHandle, SkyT);

	Effects->LightingEffectSimple->CommitChanges();

	Objects[1]->DrawSubset(0);									//Draw Sky Top

	Effects->LightingEffectSimple->SetTexture(Effects->TexSHandle, SkyBT);

	Effects->LightingEffectSimple->CommitChanges();

	Objects[2]->DrawSubset(0);									//Draw Sky Bottom

	Effects->LightingEffectSimple->EndPass();
	Effects->LightingEffectSimple->End();

	Effects->LightingEffect->SetTechnique(Effects->LightTechHandle);
		
	ambient = D3DXVECTOR4(GroundMaterial.Ambient.r,
		GroundMaterial.Ambient.g,
		GroundMaterial.Ambient.b,
		GroundMaterial.Ambient.a);

	diffuse = D3DXVECTOR4(GroundMaterial.Diffuse.r,
		GroundMaterial.Diffuse.g,
		GroundMaterial.Diffuse.b,
		GroundMaterial.Diffuse.a);
	
	WorldView = Worlds[0] * View;
	Effects->LightingEffect->SetMatrix(Effects->ViewMatrixMHandle, &WorldView);

	WorldViewProj = Worlds[0] * View * Proj;
	Effects->LightingEffect->SetMatrix(Effects->ViewProjMatrixMHandle, &WorldViewProj);
	Effects->LightingEffect->SetMatrix(Effects->ViewProjMHandle, &ViewProjMatrix);
	Effects->LightingEffect->SetVector(Effects->CameraPositionMHandle, &CameraPosition);
	Effects->LightingEffect->SetVector(Effects->LightDirMHandle, &LightDirectionFour);
	Effects->LightingEffect->SetVector(Effects->AmbientMtrlMHandle, &ambient);
	Effects->LightingEffect->SetVector(Effects->DiffuseMtrlMHandle, &diffuse);
	Effects->LightingEffect->SetTexture(Effects->TexAMHandle, Grass1T);
	Effects->LightingEffect->SetTexture(Effects->TexBMHandle, Grass2T);
	Effects->LightingEffect->SetTexture(Effects->TexCMHandle, Grass3T);
	Effects->LightingEffect->SetTexture(Effects->OpacTexBMHandle, Grass2OpacT);
	Effects->LightingEffect->SetTexture(Effects->OpacTexCMHandle, Grass3OpacT);
	Effects->LightingEffect->SetBool(Effects->FogOnMHandle, true);

	Effects->LightingEffect->Begin(0, 0);
	Effects->LightingEffect->BeginPass(0);

	Demo->Draw(false);												//Draw Map

	Effects->LightingEffect->EndPass();
	Effects->LightingEffect->End();

	ambient = D3DXVECTOR4(ObjectMaterial.Ambient.r,
		ObjectMaterial.Ambient.g,
		ObjectMaterial.Ambient.b,
		ObjectMaterial.Ambient.a);

	diffuse = D3DXVECTOR4(ObjectMaterial.Diffuse.r,
		ObjectMaterial.Diffuse.g,
		ObjectMaterial.Diffuse.b,
		ObjectMaterial.Diffuse.a);

	Effects->LightingEffectSimple->Begin(0, 0);
	Effects->LightingEffectSimple->BeginPass(0);

	Effects->LightingEffectSimple->SetTexture(Effects->TexSHandle, TexT);
	Effects->LightingEffectSimple->SetVector(Effects->CameraPositionSHandle, &CameraPosition);
	Effects->LightingEffectSimple->SetBool(Effects->FogOnSHandle, true);

	for (int i = 0; i < 10; i++)
	{
		WorldView = PillarWorlds[i] * View;
		WorldViewProj = PillarWorlds[i] * View * Proj;

		D3DXVECTOR4 amb = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
		D3DXVECTOR4 dif = D3DXVECTOR4(0.4f, 0.4f, 0.4f, 1.0f);

		Effects->LightingEffectSimple->SetMatrix(Effects->ViewMatrixSHandle, &PillarWorlds[i]);
		Effects->LightingEffectSimple->SetMatrix(Effects->ViewProjMatrixSHandle, &WorldViewProj);
		Effects->LightingEffectSimple->SetVector(Effects->AmbientIntensitySHandle, &amb);
		Effects->LightingEffectSimple->SetVector(Effects->DiffuseIntensitySHandle, &dif);

		Effects->LightingEffectSimple->CommitChanges();

		Objects[0]->DrawSubset(0);									//Draw Pillars
	}

	Effects->LightingEffectSimple->EndPass();
	Effects->LightingEffectSimple->End();

	Effects->CellEffect->SetTechnique(Effects->CellTechHandle);

	Effects->CellEffect->Begin(0, 0);
	Effects->CellEffect->BeginPass(0);

	WorldViewProj = Worlds[1] * View * Proj;
	Effects->CellEffect->SetMatrix(Effects->ViewProjMatrixCHandle, &WorldViewProj);

	Effects->CellEffect->CommitChanges();
	Objects[3]->DrawSubset(0);																	//Draw Character

	Effects->CellEffect->EndPass();
	Effects->CellEffect->End();
}

void PP_Prep()
{
	d3ddev->EndScene();
}

void PP_Effects()
{
	d3ddev->BeginScene();

	d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void Pre_Present()
{
	Post_Vertex* PV = 0;

	Quad->Lock(0, 0, (void**)&PV, 0);

	PV[0] = Post_Vertex(0.0f, 0.0f, 0.0f, 0.0f);
	PV[1] = Post_Vertex(0.0f, float(BloomHeight), 0.0f, 1.0f);
	PV[2] = Post_Vertex(float(BloomWidth), float(BloomHeight), 1.0f, 1.0f);
	PV[3] = Post_Vertex(float(BloomWidth), 0.0f, 1.0f, 0.0f);

	Quad->Unlock();

	d3ddev->SetRenderTarget(0, RenderSurface_Two);

	Effects->BloomEffect->SetTexture(Effects->BloomGameTextureHandle, RenderTexture);

	Effects->BloomEffect->SetTechnique(Effects->BloomTechHandle);

	d3ddev->SetFVF(Post_Vertex::FVF);

	Effects->BloomEffect->Begin(0, 0);

	Effects->BloomEffect->BeginPass(0);

	d3ddev->SetStreamSource(0, Quad, 0, sizeof(Post_Vertex));
	d3ddev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	Effects->BloomEffect->EndPass();

	Effects->BloomEffect->SetTexture(Effects->BloomGameTextureHandle, RenderTexture_Two);
	Effects->BloomEffect->CommitChanges();

	Effects->BloomEffect->BeginPass(1);

	d3ddev->SetRenderTarget(0, RenderSurface_Three);

	d3ddev->SetStreamSource(0, Quad, 0, sizeof(Post_Vertex));
	d3ddev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	Effects->BloomEffect->EndPass();

	Effects->BloomEffect->BeginPass(2);

	d3ddev->SetRenderTarget(0, RenderSurface_Two);

	Effects->BloomEffect->SetTexture(Effects->BloomGameTextureHandle, RenderTexture_Three);
	Effects->BloomEffect->CommitChanges();

	d3ddev->SetStreamSource(0, Quad, 0, sizeof(Post_Vertex));
	d3ddev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	Effects->BloomEffect->EndPass();
	Effects->BloomEffect->End();																	//Allow cell shaded drawing

	d3ddev->SetRenderTarget(0, RenderSurface);

	d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	d3ddev->SetFVF(Simple_Vertex::FVF);

	Effects->CellEffect->SetTechnique(Effects->CellTechHandle);

	Effects->CellEffect->Begin(0, 0);
	Effects->CellEffect->BeginPass(1);

	D3DXMATRIX ViewProjMatrix = View * Proj;
	D3DXMATRIX WorldView = Worlds[1] * View;
	D3DXMATRIX WorldViewProj = Worlds[1] * View * Proj;
	Effects->CellEffect->SetMatrix(Effects->ViewMatrixCHandle, &Worlds[1]);
	Effects->CellEffect->SetMatrix(Effects->ViewProjMatrixCHandle, &WorldViewProj);
	Effects->CellEffect->SetVector(Effects->CameraPositionCHandle, &CameraPosition);
	Effects->CellEffect->SetBool(Effects->FogOnCHandle, true);
	Effects->CellEffect->SetVector(Effects->LightDirCHandle, &LightDirectionFour);
	Effects->CellEffect->SetMatrix(Effects->ViewProjCHandle, &ViewProjMatrix);

	Effects->CellEffect->SetTexture(Effects->TexCHandle, CellT);
	Effects->CellEffect->CommitChanges();

	Objects[3]->DrawSubset(0);																	//Draw Character

	Effects->CellEffect->EndPass();
	Effects->CellEffect->End();

	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	Effects->OutlineEffect->SetTechnique(Effects->OutlineTechHandle);

	Effects->OutlineEffect->Begin(0, 0);
	Effects->OutlineEffect->BeginPass(0);
	Effects->OutlineEffect->SetMatrix(Effects->ProjOHandle, &Proj);
	Effects->OutlineEffect->SetMatrix(Effects->ViewMatrixOHandle, &WorldView);
	Effects->OutlineEffect->CommitChanges();

	d3ddev->SetTexture(0, 0); //!!!!!!!

	MeshOutlines->Draw();

	Effects->OutlineEffect->EndPass();
	Effects->OutlineEffect->End();

	d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	d3ddev->SetFVF(Post_Vertex::FVF);

	Effects->BloomEffect->Begin(0, 0);

	Effects->BloomEffect->SetTexture(Effects->BloomTextureHandle, RenderTexture_Two);
	Effects->BloomEffect->SetTexture(Effects->BloomGameTextureHandle, RenderTexture);
	Effects->BloomEffect->CommitChanges();

	d3ddev->SetRenderTarget(0, MainSurface);

	Effects->BloomEffect->BeginPass(3);

	Quad->Lock(0, 0, (void**)&PV, 0);

	PV[0] = Post_Vertex(0.0f, 0.0f, 0.0f, 0.0f);
	PV[1] = Post_Vertex(0.0f, SCREEN_HEIGHT, 0.0f, 1.0f);
	PV[2] = Post_Vertex(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f);
	PV[3] = Post_Vertex(SCREEN_WIDTH, 0.0f, 1.0f, 0.0f);

	Quad->Unlock();

	d3ddev->SetStreamSource(0, Quad, 0, sizeof(Post_Vertex));
	d3ddev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	Effects->BloomEffect->EndPass();

	Effects->BloomEffect->End();

	d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	d3ddev->SetTexture(0,0);

	d3ddev->EndScene();
}

void Mouse()
{
}

void Interface()
{
	d3ddev->BeginScene();

	SpHd->Begin(D3DXSPRITE_ALPHABLEND);

	SpHd->End();

	d3ddev->EndScene();
}

void Present()
{
	HRESULT hr;

	hr = d3ddev->Present(NULL, NULL, NULL, NULL);

	if(hr == D3DERR_DEVICELOST)
	{
		DeviceLost = true;
	}

	MouseChangeX = 0;
	MouseChangeY = 0;
	MouseWheelDelta = 0;
	DPress = false;
	APress = false;
	SPress = false;
	WPress = false;
}

void Secondary()			//For secondary presenting procedures, see test
{
}

bool CheckLostDevices()
{
	HRESULT hr;

    if(DeviceLost == true)
    {
        Sleep(100);

        if(FAILED(hr = d3ddev->TestCooperativeLevel()))
        {
            if(hr == D3DERR_DEVICELOST)
			{
                return false;
			}

            if(hr == D3DERR_DEVICENOTRESET)
            {
	            ReleaseLostObjects();

                hr = d3ddev->Reset(&d3dpp);

                if(hr == D3DERR_INVALIDCALL)
				{
                    return false;
				}

                RestoreLostObjects();
            }

            return false;										//Return to skip rendering once
        }

        DeviceLost = false;
    }

	if (DeviceLost == false)
	{
		return true;
	}
}

HRESULT ReleaseLostObjects()
{
	SafeRelease(RenderSurface);
	SafeRelease(RenderTexture);
	SafeRelease(RenderTexture_Two);
	SafeRelease(RenderSurface_Two);
	SafeRelease(RenderTexture_Three);
	SafeRelease(RenderSurface_Three);
	SafeRelease(MainSurface);

	Effects->LightingEffect->OnLostDevice();
	Effects->LightingEffectSimple->OnLostDevice();
	Effects->BloomEffect->OnLostDevice();
	SpHd->OnLostDevice();

	return S_OK;
}

HRESULT RestoreLostObjects()
{
	Effects->LightingEffect->OnResetDevice();
	Effects->LightingEffectSimple->OnResetDevice();
	Effects->BloomEffect->OnResetDevice();
	SpHd->OnResetDevice();

	d3ddev->CreateTexture(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_R8G8B8,
		D3DPOOL_DEFAULT,
		&RenderTexture,
		NULL);

	d3ddev->CreateTexture(BloomWidth,
		BloomHeight, 
		1, 
		D3DUSAGE_RENDERTARGET,
		D3DFMT_R8G8B8, 
		D3DPOOL_DEFAULT, 
		&RenderTexture_Two,
		NULL);

	d3ddev->CreateTexture(BloomWidth,
		BloomHeight, 
		1, 
		D3DUSAGE_RENDERTARGET,
		D3DFMT_R8G8B8, 
		D3DPOOL_DEFAULT, 
		&RenderTexture_Three,
		NULL);

	RenderTexture->GetSurfaceLevel(0, &RenderSurface);
	
	RenderTexture_Two->GetSurfaceLevel(0, &RenderSurface_Two);

	RenderTexture_Three->GetSurfaceLevel(0, &RenderSurface_Three);

	d3ddev->GetRenderTarget(0, &MainSurface);

	Proj = SetPerspective(1.0f, ratio, 1.0f, 10000.0f);		//REMEMBER TO CHANGE ALONG WITH ORIGINAL

	d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void SetMouseChange(int x, int y)
{
	MouseChangeX = x;
	MouseChangeY = y;
}

void SetMouseClick(int button, bool click)
{
	if (button == 0)
	{
		LeftClick = click;
	}
	else if (button == 1)
	{
		RightClick = click;
	}
}

void SetMouseWheel(int wheeldelta)
{
	MouseWheelDelta = wheeldelta;
}

void SetKeyboard(int key, bool value)
{
	switch (key)
	{
		case 1:
		{
			APress = value;
		}
		case 4:
		{
			DPress = value;
		}
		case 19:
		{
			SPress = value;
		}
		case 23:
		{
			WPress = value;
		}
	}
}

void GetPickingRay(D3DXVECTOR3& originW, D3DXVECTOR3& dirW)
{
	POINT mousepoint;
	GetCursorPos(&mousepoint);

	ScreenToClient(ghwnd, &mousepoint);

	float xoffset = 0;
	float yoffset = 0;

	float w = (float)SCREEN_WIDTH;
	float h = (float)SCREEN_HEIGHT;

	if (d3dpp.Windowed)
	{
		xoffset = 17;
		yoffset = 41;
	}

	float x = (2.0f * (mousepoint.x + (xoffset * (mousepoint.x / w))) / w - 1.0f) / Proj(0, 0);
	float y = (-2.0f * (mousepoint.y + (yoffset * (mousepoint.y / h))) / h + 1.0f) / Proj(1, 1);

	D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(x, y, 1.0f);

	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, 0, &View);

	D3DXVec3TransformCoord(&originW, &origin, &invView);
	D3DXVec3TransformNormal(&dirW, &dir, &invView);
	D3DXVec3Normalize(&dirW, &dirW);
}

void Game_End(HWND hwnd)
{
	d3d->Release();

	d3ddev->Release();

	SafeRelease(SpHd);

	SafeRelease(Quad);

	SafeRelease(MainSurface);

	SafeRelease(RenderSurface);

	SafeRelease(RenderTexture);

	SafeRelease(RenderSurface_Two);

	SafeRelease(RenderTexture_Two);

	SafeRelease(RenderSurface_Three);

	SafeRelease(RenderTexture_Three);

	SafeRelease(Effects->LightingEffect);

	SafeRelease(Effects->LightingEffectSimple);

	SafeRelease(Effects->BloomEffect);

	SafeRelease(Effects->CellEffect);

	SafeRelease(Effects->OutlineEffect);

	SafeRelease(Cursor);

	SafeRelease(CursorDrag);

	SafeRelease(Grass1T);

	SafeRelease(Grass2T);

	SafeRelease(Grass3T);

	SafeRelease(CellT);

	SafeRelease(TexT);

	SafeRelease(SkyT);

	SafeRelease(SkyBT);

	SafeRelease(Grass2OpacT);

	SafeRelease(Grass3OpacT);

	SafeRelease(Dirt1OpacT);

	for (int i = 0; i < 4; i++)
	{
		SafeRelease(Objects[i]);
	}
}