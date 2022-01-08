#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#include "src/buffer.c"
#include "src/text.c"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;
SDL_Texture *texture;
SDL_Surface *surface;
TTF_Font *font;
bool running;
int cursor[2];

void render_cursor(SDL_Renderer *r, int pos[2])
{
	int h; 
	h = TTF_FontHeight(font);
	SDL_Rect rect = {pos[0], pos[1], 2, h};
	SDL_RenderFillRect(r, &rect);
}

int main(int argc, char **args)
{
	if (TTF_Init() == -1) {
		fprintf(stderr, "failed to initialize SDL_ttf: %s\n", TTF_GetError());
		return 1;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == 0) {
		fprintf(stderr, "failed to set hint: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Finn", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
	if (window == 0) {
		fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == 0) {
		fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
		return 1;
	}

	font = open_font(args[1], 24);

	SDL_RWops *file = SDL_RWFromFile("run.bat", "rwb");
	if (file == 0)
	{
		fprintf(stderr, "failed to open file: %s\n", SDL_GetError());
		return 1;
	}

	cursor[0] = 0;
	cursor[1] = 0;

	buffer buf;
	if (open_buffer(&buf, "build.bat") == -1) {
		fprintf(stderr, "failed to create buffer\n");
		return 1;
	}

	running = true;
	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) running = false;
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		render_file(renderer, font, file, 0, 0);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		render_cursor(renderer, cursor);

		SDL_RenderPresent(renderer);

		SDL_Delay(60);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
