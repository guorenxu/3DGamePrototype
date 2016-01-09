#ifndef _MAPS_H
#define _MAPS_H

#include <iostream>
#include <fstream>
#include "CString"
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

float Lerp(float a, float b, float t);

class Map		//Remeber to add floating scale algorithm
{
	public:
		struct Terrain_Vertex
		{
			Terrain_Vertex(){}

			Terrain_Vertex(D3DXVECTOR3 Pos,
				D3DXVECTOR3 Nor, 
				float U1, 
				float V1,
				float U2,
				float V2,
				float U3,
				float V3,
				float U4,
				float V4,
				float U5,
				float V5)
			{
				p = Pos;
				n = Nor;
				u1 = U1;
				v1 = V1;
				u2 = U2;
				v2 = V2;
				u3 = U3;
				v3 = V3;
				u4 = U4;
				v4 = V4;
				u5 = U5;
				v5 = V5;
			}

			D3DXVECTOR3 p;
			D3DXVECTOR3 n;
			float u1, v1, u2, v2, u3, v3, u4, v4, u5, v5;

			static const DWORD FVF;
		};

		Map(std::string RAW_File_Name,
			int Vertex_Length,
			int Vertex_Width,
			int Cell_Spacing,
			float Height_Scale);

		~Map();

		bool Raw_To_Int_Vector(std::string File_Name);			//Converts the RAW to an int vector

		float Get_Height_From_RAW(int Row, int Column);			//Retrieves the height

		bool Generate_Vertices();								//Generates the vertices for drawing

		bool Generate_Indices();								//Generates the indices for drawing

		void Generate_Mesh();

		bool Draw(bool Wire);

		bool Create_Normals(D3DXVECTOR3* directionToLight);

		void Generate_Vertex_Normals();

		float Get_Height(float X, float Z);

		D3DXVECTOR3* GetVertexByIndex(long index);

		std::vector<int> height_int_vector;						//Holds byte data from RAW stream

		std::vector<float> height_float_vector;

		int vertex_length;										//Vertex Lengths
		int vertex_width;

		int cell_length;										//Cell Lengths
		int cell_width;

		int total_vertices;										//Number of total vectors (L * W)

		int cell_spacing;										//Distance between vertices

		int length;
		int width;

		float height_scale;										//Height scaling
		
		int total_triangles;

		std::vector<D3DXVECTOR3> CN;

		std::vector<D3DXVECTOR3> N;

		LPD3DXMESH mesh;

		DWORD* adjacency_buffer;

		D3DXVECTOR3 pre[3];

		Terrain_Vertex* vertexdata;
		WORD* indexdata;
};

#endif