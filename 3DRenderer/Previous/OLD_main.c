#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include <assert.h>
#include "Display.h"
#include "Vector.h"

#define N_POINTS (9 * 9 * 9)

// Globals
bool is_running = false;
int previous_frame_time = 0;

vec3_t cube_points[N_POINTS]; // 9x9x9 = 729 points cube
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

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

	int point_count = 0;
	// Start loading array of vectors
	// From -1 to 1 in 9 * 9 * 9 cube
	for (float x = -1; x <= 1; x += 0.25f)
	{
		for (float y = -1; y <= 1; y += 0.25f)
		{
			for (float z = -1; z <= 1; z += 0.25f)
			{
				vec3_t new_point = { .x = x, .y = y, .z = z };
				cube_points[point_count++] = new_point;
			}
		}
	}
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

// Function that recieves a 3D vecor and returns a projected 2D point
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
	// Rotate cube
	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	cube_rotation.z += 0.01;

	for (int i = 0; i < N_POINTS; i++)
	{
		vec3_t point = cube_points[i];
		// Rotate point
		vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

		// Translate point away from camera
		transformed_point.z -= camera_position.z;

		// Project point and save to array
		vec2_t projected_point = project(transformed_point);
		projected_points[i] = projected_point;
	}
}


void render(void)
{
	draw_point_grid(20);
	
	// Loop through all points and draw them
	for (int i = 0; i < N_POINTS; i++)
	{
		vec2_t point = projected_points[i];
		draw_rect(
			point.x + (window_width / 2),
			point.y + (window_height / 2),
			4,
			4,
			0xFFFFFF00
		);
	}

	render_colour_buffer();
	clear_colour_buffer(0xFF00000000);

	SDL_RenderPresent(renderer);
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

	return 0;
}