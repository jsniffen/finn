#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;
SDL_Texture *texture;
SDL_Surface *surface;
TTF_Font *font;
bool running;

int main(int argc, char **args)
{
	if (TTF_Init() == -1) {
		fprintf(stderr, "failed to initialize SDL_ttf: %s\n", TTF_GetError());
		return 1;
	}

	font = TTF_OpenFont(args[1], 100);
	if (font == 0) {
		fprintf(stderr, "failed to open font: %s\n", SDL_GetError());
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

	SDL_Color color = {0, 0, 0};
	surface = TTF_RenderText_Solid(font, "A", color);
	if (surface == 0) {
		fprintf(stderr, "failed to create render text solid: %s\n", TTF_GetError());
		return 1;
	}
	
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == 0) {
		fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
		return 1;
	}

	running = true;
	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) running = false;
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		SDL_Rect quad = {100, 100, 50, 50};
		SDL_RenderCopyEx(renderer, texture, 0, &quad, 0, 0, 0);

		SDL_RenderPresent(renderer);

		SDL_Delay(60);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
