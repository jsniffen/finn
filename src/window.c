typedef struct {
	GapBuffer tag;
	GapBuffer content;
} Window;

void win_open(Window *w, uint8_t *filename)
{
	gb_create(&w->tag, filename, strlen(filename));

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

	gb_create(&w->content, buffer, size);

	SDL_RWclose(file);
	free(buffer);
}

void win_render(Window *win, SDL_Renderer *r, MouseInput mouse, SDL_Rect pos)
{
	int font_height = get_font_height();
	int font_width = get_font_height();

	SDL_Color fg = {0, 0, 0, 255};
	
	// render the content
	SDL_Rect rect_content = {pos.x, pos.y+font_height-1, pos.w, pos.h};

	SDL_SetRenderDrawColor(r, 255, 255, 234, 255);
	SDL_RenderFillRect(r, &rect_content);

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderDrawRect(r, &rect_content);

	rect_content.x += font_width;
	
	gb_render(&win->content, r, mouse, rect_content, fg);

	// render the scrollbar
	SDL_Rect rect_scroll = {pos.x, pos.y, font_width-5, pos.h};
	SDL_SetRenderDrawColor(r, 153, 153, 76, 255);
	SDL_RenderDrawRect(r, &rect_scroll);

	// render the tag
	SDL_Rect rect_tag = {pos.x, pos.y, pos.w, font_height};

	SDL_SetRenderDrawColor(r, 234, 255, 255, 255);
	SDL_RenderFillRect(r, &rect_tag);

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderDrawRect(r, &rect_tag);

	rect_tag.x += font_width;

	gb_render(&win->tag, r, mouse, rect_tag, fg);
	
	// render the save button
	SDL_Rect rect_button = {pos.x, pos.y, font_width-5, font_height};

	SDL_SetRenderDrawColor(r, 136, 136, 204, 255);
	SDL_RenderFillRect(r, &rect_button);

	if (true) {
		// render the dirty button
		SDL_Rect rect_dirty = {pos.x+3, pos.y+3, font_width-11, font_height-6};

		SDL_SetRenderDrawColor(r, 0, 0, 153, 255);
		SDL_RenderFillRect(r, &rect_dirty);
	}
}
