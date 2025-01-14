#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "array.h"
#include <assert.h>
#include "Display.h"
#include "Vector.h"
#include "Mesh.h"

// Globals
bool is_running = false;
int previous_frame_time = 0;

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

float fov_factor = 640;

bool obj_from_blender = true; // Blender uses a right-handed coordinate system

void setup(void)
{
	// Allocate memory in bytes to hold the colour buffer
	colour_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
	// Create SDL texture to display the  colour buffer
	colour_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height);

	// Loads the cube values in the mesh data structure
	load_obj_file_data("f22.obj");
}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				is_running = false;
				break;
			}
	}
}

// Function that recieves a 3D vector and returns a projected 2D point
vec2_t project(vec3_t point)
{
	vec2_t projected_point = {
		.x = (fov_factor * point.x) /point.z,
		.y = (fov_factor * point.y) /point.z
	};
	return projected_point;
}

void update(void)
{
	// Wait some time until we reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	// Only wait if we are too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(time_to_wait);
	}
	// Get the time it takes to render the frame
	previous_frame_time = SDL_GetTicks();

	// Initialize the array of triangles to render
	triangles_to_render = NULL;

	// Rotate cube
	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;

	// Loop through all faces
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];
		vec3_t face_vertices[3] =
		{
			mesh.vertices[mesh_face.a - 1],
			mesh.vertices[mesh_face.b - 1],
			mesh.vertices[mesh_face.c - 1]
		};

		triangle_t projected_triangle;
		// Loop through all vertices of the current face and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];
			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			// Translate the vertex away from the camera
			transformed_vertex.z -= camera_position.z;

			// Invert the Z value of the vertex if the object is from Blender
			if (obj_from_blender)
			{
				transformed_vertex.z *= -1;
			}

			// Project the current vertex
			vec2_t projected_point = project(transformed_vertex);

			// Scale and translate the projected points to the middle of the screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);

			projected_triangle.points[j] = projected_point;
		}
		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}
}


void render(void)
{
	draw_point_grid(20);
	
	// Loop through all triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t triangle = triangles_to_render[i];
		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

		draw_triangle(
			triangle.points[0].x, triangle.points[0].y,
			triangle.points[1].x, triangle.points[1].y,
			triangle.points[2].x, triangle.points[2].y,
			0xFF00FF00);
	}
	
	// Clear the array of triangles to render
	array_free(triangles_to_render);

	render_colour_buffer();
	clear_colour_buffer(0xFF00000000);

	SDL_RenderPresent(renderer);
}

// Free dynamically allocated memory
void free_resources(void)
{
	free(colour_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int main(int argc, char* args[])
{
	is_running = initialize_window();
	setup();	

	assert(colour_buffer); // TODO implement error handling for possible NULL pointer
	while (is_running)
	{
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}