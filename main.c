#include <SDL/SDL.h>
#include <stdio.h>

int main(int argc, char **args)
{
	SDL_Window *window;
	SDL_Surface *screen;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Finn", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
	if (window == 0) {
		fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
	}

	screen = SDL_GetWindowSurface(window);
	SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0));
	SDL_UpdateWindowSurface(window);
	
	SDL_Delay(2000);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
