#include <stdio.h>
#include "Mesh.h"
#include "array.h"

mesh_t mesh = 
{ 
	.vertices = NULL,
	.faces = NULL,
	.rotation = {.x = 0, .y = 0, .z = 0 }
};

// Define mesh arrays

vec3_t cube_vertices[N_CUBE_VERTICES] =
{
	{.x = -1, .y = -1, .z = -1 },    // 1
	{.x = -1, .y = 1,  .z = -1 },    // 2
	{.x = 1,  .y = 1,  .z = -1 },    // 3
	{.x = 1,  .y = -1, .z = -1 },    // 4
	{.x = 1,  .y = 1,  .z = 1 },     // 5
	{.x = 1,  .y = -1, .z = 1 },     // 6
	{.x = -1, .y = 1,  .z = 1 },     // 7
	{.x = -1, .y = -1, .z = 1 }      // 8
};

face_t cube_faces[N_CUBE_FACES] =
{
	// front face
	{.a = 1, .b = 2, .c = 3 },
	{.a = 1, .b = 3, .c = 4 },
	// right face
	{.a = 4, .b = 3, .c = 5 },
	{.a = 4, .b = 5, .c = 6 },
	// back face
	{.a = 6, .b = 5, .c = 7 },
	{.a = 6, .b = 7, .c = 8 },
	// left face
	{.a = 8, .b = 7, .c = 2 },
	{.a = 8, .b = 2, .c = 1 },
	// top face
	{.a = 2, .b = 7, .c = 5 },
	{.a = 2, .b = 5, .c = 3 },
	// bottom face
	{.a = 6, .b = 8, .c = 1 },
	{.a = 6, .b = 1, .c = 4 }
};

void load_cube_mesh_data(void)
{
	for (int i = 0; i < N_CUBE_VERTICES; i++)
	{
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices, cube_vertex);
	}

	for (int i = 0; i < N_CUBE_FACES; i++)
	{
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces, cube_face);
	}
}

void load_obj_file_data(char* filename)
{
	// Read the contents of the obj file and
	// load the vertices and faces in our mesh.vertices and mesh.faces arrays
	FILE* file;
	char str[50];
	file = fopen_s(&file, filename, "r");
	if (!file)
	{
		printf("Failed to open file: %s\n", filename);		
	}
	// Each line of the file which begins with a single v is a vertex and needs to be read and pushed to the mesh.vertices array
	// Each line of the file which begins with a single f is a face and needs to be read and pushed to the mesh.faces array 
	// However faces are stored like this: f 1/1/1 2/2/2 3/3/3 and we only need the first number of each group
	while (fgets(str, 49, file) != NULL)
	{
		if (str[0] == 'v' && str[1] == ' ')
		{
			vec3_t vertex;
			if (sscanf_s(str, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z) == 3)
			{
				array_push(mesh.vertices, vertex);
			}
		}
		else if (str[0] == 'f' && str[1] == ' ')
		{
			face_t face;
			// Temporary variables to store the face information
			int v1, v2, v3;
			int t1, t2, t3;
			int n1, n2, n3;
			if (sscanf_s(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3) == 9)
			{
				face.a = v1;
				face.b = v2;
				face.c = v3;
				array_push(mesh.faces, face);
			}
		}
	}
	fclose(file);
}