typedef struct {
	GapBuffer filename;
	GapBuffer contents;
} Window;

void win_open(Window *w, uint8_t *filename)
{

	gb_create(&w->filename, filename, strlen(filename));

	int64_t size;
	size_t read;

	SDL_RWops *file = NULL;

	file = SDL_RWFromFile(filename, "r+b");
	if (file == 0) {
		fprintf(stderr, "failed to open file: %s\n", SDL_GetError());
		return;
	}

	size = SDL_RWsize(file);
	if (size == -1) {
		fprintf(stderr, "failed to get file size: %s\n", SDL_GetError());
		return;
	}

	uint8_t *buffer = (uint8_t *)malloc(size);
	read = SDL_RWread(file, buffer, 1, size);
	if (read < size) {
		fprintf(stderr, "failed to read the entire file: %s\n", SDL_GetError());
		return;
	}

	gb_create(&w->contents, buffer, size);

	SDL_RWclose(file);
	free(buffer);
}

void win_render(Window *win, SDL_Renderer *r, SDL_Rect pos, SDL_Color bg, SDL_Color fg)
{
	SDL_Rect title = {10, 10, 100, 100};
	SDL_Rect contents = {10, 100, 1000, 1000};
	gb_render(&win->filename, r, title, bg, fg);
	gb_render(&win->contents, r, contents, bg, fg);
}
