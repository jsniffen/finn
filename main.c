#define SDL_ASSERT_LEVEL 2

#ifdef __linux__

#include <SDL.h>
#include <SDL_ttf.h>

#else

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#endif

#include <stdio.h>
#include <stdbool.h>

#include "src/text.c"
#include "src/gapbuffer.c"
#include "src/buffer.c"
#include "src/window.c"


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;
SDL_Texture *texture;
SDL_Surface *surface;
bool running;

int
main(int argc, char **args)
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

	if (!open_font(args[1], 32)) {
		fprintf(stderr, "failed to open font: %s\n", SDL_GetError());
		return 1;
	}

	Buffer buf;
	if (bopen(&buf, "test.txt") == -1) {
		fprintf(stderr, "failed to create Buffer\n");
		return 1;
	}

	Window win;
	win_open(&win, "test.txt");

	GapBuffer gb;
	gb_create(&gb, (uint8_t *)"hello, world", 12);

	SDL_StartTextInput();

	SDL_Rect rect = {10, 10, 100, 100};
	SDL_Color bg = {0, 0, 0, 255};
	SDL_Color fg = {0, 255, 0, 255};

	running = true;
	while (running) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: {
			      		running = false;
			       	} break;

				case SDL_TEXTINPUT: {
					binserttext(&buf, e.text.text);
			    	} break;

				case SDL_KEYDOWN: {
					switch (e.key.keysym.sym) {
						case SDLK_UP:
							bmoveu(&buf);
							break;

						case SDLK_DOWN:
							bmoved(&buf);
							break;

						case SDLK_LEFT:
							bmovel(&buf);
							break;

						case SDLK_RIGHT:
							bmover(&buf);
							break;

						case SDLK_BACKSPACE:
							bremove(&buf);
							break;

						case SDLK_RETURN:
							binsert(&buf, '\n');
							break;
					}
				} break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		// gb_render(&gb, renderer, rect, bg, fg);
		win_render(&win, renderer, rect, bg, fg);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
