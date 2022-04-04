typedef struct {
	GapBuffer tag;
	bool tag_active;

	GapBuffer content;
	bool content_active;

	bool active;
	bool dirty;
} Window;

void win_open(Window *w, uint8_t *filename)
{
	w->active = true;
	w->content_active = true;
	w->dirty = false;
	w->tag_active = false;

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

void win_write(Window *w)
{
	// TODO(Julian): You probably need to make sure that
	// this string ends in a null terminator.
	char buf[256];
	int lsize, rsize;
	char *lptr, *rptr;
	GapBuffer *gb;

	SDL_RWops *filename = SDL_RWFromMem(buf, 256);
	if (filename) {
		gb = &w->tag;
		lsize = gb->gap;
		rsize = gb->size - lsize - gb->gapsize;
		lptr = gb->data;
		rptr = gb->data + lsize + gb->gapsize;

		SDL_RWwrite(filename, lptr, lsize, 1);
		SDL_RWwrite(filename, rptr, rsize, 1);
		SDL_RWclose(filename);

		SDL_RWops *newfile = SDL_RWFromFile(buf, "w+b");
		if (newfile) {
			gb = &w->content;
			lsize = gb->gap;
			rsize = gb->size - lsize - gb->gapsize;
			lptr = gb->data;
			rptr = gb->data + lsize + gb->gapsize;

			SDL_RWwrite(newfile, lptr, lsize, 1);
			SDL_RWwrite(newfile, rptr, rsize, 1);
			SDL_RWclose(newfile);

			w->dirty = false;
		} else {
			fprintf(stderr, "Error opening file: %s\n", SDL_GetError());
		}
	} else {
		fprintf(stderr, "Error opening file: %s\n", SDL_GetError());
	}
}

void win_keydown(Window *w, uint32_t type)
{
	if (!w->active) {
		return;
	}

	GapBuffer *gb = w->content_active ? &w->content : &w->tag;

	switch (type) {
		case SDLK_BACKSPACE: {
			gb_remove(gb);
		} break;
	}
}

void win_input(Window *w, char *text)
{
	char c;
	if (w->content_active) {
		while ((c = *text++) != '\0') {
			gb_insert(&w->content, c);
		}
		w->dirty = true;
	} else if (w->tag_active) {
		while ((c = *text++) != '\0') {
			gb_insert(&w->tag, c);
		}
		w->dirty = true;
	}
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
	
	gb_render(&win->content, r, mouse, rect_content, fg, win->content_active);

	// render the scrollbar
	SDL_Rect rect_scroll = {pos.x, pos.y, font_width-5, pos.h};
	SDL_SetRenderDrawColor(r, 153, 153, 76, 255);
	SDL_RenderDrawRect(r, &rect_scroll);

	int nlines = pos.h / font_height;
	double scrollp = (double)nlines / (double)win->content.nlines;
	int scrollh = scrollp*pos.h;

	SDL_Rect rect_scrollbar = {rect_scroll.x, rect_scroll.y, rect_scroll.w, scrollh};
	SDL_RenderFillRect(r, &rect_scrollbar);

	// render the tag
	SDL_Rect rect_tag = {pos.x, pos.y, pos.w, font_height};

	SDL_SetRenderDrawColor(r, 234, 255, 255, 255);
	SDL_RenderFillRect(r, &rect_tag);

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderDrawRect(r, &rect_tag);

	rect_tag.x += font_width;

	gb_render(&win->tag, r, mouse, rect_tag, fg, win->tag_active);
	
	// render the save button
	SDL_Rect rect_button = {pos.x, pos.y, font_width-5, font_height};

	SDL_SetRenderDrawColor(r, 136, 136, 204, 255);
	SDL_RenderFillRect(r, &rect_button);

	if (mouse.buttons[0].pressed) {
		if (SDL_PointInRect(&mouse.position, &rect_button)) {
			win_write(win);
		}

		win->tag_active = SDL_PointInRect(&mouse.position, &rect_tag);
		win->content_active = SDL_PointInRect(&mouse.position, &rect_content);
		win->active = win->tag_active || win->content_active;
	}

	if (win->dirty) {
		// render the dirty button
		SDL_Rect rect_dirty = {pos.x+3, pos.y+3, font_width-11, font_height-6};

		SDL_SetRenderDrawColor(r, 0, 0, 153, 255);
		SDL_RenderFillRect(r, &rect_dirty);
	}
}
