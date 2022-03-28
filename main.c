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

typedef struct
{
	bool was_pressed;
	bool pressed;
} input_state;

typedef struct
{
	SDL_Point position;
	input_state buttons[2];
} MouseInput;

bool lclick(MouseInput mouse) {
	return mouse.buttons[0].was_pressed == false
		&& mouse.buttons[0].pressed == true;
}

#include "src/text.c"
#include "src/gapbuffer.c"
#include "src/buffer.c"
#include "src/window.c"


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;
SDL_Texture *texture;
SDL_Surface *surface;
static Window win;
static MouseInput mouse;
bool running;

void update_mouse()
{
	int state = SDL_GetMouseState(&mouse.position.x, &mouse.position.y);
	mouse.buttons[0].was_pressed = mouse.buttons[0].pressed;
	mouse.buttons[0].pressed = state & SDL_BUTTON_LMASK;
	mouse.buttons[1].was_pressed = mouse.buttons[1].pressed;
	mouse.buttons[1].pressed = state & SDL_BUTTON_RMASK;
}


void handle_events()
{
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT: {
				running = false;
			} break;

#if 0
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP: {
				update_mouse();
			} break;
#endif
			case SDL_TEXTINPUT: {
				win_input(&win, e.text.text);
			} break;

			case SDL_KEYDOWN: {
				win_keydown(&win, e.key.keysym.sym);
			} break;
		}
	}
}

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

	if (!open_font(args[1], 24)) {
		fprintf(stderr, "failed to open font: %s\n", SDL_GetError());
		return 1;
	}

	win_open(&win, "test.txt");
	SDL_Rect rect = {0, 0, 1280, 720};

	SDL_StartTextInput();


	running = true;
	while (running) {
		handle_events();

		update_mouse();

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		win_render(&win, renderer, mouse, rect);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
