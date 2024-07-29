#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* colour_buffer;
extern SDL_Texture* colour_buffer_texture;
extern int window_width;
extern int window_height;

bool initialize_window(void);
void draw_grid(int gridSize);
void draw_point_grid(int gridSize);
void draw_pixel(int x, int y, uint32_t colour);
void draw_rect(int x, int y, int width, int height, uint32_t colour);
void render_colour_buffer(void);
void clear_colour_buffer(uint32_t colour);
void destroy_window(void);


#endif // !DISPLAY_H

