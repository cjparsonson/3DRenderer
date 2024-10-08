#include "Display.h"
#include <stdio.h>

extern SDL_Window* window = NULL;
extern SDL_Renderer* renderer = NULL;
extern uint32_t* colour_buffer = NULL;
extern SDL_Texture* colour_buffer_texture = NULL;
extern int window_width = 800;
extern int window_height = 600;

bool initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error Initializing SDL.\n");
		return false;
	}
	// Query max screen size
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;
	// create window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);
	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// create renderer
	renderer = SDL_CreateRenderer(
		window,
		-1,
		0
	);
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	return true;
}


void draw_grid(int gridSize)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			if ((x % gridSize == 0) || (y % gridSize == 0))
			{
				colour_buffer[(window_width * y) + x] = 0xFF333333;
			}
		}
	}
}

void draw_point_grid(int gridSize)
{
	for (int y = 0; y < window_height; y += gridSize)
	{
		for (int x = 0; x < window_width; x += gridSize)
		{
			colour_buffer[(window_width * y) + x] = 0xFF333333;
		}
	}
}

void draw_pixel(int x, int y, uint32_t colour)
{
	if (x >= 0 && x < window_width && y >= 0 && y < window_height)
	{
		colour_buffer[(window_width * y) + x] = colour;
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t colour)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;
	for (int i = 0; i <= longest_side_length; i++)
	{
		draw_pixel(round(current_x), round(current_y), colour);
		current_x += x_inc;
		current_y += y_inc;
		
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t colour)
{
	draw_line(x0, y0, x1, y1, colour);
	draw_line(x1, y1, x2, y2, colour);
	draw_line(x2, y2, x0, y0, colour);
}

void draw_rect(int x, int y, int width, int height, uint32_t colour)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int current_x = x + i;
			int current_y = y + j;
			draw_pixel(current_x, current_y, colour);
		}
	}
}

void render_colour_buffer(void)
{
	SDL_UpdateTexture(
		colour_buffer_texture,
		NULL,
		colour_buffer,
		(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, colour_buffer_texture, NULL, NULL);
}

void clear_colour_buffer(uint32_t colour)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			colour_buffer[(window_width * y) + x] = colour;
		}
	}
}

void destroy_window(void)
{
	free(colour_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

