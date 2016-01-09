#define D3D_DEBUG_INFO

#define SafeRelease(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

#include "Maps.h" 

extern LPDIRECT3DDEVICE9 d3ddev;
extern D3DXVECTOR4 LightDirectionFour;
extern D3DXVECTOR3 LightDirectionThree;

const DWORD Map::Terrain_Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5;

float Lerp(float a, float b, float t)
{
	return a - (a*t) + (b*t);
}

Map::Map(std::string RAW_File_Name,
		 int Vertex_Length,
		 int Vertex_Width,
		 int Cell_Spacing,
		 float Height_Scale)
{
	vertex_length = Vertex_Length;
	vertex_width = Vertex_Width;

	total_vertices = vertex_length * vertex_width;				//Total = L * W

	cell_length = vertex_length - 1;
	cell_width = vertex_width - 1;

	cell_spacing = Cell_Spacing;
	height_scale = Height_Scale;

	length = cell_length * cell_spacing;
	width = cell_width * cell_spacing;

	total_triangles = cell_length * cell_width * 2;

	D3DXCreateMeshFVF(cell_length * cell_width * 2,
		vertex_length * vertex_width,
		D3DXMESH_MANAGED,
		Terrain_Vertex::FVF,
		d3ddev,
		&mesh);

	if (!Raw_To_Int_Vector(RAW_File_Name))
	{
		MessageBox(0, "Raw_To_Int_Vector - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	for (int i = 0; i < height_int_vector.size(); i++)
	{
		height_float_vector.push_back((float)height_int_vector[i]);

		height_float_vector[i] = height_float_vector[i] * height_scale;
	}

	if (!Create_Normals(&LightDirectionThree))
	{
		MessageBox(0, "Create_Normals - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	Generate_Vertex_Normals();

	if (!Generate_Vertices())
	{
		MessageBox(0, "Generate_Vertices - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	if (!Generate_Indices())									//Also Generates Mesh
	{
		MessageBox(0, "Generate_Indices - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	adjacency_buffer = new DWORD[cell_width * cell_length * 2 * 3];

	mesh->GenerateAdjacency(0.001f, adjacency_buffer);
	mesh->OptimizeInplace(D3DXMESH_MANAGED |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_ATTRSORT | 
		D3DXMESHOPT_VERTEXCACHE,
		adjacency_buffer,
		adjacency_buffer,
		0,
		0);

	IDirect3DVertexBuffer9* bufvert;
	Terrain_Vertex* tempvert;

	mesh->GetVertexBuffer(&bufvert);
	bufvert->Lock(0, 0, (void**)&tempvert, 0);
	vertexdata = tempvert;
	bufvert->Unlock();

	IDirect3DIndexBuffer9* bufind;
	WORD* tempind;

	mesh->GetIndexBuffer(&bufind);
	bufind->Lock(0, 0, (void**)&tempind, 0);
	indexdata = tempind;
	bufind->Unlock();

	SafeRelease(bufvert);
	SafeRelease(bufind);
}

bool Map::Raw_To_Int_Vector(std::string File_Name)
{
	std::vector<BYTE> temp(total_vertices);						//Data will be copied to the int vector

	std::ifstream input_file(File_Name.c_str(),					//Stores data in input_file
		std::ios_base::binary);

	if (input_file == 0)
	{
		return false;
	}

	input_file.read((char*)&temp[0],							//Copies data into temp
		temp.size());

	input_file.close();

	height_int_vector.resize(total_vertices);

	for (int i = 0; i < temp.size(); i++)						//Copies data into int vector
	{
		height_int_vector[i] = temp[i];
	}

	return true;
}

float Map::Get_Height_From_RAW(int Row, int Column)
{
	return height_float_vector[(Row * vertex_length) + Column];
}

bool Map::Generate_Vertices()
{
	HRESULT hresult = 0;

	int start_x = 0;
	int start_z = 0;

	int end_x = length;
	int end_z = width;

	float u_increment = 1.0f / (float)cell_length;
	float v_increment = 1.0f / (float)cell_width;

	IDirect3DVertexBuffer9* temp_buffer = 0;
	mesh->GetVertexBuffer(&temp_buffer);

	Terrain_Vertex* v;

	temp_buffer->Lock(0, 0, (void**)&v, 0);

	int i = 0;

	for (int z = start_z; z <= end_z; z = z + cell_spacing)		//###
	{
		int j = 0;

		for (int x = start_x; x <= end_x; x = x + cell_spacing)
		{
			int element = (i * vertex_length) + j;

			D3DXVECTOR3 Posi((float)x,
				height_float_vector[element],
				(float)z);

			D3DXVECTOR3 Norm(-N[element].x,
				-N[element].y,
				N[element].z);

			v[element] = Terrain_Vertex(
				Posi,
				Norm,
				(float)j * u_increment * 5,
				(float)i * v_increment * 5,
				(float)j * u_increment * 5,
				(float)i * v_increment * 5,
				(float)j * u_increment * 5,
				(float)i * v_increment * 5,
				(float)j * u_increment,
				(float)i * v_increment,
				(float)j * u_increment,
				(float)i * v_increment);

			j++;
		}
		i++;
	}

	temp_buffer->Unlock();

	return true;
}

bool Map::Generate_Indices()
{
	HRESULT hresult = 0;

	IDirect3DIndexBuffer9* temp_buffer = 0;
	mesh->GetIndexBuffer(&temp_buffer);

	WORD* index;

	temp_buffer->Lock(0, 0, (void**)&index, 0);

	int six_count = 0;

	for (int i = 0; i < cell_width; i++)
	{
		for (int j = 0; j < cell_length; j++)
		{
			index[six_count]  =
				(i * vertex_length) + j;

			index[six_count + 1] =
				(i * vertex_length) + j + 1;

			index[six_count + 2] =
				((i + 1) * vertex_length) + j;

			index[six_count + 3] =
				((i + 1) * vertex_length) + j;

			index[six_count + 4] =
				(i * vertex_length) + j + 1;

			index[six_count + 5] =
				((i + 1) * vertex_length) + j + 1;


			six_count = six_count + 6;
		}
	}

	temp_buffer->Unlock();

	SafeRelease(temp_buffer);

	return true;
}

bool Map::Draw(bool Wire)
{
	HRESULT hresult = 0;

	d3ddev->SetFVF(Terrain_Vertex::FVF);

	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); //WARNING : FIX LATER

	if (Wire == true)
	{
		d3ddev->SetRenderState(D3DRS_FILLMODE, 
			D3DFILL_WIREFRAME);

		mesh->DrawSubset(0);

		d3ddev->SetRenderState(D3DRS_FILLMODE,
			D3DFILL_SOLID);
	}

	else if (Wire == false)
	{
		mesh->DrawSubset(0);
	}

	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if(FAILED(hresult))
	{
		return false;
	}

	return true;
}

bool Map::Create_Normals(D3DXVECTOR3* directionToLight)
{
	for(int i = 0; i < cell_width; i++)
	{
		for(int j = 0; j < cell_length; j++)
		{
			float heightA = float(Get_Height_From_RAW(i, j));
			float heightB = float(Get_Height_From_RAW(i, j + 1));
			float heightC = float(Get_Height_From_RAW(i + 1, j));

			D3DXVECTOR3 u(float(-cell_spacing),
				heightB - heightA, 
				0.0f);
			
			D3DXVECTOR3 v(0.0f,
				heightC - heightA, 
				float(-cell_spacing));

			D3DXVec3Normalize(&u, &u);
			D3DXVec3Normalize(&v, &v);

			D3DXVECTOR3 n;

			D3DXVec3Cross(&n, &u, &v);
			D3DXVec3Normalize(&n, &n);

			CN.push_back(n);
		}
	}

	return true;
}

void Map::Generate_Vertex_Normals()
{
	for (int i = 0; i < vertex_width; i++)
	{
		for (int j = 0; j < vertex_length; j++)
		{
			D3DXVECTOR3 temp;

			if (j == 0 && i == 0)
			{
				temp = CN[0];
			}
			else if (j == 0 && i == vertex_width - 1)
			{
				temp = CN[cell_length * (cell_width - 1)];
			}
			else if (j == vertex_length - 1 && i == 0)
			{
				temp = CN[cell_length - 1];
			}
			else if (j == vertex_length - 1 && i == vertex_width - 1)
			{
				temp = CN[(cell_length * cell_width) - 1];
			}
			else if (j == 0)
			{
				temp = CN[cell_length * i]
				+ CN[cell_length * (i - 1)];
			}
			else if (j == vertex_length - 1)
			{
				temp = CN[(cell_length * i) + cell_length - 1]
				+ CN[(cell_length * (i - 1)) + cell_length - 1];
			}
			else if (i == 0)
			{
				temp = CN[j - 1]
				+ CN[j];
			}
			else if (i == vertex_width - 1)
			{
				temp = CN[(cell_length * (i - 1)) + j]
				+ CN[(cell_length * (i - 1)) + j - 1];
			}
			else
			{
				temp = CN[(cell_length * i) + j]
				+ CN[(cell_length * i) + j - 1]
				+ CN[(cell_length * (i - 1)) + j]
				+ CN[(cell_length * (i - 1)) + j - 1];
			}

			D3DXVec3Normalize(&temp, &temp);

			N.push_back(temp);
		}
	}
}

void Map::Generate_Mesh()
{
}

float Map::Get_Height(float X, float Z)
{
	float height = 0.0f;

	X = X;
	Z = Z;

	X = X / cell_spacing;
	Z = Z / cell_spacing;

	int column = int(floorf(X)); //WARNING
	int row = int(floorf(Z));

	float A = Get_Height_From_RAW(row, column);
	float B = Get_Height_From_RAW(row, column + 1);
	float C = Get_Height_From_RAW(row + 1, column);
	float D = Get_Height_From_RAW(row + 1, column + 1);

	float dx = X - column;
	float dz = Z - row;

	if(dz < 1.0f - dx)
	{
		float uy = B - A;
		float vy = C - A;
		height = A + Lerp(0.0f, uy, dx) +
		Lerp(0.0f, vy, dz);
	}
	else
	{
		float uy = C - D;
		float vy = B - D;
		height = D + Lerp(0.0f, uy, 1.0f - dx) +
		Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

Map::~Map()
{
};